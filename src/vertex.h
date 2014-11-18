#ifndef DCEL_VERTEX_H
#define DCEL_VERTEX_H

#include "point3.h"
#include "vector3.h"
#include "halfedge.h"
#include "stlpache.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>

class dhalfedge;
class dedge;
class dface;
class dvertex;

class dvertex
{
public:
	struct __half_edge_iterator:public std::iterator<bidirectional_iterator_tag,dhalfedge,ptrdiff_t> 
	{
		typedef __half_edge_iterator self;
		typedef dhalfedge* link_edge_type;
		typedef dvertex* link_vertex_type;

		link_vertex_type vertex;
		link_edge_type half;

		__half_edge_iterator()
		{
			
		}
		__half_edge_iterator(link_vertex_type v):vertex(v),half(v->half)
		{

		}
		__half_edge_iterator(__half_edge_iterator const & x):vertex(x.vertex),half(x.half)
		{

		}
		bool operator==(const self& x) const 
		{ 
			return vertex==x.vertex&&half==x.half; 
		}
		bool operator!=(const self& x) const 
		{ 
			return vertex!=x.vertex||half!=x.half; 
		}
		reference operator*() const 
		{ 
			return *half; 
		}	
		pointer operator->() const 
		{ 
			return &(operator*()); 
		}
		self& operator++() 
		{ 
			if (half==NULL&&vertex->half!=NULL){
				half=vertex->half;
			}else{
				half = half->pair->next==vertex->half?NULL:half->pair->next;
			}
			
			return *this;
		}
		self operator++(int) 
		{ 
			self tmp = *this;
			++*this;
			return tmp;
		}
		self& operator--() 
		{ 
			if (half==NULL&&vertex->half!=NULL){
				half = vertex->half->previous->pair;
			}else{
				half = half==vertex->half?NULL:half->previous->pair; 
			}
			return *this;
		}
		self operator--(int) 
		{ 
			self tmp = *this;
			--*this;
			return tmp;
		}
	};
public:
	typedef __half_edge_iterator halfedge_iterator;
	halfedge_iterator half_begin()
	{ 
		return start;
	}
	halfedge_iterator half_end()
	{
		return finish;
	}
public:
    dvertex();
    dvertex(dvertex const& v);
	~dvertex();
    dvertex& operator=(dvertex const& v);

public:
    operator std::string() const;
    friend std::istream& operator>>(std::istream& in,dvertex& v);
    friend std::ostream& operator<<(std::ostream& out,dvertex const& v);
    friend std::ostream& operator<<(std::ostream& out,dvertex const* v);

private:
    void init_from_string(std::string const& line);
	void initialize_end() 
	{
		finish.vertex=this;
		finish.half=NULL;
	}
	void initialize_beg() 
	{
		start.vertex = this;
		start.half = half;
	}
public:
	bool is_picked;
	int id;
    point3f coords;
    vector3f normal;
	dhalfedge* half;

private:
	halfedge_iterator start;
	halfedge_iterator finish;
private:
	friend class dsolid;
	friend class dhalfedge;
	friend class dface;
	friend class dedge;
};

#endif