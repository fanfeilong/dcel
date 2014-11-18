#ifndef DCEL_SOLID_H
#define DCEL_SOLID_H

#include "point3.h"
#include "vector3.h"
#include "stlpache.h"
#include "3ds.h"

#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace stlpache;
using namespace std;

#define MAX_LINE 2048
#define PI 3.1415926535897932

class dvertex;
class dhalfedge;
class dedge;
class dface;

class dsolid
{
public:
    typedef map<int,dvertex*>::iterator   vertex_iterator;
    typedef map<int,dhalfedge*>::iterator halfedge_iterator;
	typedef map<int,dedge*>::iterator     edge_iterator;
    typedef map<int,dface*>::iterator     face_iterator;

public:
	vertex_iterator vertex_begin(){return _vertexs.begin();}
	vertex_iterator vertex_end(){return _vertexs.end();}
	halfedge_iterator half_edge_begin(){return _half_edges.begin();}
	halfedge_iterator half_edge_end(){return _half_edges.end();}
	edge_iterator edge_begin(){return _edges.begin();}
	edge_iterator edge_end(){return _edges.end();}
	face_iterator face_begin(){return _faces.begin();}
	face_iterator face_end(){return _faces.end();}

public:
    dsolid():_is_suspended(true),_is_refreshed(true),_is_new_file(false),
        _center(0,0,0),_width(1),_height(1),_is_empty(true)
    {

    }
    ~dsolid()
    {
        clear();
    }
	dsolid(dsolid const & rhs)
	{
		_is_suspended = rhs._is_suspended;
		_is_refreshed = rhs._is_refreshed;
		_is_new_file = rhs._is_new_file;
		_is_empty = rhs._is_empty;
		_vertexs = rhs._vertexs;
		_half_edges = rhs._half_edges;
		_edges = rhs._edges;
		_faces = rhs._faces;
		_center = rhs._center;
		_width = rhs._width;
		_height = rhs._height;
		_file_name = rhs._file_name;
		_name = rhs._name;
	}
	dsolid& operator=(dsolid const& rhs)
	{
		if (&rhs==this)
		{
			return *this;
		}
		_is_suspended = rhs._is_suspended;
		_is_refreshed = rhs._is_refreshed;
		_is_new_file = rhs._is_new_file;
		_is_empty = rhs._is_empty;
		_vertexs = rhs._vertexs;
		_half_edges = rhs._half_edges;
		_edges = rhs._edges;
		_faces = rhs._faces;
		_center = rhs._center;
		_width = rhs._width;
		_height = rhs._height;
		_file_name = rhs._file_name;
		_name = rhs._name;
		return	*this;
	}
	bool operator<(dsolid const& rhs) const
	{
		return _id<rhs._id;
	}

public:
    void load_from_off_file(string fileName);
    void load_from_m_file(string fileName);
	void load_from_txt_file(string fileName);
	void load_from_3ds_object(t3DObject);
	void load_from_3ds_file(string fileName);
	void load(string path_);
	void save_to_m_file(string fileName);

    void test_solid();
    void loop_subdivision();
    void catmull_clark_subdivision();
    void modified_butterfly_subdivision();
    void doo_sabin_subdivision();
    void caculate_vertex_normal();
	void turn_to_trimesh();

private:
    void deal_face_data(dface *, vector<int> &vertex_table,map<int, vector<dhalfedge*> > &vertex_half_dic);
    void creat_edge_and_pair(map<int,vector<dhalfedge*> > &vertex_half_dic);
	void butterfly_singularity(dhalfedge*, point3f& coords, vector3f& normal);

public:
    void begin_suspend()
    {
		_is_empty = true;
        _is_suspended = true;
    }
    void end_suspend()
    {
        caculate_center_parameter();
        _is_suspended = false;
		_is_empty = false;
    }
    bool is_suspended() const
    {
        return _is_suspended;
    }
    bool is_refreshed()
    {
        if(_is_refreshed)
        {
            _is_refreshed=false;
            return true;
        }
        else
        {
            return false;
        }
    }
    bool is_new_file()
    {
        if(_is_new_file)
        {
            _is_new_file=false;
            return true;
        }
        else
        {
            return false;
        }
    }
    void clear()
    {
	    _is_empty = true;
        delete_values(&_vertexs);
        delete_values(&_half_edges);
        delete_values(&_edges);
        delete_values(&_faces);
    }

private:
	bool is_empty()
	{
		return _is_empty;
	}
    bool is_face_tag( string tag )
    {
        return tag=="Face";
    }
    bool is_vertex_tag( string tag )
    {
        return tag=="Vertex";
    }
    void reset_center_parameter()
    {
        //reset center
        _center.zero();
        _width=1.0;
        _height=1.0;

        _is_refreshed = true;
    }
    void caculate_center_parameter();

public:
    vector3f center()
    {
        return _center;
    }
    float width()
    {
        return _width;
    }
    float height()
    {
        return _height;
    }
	string path()
	{
		return _file_name;
	}
	int id(){return _id;}
	void set_id(int id_){_id=id_;}

private:
    bool       _is_suspended;
    bool       _is_refreshed;
    bool       _is_new_file;
	bool       _is_empty;

    map<int,dvertex*>     _vertexs;
    map<int,dhalfedge*>   _half_edges;
    map<int,dedge*>       _edges;
    map<int,dface*>       _faces;

    vector3f  _center;
    float     _width;
    float     _height;
	string    _file_name;
	string    _name;
	int _id;
};

#endif