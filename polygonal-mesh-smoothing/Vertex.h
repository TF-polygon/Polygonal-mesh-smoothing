#ifndef __VERTEX_H__
#define __VERTEX_H__

#pragma once
#include "Vec3.h"
#include <vector>

using namespace std;

class Face;
class Edge;
class Vertex
{
public:
	int				_index;
	bool			_flag;			// Prevant duplicate operations
	Vec3<double>	_pos;			// X, Y, Z
	Vec3<double>	_pos0;
	Vec3<double>	_normal;
	vector<Face*>	_nbFaces;		// Neighbor Face
	vector<Vertex*>	_nbVertices;	// Neighbor Vertex
	vector<Edge*>	_nbEdges;		// Neighbor Edge

public:
	Vertex();
	Vertex(int index, Vec3<double> p)
	{
		_flag	= false;
		_index  = index;
		_pos	= p;
		_pos0	= p;
	}
	~Vertex();
public:
	inline void x(double value) { _pos[0] = value; }
	inline void y(double value) { _pos[1] = value; }
	inline void z(double value) { _pos[2] = value; }
	inline double x(void) { return _pos[0]; }
	inline double y(void) { return _pos[1]; }
	inline double z(void) { return _pos[2]; }

public:
	bool hasNbVertex(Vertex* v);
};

#endif