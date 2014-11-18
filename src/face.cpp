#include "vertex.h"
#include "halfedge.h"
#include "face.h"

dface::dface():id(-1)
{
    half = NULL;
	initialize_beg();
	initialize_end();
}
dface::~dface()
{

}
dface::dface(dface const& p)
{
    id = p.id;
    half = p.half;
	initialize_beg();
	initialize_end();
}
dface& dface::operator=(dface const& p)
{
    if(&p==this)
        return *this;
    id = p.id;
    half = p.half;
	initialize_beg();
	initialize_end();
    return *this;
}

dface::operator std::string() const
{
    std::stringstream sstr;
    sstr<<"Face"<<" "
        <<id<<" ";

    dhalfedge* current_half_edge=half;
    do
    {
        sstr<<current_half_edge->origin->id<<" ";
        current_half_edge=current_half_edge->next;
    }
    while (current_half_edge!=half);

    return sstr.str();
}


std::ostream& operator<<(std::ostream& out,dface const& v)
{
    out<<static_cast<string>(v);
    return out;
}
std::ostream& operator<<(std::ostream& out,dface const* v)
{
    out<<static_cast<string>(*v);
    return out;
}