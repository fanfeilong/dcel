#include "vertex.h"
#include "edge.h"
#include "face.h"
#include "halfedge.h"

dhalfedge::dhalfedge():id(-1)
{
    next     = NULL;
    previous = NULL;
    pair     = NULL;
    origin   = NULL;
    edge    = NULL;
    left     = NULL;
}
dhalfedge::~dhalfedge()
{

}

dhalfedge::dhalfedge(dhalfedge const& half)
{
    id       = half.id;
    next     = half.next;
    previous = half.previous;
    pair     = half.pair;
    origin   = half.origin;
    edge    = half.edge;
    left     = half.left;
}
dhalfedge& dhalfedge::operator=(dhalfedge const& half)
{
    if (&half==this)
        return *this;
	id       = half.id;
    next     = half.next;
    previous = half.previous;
    pair     = half.pair;
    origin   = half.origin;
    edge    = half.edge;
    left     = half.left;

    return *this;
}

dvertex*& dhalfedge::destination()
{
    return next->origin;
}
dhalfedge*& dhalfedge::vertex_next()
{
	return pair->next;
}
dhalfedge*& dhalfedge::vertex_previous()
{
	return previous->pair;
}