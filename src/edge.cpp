
#include "halfedge.h"
#include "edge.h"

#include <cstddef>

dedge::dedge():id(-1)
{
    half = NULL;
}
dedge::~dedge()
{

}
dedge::dedge(dedge const & e)
{
    id=e.id;
    half=e.half;
}
dedge& dedge::operator=(dedge const& e)
{
    if (&e==this)
    {
        return *this;
    }
	id=e.id;
    half=e.half;
    return *this;
}
