#ifndef __MESH_H__
#define __MESH_H__

#include "Face.h"
#include "Edge.h"
#include "GL/glut.h"

#pragma once
class Mesh
{
public:
	Vec3<double>	_minPos;
	Vec3<double>	_maxPos;
	vector<Vertex*>	_vertices;
	vector<Face*>	_faces;
	vector<Edge*>	_edges;

public:
	Mesh(const char* filename) { open(filename); }
	~Mesh(void) {}

public:
	void	init(void);
	void	open(const char* filename);
	void	reset(void);
	void	BuildList(void);
	void	moveToCenter(double scaling);
	void	computeNormal(void);
	void	computeEdge(void);
	void	computeCotWeight(void);

public:
	void	drawPoint(void);
	void	drawSurface(bool smooth = false);
	void	drawWireframe(void);

public:
	void	VerticesAverageSmoothing(int iter = 1);
	void	LaplacianSmoothing(int iter = 1);
	void	CotangentLaplacianSmoothing(int iter = 1);
};

#endif