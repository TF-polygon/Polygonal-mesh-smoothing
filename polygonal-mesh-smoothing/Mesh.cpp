#include "Mesh.h"

void Mesh::open(const char* filename)
{
	FILE* fp;
	char header[256] = { 0 };
	double pos[3];
	int index = 0;
	_minPos.Set(100000.0);
	_maxPos.Set(-100000.0);

	fopen_s(&fp, filename, "r");
	while (fscanf(fp, "%s %lf %lf %lf", header, &pos[0], &pos[1], &pos[2]) != EOF) {
		if (header[0] == 'v' && header[1] == NULL) {
			for (int i = 0; i < 3; i++) {
				if (_minPos[i] > pos[i]) _minPos[i] = pos[i];
				if (_maxPos[i] < pos[i]) _maxPos[i] = pos[i];
			}
			_vertices.push_back(new Vertex(index++, Vec3<double>(pos[0], pos[1], pos[2])));
		}
	}

	index = 0;
	int indices[3];
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %d %d %d", header, &indices[0], &indices[1], &indices[2]) != EOF) {
		if (header[0] == 'f' && header[1] == NULL) {
			auto v0 = _vertices[indices[0] - 1];
			auto v1 = _vertices[indices[1] - 1];
			auto v2 = _vertices[indices[2] - 1];
			_faces.push_back(new Face(index++, v0, v1, v2));
		}
	}
	moveToCenter(10.0f);
	BuildList();
	computeNormal();
	computeCotWeight();
	fclose(fp);

	printf("Vertices number : [%d]\n", _vertices.size());
	printf("Faces number : [%d]\n", _faces.size());
}

void Mesh::moveToCenter(double scaling)
{
	auto crossVec = _maxPos - _minPos;
	double maxLength = fmax(fmax(crossVec.x(), crossVec.y()), crossVec.z());
	auto center = (_maxPos + _minPos) / 2.0;
	Vec3<double> newCenter(0.0, 0.0, 0.0);

	for (auto v : _vertices) {
		auto pos = v->_pos;
		auto grad = pos - center;
		grad /= maxLength;
		grad *= scaling;
		pos = newCenter;
		pos += grad;
		v->_pos = pos;
	}
}

void Mesh::init(void)
{
	for (auto v : _vertices)
		v->_flag = false;
}

void Mesh::BuildList(void)
{
	// vertex-face
	for (auto f : _faces) {
		for (auto fv : f->_vertices) {
			fv->_nbFaces.push_back(f);
		}
	}

	// vertex-vertex
	for (auto v : _vertices) {
		for (auto nf : v->_nbFaces) {
			int index = nf->getIndex(v);
			v->_nbVertices.push_back(nf->v((index + 1) % 3));
			v->_nbVertices.push_back(nf->v((index + 3 - 1) % 3));
		}
	}

	computeEdge();

	// edge-face
	for (auto e : _edges) {
		auto v = e->v(0);
		for (auto nf : v->_nbFaces) {
			if (nf->hasVertex(e->v(0)) && nf->hasVertex(e->v(1))) {
				if (!nf->hasEdge(e)) {
					nf->_edges.push_back(e);
				}
				if (!e->hasFace(nf)) {
					e->_nbFaces.push_back(nf);
				}
			}
		}
	}

	printf("Build adjacency list\n");
}

void Mesh::computeNormal(void)
{
	// face normal
	for (auto f : _faces) {
		auto v0 = f->v(1)->_pos - f->v(0)->_pos;
		auto v1 = f->v(2)->_pos - f->v(0)->_pos;
		f->_normal = v0.Cross(v1);
		f->_normal.Normalize();
	}

	// vertex normal
	for (auto v : _vertices) {
		v->_normal.Clear();
		for (auto nf : v->_nbFaces) {
			v->_normal += nf->_normal;
		}
		v->_normal /= (double)v->_nbFaces.size();
	}
}

void Mesh::computeEdge(void)
{
	int index = 0;
	for (auto v : _vertices) {
		for (auto nv : v->_nbVertices) {
			if (!nv->_flag) {
				_edges.push_back(new Edge(index++, v, nv));
			}
		}

		v->_flag = true;
	}
	init();
	printf("Edges number : [%d]\n", _edges.size());
}

void Mesh::computeCotWeight(void)
{
	for (auto e : _edges) {
		if (e->_nbFaces.size() == 2) {
			double weight = 0.0f;
			
			auto other_v = e->_nbFaces[0]->getOtherVertex(e);
			auto dir0 = e->v(0)->_pos - other_v->_pos;
			auto dir1 = e->v(1)->_pos - other_v->_pos;
			dir0.Normalize();
			dir1.Normalize();

			double angle = acos(dir0.Dot(dir1));
			weight += 1.0f / tan(angle);

			other_v = e->_nbFaces[1]->getOtherVertex(e);
			dir0 = e->v(0)->_pos - other_v->_pos;
			dir1 = e->v(1)->_pos - other_v->_pos;
			dir0.Normalize();
			dir1.Normalize();

			angle = acos(dir0.Dot(dir1));
			weight += 1.0f / tan(angle);

			e->_cotWeight = weight;
		}
	}
}

void Mesh::reset(void)
{
	for (auto v : _vertices) {
		v->_pos = v->_pos0;
	}
	moveToCenter(10.0f);
	computeNormal();
}

void Mesh::VerticesAverageSmoothing(int iter)
{
	vector<Vec3<double>> newPos;

	for (int it = 0; it < iter; it++) {
		for (auto v : _vertices) {
			auto pos = v->_pos;
			for (auto nv : v->_nbVertices) {
				pos += nv->_pos;
			}
			pos /= v->_nbVertices.size() + 1;
			newPos.push_back(pos);
		}

		for (int i = 0; i < _vertices.size(); i++) {
			auto v = _vertices[i];
			v->_pos = newPos[i];
		}
	}
}

void Mesh::LaplacianSmoothing(int iter)
{
	double lambda = 0.25f;
	int numVertices = _vertices.size();

	for (int it = 0; it < iter; it++) {
		for (int i = 0; i < numVertices; i++) {
			auto v = _vertices[i];
			auto pos = v->_pos;
			Vec3<double> sumNbPos;
			for (auto nv : v->_nbVertices) {
				sumNbPos += nv->_pos;
			}
			int numNbVertices = v->_nbVertices.size();
			auto newPos = (sumNbPos / numNbVertices - pos) * lambda + pos;
			_vertices[i]->_pos = newPos;
		}
	}
	computeNormal();
}

void Mesh::CotangentLaplacianSmoothing(int iter)
{
	double lambda = 0.25f;
	int numVertices = _vertices.size();

	for (int it = 0; it < iter; it++) {
		for (int i = 0; i < numVertices; i++) {
			auto v = _vertices[i];
			auto pos = v->_pos;

			Vec3<double> sumNbPos;
			double sumCotWeight = 0.0f;

			for (auto ne : v->_nbEdges) {
				auto pos0 = ne->v(0)->_pos;
				auto pos1 = ne->v(1)->_pos;
				double cotWeight = ne->_cotWeight;

				if (v == ne->v(0)) {
					sumNbPos += (pos1 - pos0) * cotWeight;
				}
				else {
					sumNbPos += (pos0 - pos1) * cotWeight;
				}
				sumCotWeight += cotWeight;
			}
			auto newPos = (sumNbPos / sumCotWeight) * lambda + pos;
			_vertices[i]->_pos = newPos;
		}
	}
	computeNormal();
}

void Mesh::drawPoint(void)
{
	glPushMatrix();
	glEnable(GL_LIGHTING);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (auto v : _vertices) {
		glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
		glVertex3f(v->x(), v->y(), v->z());
	}
	glEnd();
	glPointSize(1.0f);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Mesh::drawWireframe(void)
{
	glPushMatrix();
	glEnable(GL_LIGHTING);
	glBegin(GL_LINES);
	for (auto f : _faces) {
		for (int j = 0; j < 3; j++) {
			auto v0 = f->v((j + 1) % 3);
			auto v1 = f->v((j + 2) % 3);
			auto n0 = f->v((j + 1) % 3)->_normal;
			auto n1 = f->v((j + 2) % 3)->_normal;
			glNormal3f(n0.x(), n0.y(), n0.z());
			glVertex3f(v0->x(), v0->y(), v0->z());

			glNormal3f(n1.x(), n1.y(), n1.z());
			glVertex3f(v1->x(), v1->y(), v1->z());
		}
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Mesh::drawSurface(bool smooth)
{
	glPushMatrix();
	glEnable(GL_LIGHTING);
	smooth == true ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);
	for (auto f : _faces) {
		glBegin(GL_TRIANGLES);
		if (!smooth)
			glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
		for (int j = 0; j < 3; j++) {
			auto v = f->v(j);
			if (smooth)
				glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
			glVertex3f(v->x(), v->y(), v->z());
		}
		glEnd();
	}
	glEnable(GL_LIGHTING);
	glPopMatrix();
}