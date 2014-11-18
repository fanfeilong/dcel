#ifndef DCEL_FACE_H
#define DCEL_FACE_H

#include <iostream>
#include <sstream>

class dhalfedge;

class dface
{
public:
	struct __half_edge_iterator:public std::iterator<bidirectional_iterator_tag,dhalfedge,ptrdiff_t>
	{
		typedef __half_edge_iterator self;
		typedef dhalfedge* link_edge_type;
		typedef dface* link_face_type;

		link_face_type face;
		link_edge_type half;

		__half_edge_iterator()
		{

		}
		__half_edge_iterator(link_face_type v):face(v),half(v->half)
		{

		}
		__half_edge_iterator(__half_edge_iterator const & x):face(x.face),half(x.half)
		{

		}
		bool operator==(const self& x) const 
		{ 
			return face==x.face&&half==x.half; 
		}
		bool operator!=(const self& x) const 
		{ 
			return face!=x.face||half!=x.half; 
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
			if (half==NULL&&face->half!=NULL){
				half=face->half;
			}else{
				half = half->next==face->half?NULL:half->next;
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
			if (half==NULL&&face->half!=NULL){
				half = face->half->previous;
			}else{
				half = half==face->half?NULL:half->previous; 
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
	halfedge_iterator half_begin(){return start;}
	halfedge_iterator half_end(){return finish;}
public:
    dface();
    dface(dface const& p);
    dface& operator=(dface const& p);
    ~dface();
public:
    operator std::string() const;
    friend std::ostream& operator<<(std::ostream& out,dface const& v);
    friend std::ostream& operator<<(std::ostream& out,dface const* v);
	
private:
	void initialize_beg()
	{
		start.face = this;
		start.half = half;
	}
	void initialize_end()
	{
		finish.face = this;
		finish.half = half;
	}

public:
    int id;
    dhalfedge* half;

private:
	halfedge_iterator start;
	halfedge_iterator finish;
};
#endif