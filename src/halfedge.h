#ifndef DCEL_HALFEDGE_H
#define DCEL_HALFEDGE_H

class dvertex;
class dedge;
class dface;

class dhalfedge
{
public:
    dhalfedge();
    dhalfedge(dhalfedge const& half);
    dhalfedge& operator=(dhalfedge const& half);
    ~dhalfedge();
    
	dvertex*& destination();
	dhalfedge*& vertex_next();
	dhalfedge*& vertex_previous();

public:
    //pointer
    dhalfedge* next;
    dhalfedge* previous;
    dhalfedge* pair;

    //data
    int id;
    dvertex* origin;
    dedge*  edge;
    dface*   left;

    //typedef __dcel_half_edge_iterator iterator;
};
#endif