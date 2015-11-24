#include <iostream>
#include <string>

#include "vertex.h"
#include "halfedge.h"
#include "edge.h"
#include "face.h"
#include "algorithm.h"
#include "solid.h"

using namespace std;

void dcel_test_begin( string const &str ) 
{
	cout<<endl;
	string underline(str.size()+4,'-');
	cout<<underline<<endl;
	cout<<"=="<<str<<"=="<<endl;
	cout<<underline<<endl;
}
void dcel_test_end()
{
	cout<<endl;
}
void dcel_test_dsolid_vertex_iterator( dsolid* solid ) 
{
	dcel_test_begin("test the dsolid::vertex_iterator");
	for(auto it=solid->vertex_begin();it!=solid->vertex_end();++it)
	{
		int id=it->first;
		dvertex vertex=*(it->second);
		cout<<vertex<<endl;
	}
	dcel_test_end();
}
void dcel_test_dsolid_halfedge_iterator( dsolid* solid ) 
{
	dcel_test_begin("test the dsolid::halfedge_iterator");
	for(auto it=solid->half_edge_begin();it!=solid->half_edge_end();++it)
	{
		int id=it->first;
		dhalfedge* half = it->second;
		cout<<half->origin<<endl;
	}
	dcel_test_end();
}
void dcel_test_dsolid_edge_iterator( dsolid* solid ) 
{
	dcel_test_begin("test the dsolid::edge_iterator");
	for(auto it=solid->edge_begin();it!=solid->edge_end();++it)
	{
		int id=it->first;
		dedge* edge = it->second;
		cout<<edge->half->origin<<endl;
	}
	dcel_test_end();
}
void dcel_test_dsolid_face_iterator( dsolid* solid ) 
{
	dcel_test_begin("test the dsolid::face_iterator");
	for(auto it=solid->face_begin();it!=solid->face_end();++it)
	{
		int id=it->first;
		dface* face = it->second;
		cout<<face->half->origin<<endl;
	}
	dcel_test_end();
}

void dcel_test_dvertex_halfedge_iterator( dsolid::vertex_iterator vbeg ) 
{
	dcel_test_begin("dvertex::halfedge_iterator");
	dvertex* vertex_ptr = vbeg->second; 
	dvertex vertex = *vertex_ptr;
	for (dvertex::halfedge_iterator hit=vertex.half_begin();hit!=vertex.half_end();++hit)
	{
		dhalfedge half = *hit;
		cout<<half.id<<endl;
	}
	cout<<"---------"<<endl;
	dhalfedge* temp_half = vertex.half;
	do 
	{
		cout<<temp_half->id<<endl;
		temp_half = temp_half->pair->next;
	} while (temp_half!=vertex.half);
	dcel_test_end();
}


int main(int argc, char** argv)
{
	//get the data file
	string file_name(argv[0]);
	file_name = file_name.substr(0,file_name.find_last_of("/\\"))
		.append("\\..\\data\\m_files\\example_cubic.m");

	//creat a solid instance and load the data file
	dsolid* solid = new dsolid();
	solid->load_from_m_file(file_name);
	for(int i=0;i<5;i++){
		solid->catmull_clark_subdivision();
	}
	//do some test for the solid
	dcel_test_dsolid_vertex_iterator(solid);
	dcel_test_dsolid_halfedge_iterator(solid);
	dcel_test_dsolid_edge_iterator(solid);
	dcel_test_dsolid_face_iterator(solid);
	dcel_test_dvertex_halfedge_iterator(solid->vertex_begin());

	//exit 
	system("PAUSE");
	return 0;
}