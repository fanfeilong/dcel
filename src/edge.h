#ifndef DCEL_EDGE_H
#define DCEL_EDGE_H

class dhalfedge;

class dedge
{
public:
    dedge();
    dedge(dedge const & e);
    dedge& operator=(dedge const& e);
    ~dedge();
public:
    int id;
    dhalfedge* half;
};
#endif