#include "solid.h"
#include "vertex.h"
#include "edge.h"
#include "halfedge.h"
#include "face.h"
#include "stlpache.file.h"
#include "3ds.h"

#include <fstream>
#include <sstream>
#include <list>
#include <queue>

//----------------------------//
//aux methods
//----------------------------//
bool compare_vetex_by_id(std::pair<int, dvertex*> i, std::pair<int, dvertex*> j)
{
    return i.second->id < j.second->id;
}
template <int I>
struct less_by_vertex_i_coord
{
    bool operator()(dvertex* lhs, dvertex* rhs ) const
    {
        return lhs->coords[I]< rhs->coords[I];
    }
};
void split_path(const char *szfullfilename, char *szpathname, char *szfilename, char *szextname)
{
	int i, j;

	i = 0;
	while (szfullfilename[i] != '\0')
		i++;
	while (szfullfilename[i] != '\\')
		i--;

	for (j = 0; j <i; j++)
	{
		szpathname[j] = szfullfilename[j];
	}
	szpathname[j] = '\0';

	j = 0;
	i++;
	while((szfilename[j] = szfullfilename[i]) != '\0')
	{
		i++;
		j++;
	}

	while (szfullfilename[i] != '.')
		i--;

	j = 0;
	i++;
	while((szextname[j] = szfullfilename[i]) != '\0')
	{
		j++;
		i++;
	}
}


//----------------------------//
//aux end
//----------------------------//

void dsolid::load_from_off_file(string fileName)
{
    begin_suspend();
    clear();
    _is_new_file = true;
	this->_file_name = fileName;

    map<int,vector<dhalfedge*> > vertex_half_dic;

    std::ifstream fin(fileName);
    istream_line_iterator beg(fin);
    istream_line_iterator end;

    int num_vertices=0, num_faces=0, num_edges=0;
    for (; beg!=end; ++beg)
    {
        line str_line=*beg;
        trim(str_line.data);
        if (str_line.data==""||str_line.data=="OFF"||str_line.data[0]=='#')
        {
            continue;
        }

        string_token_iterator str_it(str_line.data," \n");
        string_token_iterator str_end;

        //string tag = *str_it;
        num_vertices = atoi((*str_it++).c_str());
        num_faces = atoi((*str_it++).c_str());
        num_edges = atoi((*str_it++).c_str());
        if (str_it!=str_end)
        {
            debug("NVERTICES  NFACES  NEDGES maybe wrong");
        }
        break;
    }
    beg++;
    for(int vertex_id = 1; vertex_id <= num_vertices && beg!=end; ++beg, ++vertex_id)
    {
        line str_line=*beg;
        trim(str_line.data);
        if (str_line.data==""||str_line.data[0]=='#')
        {
            continue;
        }

        string_token_iterator str_it(str_line.data," \n");
        string_token_iterator str_end;

        //create vertex
        dvertex* vertex = new dvertex();
        string str;
        vertex->id=vertex_id;
        //Read Coords
        for(int i=0; i<3; i++)
        {
            str=*(str_it++);
            vertex->coords[i]=static_cast<float>(atof(str.c_str()));
        }
        //store vertex
        _vertexs[vertex->id]=vertex;
    }
    //
    //create half edge and face
    //update vertex, half edge and face
    //
    for(int face_id = 1; face_id <= num_faces && beg!=end; ++beg, ++face_id)
    {
        line str_line=*beg;
        trim(str_line.data);
        if (str_line.data==""||str_line.data[0]=='#')
        {
            continue;
        }

        string_token_iterator str_it(str_line.data," \n");
        string_token_iterator str_end;

        string str = *str_it++;
        //debug(str);
        int num_face_vertices=atoi(str.c_str());

        //get the face vertex id table
        vector<int> vertex_table;
        for (int i = 0; i < num_face_vertices&&str_it!=str_end; ++str_it, ++i)
        {
            str=*str_it;
            vertex_table.push_back(atoi(str.c_str())+1);
        }
		dface* face = new dface;
		face->id = face_id;

		deal_face_data(face, vertex_table, vertex_half_dic);

    }
    creat_edge_and_pair(vertex_half_dic);

    end_suspend();
}
void dsolid::load_from_txt_file(string fileName)
{
	begin_suspend();
    clear();
    _is_new_file = true;
	this->_file_name = fileName;

	typedef pair<int , int> position;
	map<position, dvertex*> txt_vertexs;
	map<int,vector<dhalfedge*> > vertex_half_dic;

    std::ifstream fin(fileName);
    istream_line_iterator beg(fin);
    istream_line_iterator end;
	unsigned int vertex_id=1, face_id=1;
    for (;beg!=end; ++beg)
    {
        string str_line=(*beg).data;
        trim(str_line);
        if (str_line=="")
        {
            continue;
        }
		
		//create vertex
        dvertex* vertex = new dvertex();
		//read position
		int nbeg = str_line.find_first_of("<");
		int nmid = str_line.find_first_of(",");
		int nend = str_line.find_first_of(">");
		if (nend>0)
		{
			int x_position = atoi(str_line.substr( nbeg+1, nmid-nbeg-1 ).c_str());
			int y_position = atoi(str_line.substr( nmid+1, nend-nmid-1 ).c_str());
			position current_position;

			current_position.first = x_position;
			current_position.second = y_position;
			txt_vertexs[current_position]=vertex;
		}

		//read coordinate 
		string str_cood = str_line.substr(nend+1);
		string_token_iterator str_it(str_cood," \n");
		string_token_iterator str_end;
        vertex->id=vertex_id;
		string str;
        for(int i=0; i<3; i++)
        {
            str=*str_it;
            vertex->coords[i]=static_cast<float>(atof(str.c_str()));
			str_it++;
        }
        //store vertex
        _vertexs[vertex->id]=vertex;
		vertex_id++;
    }
	for (auto it = txt_vertexs.begin(); it != txt_vertexs.end(); ++it)
	{
		int x_position = it->first.first;
		int y_position = it->first.second;
		dvertex * left=NULL ,* down = NULL,* left_down = NULL;
		auto fit = txt_vertexs.find(position(x_position+1, y_position));
		if (fit != txt_vertexs.end())
		{
			left = (*fit).second;
		}
		fit = txt_vertexs.find(position(x_position+1, y_position+1));
		if (fit != txt_vertexs.end())
		{
			left_down = (*fit).second;
		}
		fit = txt_vertexs.find(position(x_position, y_position+1));
		if (fit != txt_vertexs.end())
		{
			down = (*fit).second;
		}
		if (left && down && left_down)
		{
			vector<int> vertex_table;
			vertex_table.push_back(it->second->id);
			vertex_table.push_back(left->id);
			vertex_table.push_back(left_down->id);
			vertex_table.push_back(down->id);

			dface* face = new dface;
			face->id = face_id++;
			deal_face_data(face, vertex_table, vertex_half_dic);
		}
	}
	creat_edge_and_pair(vertex_half_dic);
	end_suspend();
}
void dsolid::load_from_3ds_object(t3DObject object3DS)
{
	begin_suspend();
	clear();
	_is_new_file = true;

	int new_vertex_id=0;
	int new_face_id=0;
	int first_vertex_id=1;
	map<int,vector<dhalfedge*> > vertex_half_dic;

	int numOfVerts = object3DS.numOfVerts;
	for (int indexOfVerts=0; indexOfVerts < numOfVerts; indexOfVerts++)
	{
		//create vertex
		dvertex* vertex = new dvertex();
		vertex->id = ++new_vertex_id;
		vertex->coords = point3f(object3DS.pVerts[indexOfVerts].x(),object3DS.pVerts[indexOfVerts].y(),object3DS.pVerts[indexOfVerts].z());
		//store vertex
		_vertexs[vertex->id]=vertex;
	}
	int numOfFaces = object3DS.numOfFaces;
	for (int indexOfFaces=0; indexOfFaces < numOfFaces; indexOfFaces++)
	{
		int face_id=++new_face_id;

		//get the face vetex id table
		vector<int> vertex_table;
		vertex_table.push_back(object3DS.pFaces[indexOfFaces].vertIndex[0]+first_vertex_id);
		vertex_table.push_back(object3DS.pFaces[indexOfFaces].vertIndex[1]+first_vertex_id);
		vertex_table.push_back(object3DS.pFaces[indexOfFaces].vertIndex[2]+first_vertex_id);

		dface* face = new dface;
		face->id = face_id;
		deal_face_data(face, vertex_table, vertex_half_dic);
	}
	//first_vertex_id+=numOfVerts;
	creat_edge_and_pair(vertex_half_dic);

	end_suspend();
}
void dsolid::load_from_3ds_file(string fileName)
{
	begin_suspend();
	clear();
	_is_new_file = true;

	C3DSModel	object3DS;
	object3DS.Load(fileName.c_str());

	int new_vertex_id=0;
	int new_face_id=0;
	int first_vertex_id=1;
	map<int,vector<dhalfedge*> > vertex_half_dic;
	for (auto it = object3DS.m_3DModel.pObject.begin(); it!=object3DS.m_3DModel.pObject.end(); it++)
	{
		int numOfVerts = it->numOfVerts;
		for (int indexOfVerts=0; indexOfVerts < numOfVerts; indexOfVerts++)
		{
			//create vertex
			dvertex* vertex = new dvertex();
			vertex->id = ++new_vertex_id;
			vertex->coords = point3f(it->pVerts[indexOfVerts].x(),it->pVerts[indexOfVerts].y(),it->pVerts[indexOfVerts].z());
			//5dq也不是从3ds文件中读取法向，而是计算法向，他不是用dcel，计算法向效率不高，注释掉了他的计算法向也就不用读取法向信息了
			//vertex->normal = Vector3f(it->pNormals[indexOfVerts].x,it->pNormals[indexOfVerts].y,it->pNormals[indexOfVerts].z);
			//store vertex
			_vertexs[vertex->id]=vertex;
		}
		int numOfFaces = it->numOfFaces;
		for (int indexOfFaces=0; indexOfFaces < numOfFaces; indexOfFaces++)
		{
			int face_id=++new_face_id;

			//get the face vetex id table
			vector<int> vertex_table;
			vertex_table.push_back(it->pFaces[indexOfFaces].vertIndex[0]+first_vertex_id);
			vertex_table.push_back(it->pFaces[indexOfFaces].vertIndex[1]+first_vertex_id);
			vertex_table.push_back(it->pFaces[indexOfFaces].vertIndex[2]+first_vertex_id);

			dface* face = new dface;
			face->id = face_id;

			deal_face_data(face, vertex_table, vertex_half_dic);
		}
		first_vertex_id+=numOfVerts;

	}

	creat_edge_and_pair(vertex_half_dic);

	end_suspend();
}
void dsolid::load_from_m_file(string fileName)
{
	debug("load m file:" + fileName);

    begin_suspend();
    clear();
    _is_new_file = true;

    map<int,vector<dhalfedge*> > vertex_half_dic;

    std::ifstream fin(fileName);
    istream_line_iterator beg(fin);
    istream_line_iterator end;

    for(; beg!=end; ++beg)
    {
        line str_line=*beg;
        trim(str_line.data);
        if (str_line.data=="")
        {
            continue;
        }

        string_token_iterator str_it(str_line.data," \n");
        string_token_iterator str_end;

        string tag = *str_it;

        //
        //create vertex
        //
        if(is_vertex_tag(tag))
        {
            //create vertex
            dvertex* vertex = new dvertex();
            vertex->init_from_string(str_line);

            //store vertex
            _vertexs[vertex->id]=vertex;
        }

        //
        //create half edge and face
        //update vertex, half edge and face
        //
        if (is_face_tag(tag))
        {
            string str=*(++str_it);
            int face_id=atoi(str.c_str());

            //get the face vetex id table
            vector<int> vertex_table;
            for (++str_it; str_it!=str_end; ++str_it)
            {
                str=*str_it;
                vertex_table.push_back(atoi(str.c_str()));
            }
			dface* face = new dface;
			face->id = face_id;

            deal_face_data(face, vertex_table, vertex_half_dic);
        }
    }
    creat_edge_and_pair(vertex_half_dic);

    end_suspend();
}

void dsolid::load(string file_name_)
{
	begin_suspend();
	clear();
	_is_new_file = true;

	_file_name = file_name_;
	_name = _file_name.substr(_file_name.find_last_of("/") + 1);
	string ext = _name.substr(_name.find_last_of(".") + 1);

	if (ext=="m") 
		load_from_m_file(_file_name);
	else if(ext=="off")
		load_from_off_file(_file_name);
	//else if(ext=="3ds")
	//	load_from_3ds_file(_file_name);
	else if (ext=="txt")
		load_from_txt_file(_file_name);
	else
		debug("wrong file format!");
	end_suspend();
	
}

void dsolid::deal_face_data(dface* face, vector<int>& vertex_table, map<int,vector<dhalfedge*> > &vertex_half_dic)
{
	int face_id = face->id;
    //check if the vertex order of the face is consistency
    bool is_right_order=true;
    for (auto vit=vertex_table.begin(); vit!=vertex_table.end(); ++vit)
    {
        //get vertex id
        int vertex_id=*vit;
        int next_vertex_id=*stlpache::next(vertex_table.begin(),vertex_table.end(),vit);
        int previous_vertex_id = *stlpache::previous(vertex_table.begin(),vertex_table.end(),vit);

        vector<dhalfedge*> vertex_halfs = vertex_half_dic[vertex_id];
        for(auto vhit=vertex_halfs.begin(); vhit!=vertex_halfs.end(); ++vhit)
        {
            dhalfedge* vertex_current_half= *vhit;
            if (vertex_current_half->destination()->id==next_vertex_id)
            {
                is_right_order = false;
                break;
            }
        }

        if (!is_right_order)
        {
            break;
        }
    }

    if (!is_right_order)
    {
        reverse(vertex_table.begin(),vertex_table.end());
    }

    int half_edge_id = 0;
    if (_half_edges.size())
    {
        half_edge_id = _half_edges.rbegin()->first+1;
    }
    //create half_edges
    vector<int> half_edge_table;
    for (auto vit=vertex_table.begin(); vit!=vertex_table.end(); ++vit)
    {
        //get  vertex index
        int first_vertex_id = *vit;
        int second_vertex_id = *stlpache::next(vertex_table.begin(),vertex_table.end(),vit);
        half_edge_id++;//(face->id-1)*3+i;

        //create the half edge
        dhalfedge* half_edge = new dhalfedge();
        half_edge->id = half_edge_id;

        //update vertex' half
        if (_vertexs[first_vertex_id]->half==NULL)
        {
            _vertexs[first_vertex_id]->half=half_edge;
        }

        //##########################################################
        //#set aux vertex half_edge_list                                          #dcel_half_edge::pair
        //##########################################################
        vertex_half_dic[first_vertex_id].push_back(half_edge);

        //update half edge's origin and left
        half_edge->origin=_vertexs[first_vertex_id];
        half_edge->left = face;

        //store half edge and edge id table
        _half_edges[half_edge_id]=half_edge;
        half_edge_table.push_back(half_edge_id);
    }

    //update half edge's previous and next
    for(auto htbeg=half_edge_table.begin(); htbeg!=half_edge_table.end(); ++htbeg)
    {
        int current_half_id =*htbeg;
        int next_half_id=*stlpache::next(half_edge_table.begin(),half_edge_table.end(),htbeg);
        int previous_half_id=*stlpache::previous(half_edge_table.begin(),half_edge_table.end(),htbeg);

        _half_edges[current_half_id]->previous = _half_edges[previous_half_id];
        _half_edges[current_half_id]->next=_half_edges[next_half_id];
    }

    //strore face
    _faces[face_id]=face;

    //update face's half
    face->half=_half_edges[half_edge_table[0]];
}
void dsolid::creat_edge_and_pair(map<int,vector<dhalfedge*> > &vertex_half_dic)
{
    //##########################################################
    //update half_edge's pair                                                    #dcel_half_edge::pair
    //##########################################################
    //debug("[half_edge information]:  loading ...");
    for (auto hit=_half_edges.begin(); hit!=_half_edges.end(); ++hit)
    {
        dhalfedge* half_edge= hit->second;
        if (half_edge->pair==NULL)
        {
            dvertex* origin_vertex=half_edge->origin;
            dvertex* dest_vertex=half_edge->destination();

            vector<dhalfedge*> dest_half_edges = vertex_half_dic[dest_vertex->id];

            for (auto dhit = dest_half_edges.begin(); dhit!=dest_half_edges.end(); ++dhit)
            {
                dhalfedge* dest_half_edge= *dhit;
                dvertex* dest_half_edge_next_origin = dest_half_edge->destination();
                if (dest_half_edge_next_origin==origin_vertex)
                {
					if (dest_half_edge->pair==NULL)//若有三个以上的面共边，除了前两个半边外的其他半边设为边界边
					{
						half_edge->pair=dest_half_edge;
						dest_half_edge->pair=half_edge;
					}
                    break;
                }
            }
        }
    }

    //
    //create the edges
    //
    set<int> edge_half_table;
    int edge_id(0);
    for (auto hit=_half_edges.begin(); hit!=_half_edges.end(); ++hit)
    {
        dhalfedge* half=hit->second;
        dhalfedge* pair=half->pair;

        if (edge_half_table.find(half->id)!=edge_half_table.end())
            continue;

        dedge* edge = new dedge();
        edge->id = ++edge_id;
        edge->half=half;
        _edges[edge->id]=edge;

        half->edge=edge;
        edge_half_table.insert(half->id);

        if (pair!=NULL)   //说明不是边界的半边
        {
            pair->edge=edge;
            edge_half_table.insert(pair->id);
        }

    }
}
void dsolid::save_to_m_file(string fileName)
{
    std::ofstream fout(fileName);

    //export the vertexs information
    std::transform(
        _vertexs.begin(),_vertexs.end(),
        ostream_iterator<dvertex*>(fout,"\n"),
        make_select2nd(_vertexs));

    //write a blank line
    fout<<std::endl;

    //export the face information
    std::transform(
        _faces.begin(),_faces.end(),
        ostream_iterator<dface*>(fout,"\n"),
        make_select2nd(_faces));
}

void dsolid::modified_butterfly_subdivision()
{
	begin_suspend();
	turn_to_trimesh();
	debug("begin Modified Butterfly subdivision...");
	map<dvertex*, int> valence_of_edges;
	for (auto it = _vertexs.begin(); it!=_vertexs.end(); ++it)
	{
		if (it->second->half==NULL)   //该点没有在面数据中用到
		{
			continue;
		}
		dhalfedge* start_half = it->second->half;
		dhalfedge* current_half = start_half;
		bool is_on_boundary = false;
		int valence=0;
		do
		{
			valence++;
			if (current_half->pair == NULL)
			{
				is_on_boundary = true;
				break;
			}
			current_half = current_half->pair->next;
		}
		while (current_half != start_half);
		if (is_on_boundary)   //如果在边界上，从开始半边方向遍历三角形
		{
			start_half = start_half->previous;
			current_half = start_half;
			do
			{
				valence++;
				if (current_half->pair == NULL)
				{
					break;
				}
				current_half = current_half->pair->previous;
			}
			while (current_half != start_half);
		}
		valence_of_edges[it->second] = valence;
	}

	//产生新边点，把边断成两条
	int new_vertex_id = _vertexs.rbegin()->first;
	int new_half_id = _half_edges.rbegin()->first;
	int new_edge_id = _edges.rbegin()->first;
	int new_face_id = _faces.rbegin()->first;
	map<int, dedge*> new_edges;//遍历边的过程中产生的新边要先放在这里
	for (auto it = _edges.begin(); it!=_edges.end(); ++it)
	{
		dvertex* new_vertex = new dvertex;
		new_vertex->id = ++new_vertex_id;
		_vertexs[new_vertex->id]=new_vertex;
		dhalfedge* start_half = it->second->half;
		dvertex* first_vertex = start_half->origin;
		dvertex* second_vertex = start_half->destination();
		int valence_of_first = valence_of_edges[first_vertex];
		int valence_of_second = valence_of_edges[second_vertex];

		if (valence_of_first==6&&valence_of_second==6)
		{
			dvertex* eighth_up = start_half->previous->origin;
			dvertex* eighth_down = start_half->pair->previous->origin;
			dvertex* sixteenth_left_up = start_half->next->pair->previous->origin;
			dvertex* sixteenth_left_down = start_half->pair->previous->pair->previous->origin;
			dvertex* sixteenth_right_up = start_half->previous->pair->previous->origin;
			dvertex* sixteenth_right_down = start_half->pair->next->pair->previous->origin;

			new_vertex->coords = ((first_vertex->coords+second_vertex->coords)*8.0
				+(eighth_down->coords+eighth_up->coords)*2.0+
				(sixteenth_left_down->coords+sixteenth_left_up->coords+sixteenth_right_down->coords+
				sixteenth_right_up->coords)*(-1))/16;
			new_vertex->normal = ((first_vertex->normal+second_vertex->normal)*8.0
				+(eighth_down->normal+eighth_up->normal)*2-
				sixteenth_left_down->normal-sixteenth_left_up->normal-sixteenth_right_down->normal-
				sixteenth_right_up->normal)/16;
		}
		else if (start_half->pair==NULL)//Boundary edges
		{
			//debug("Boundary edges");
			//debug(it->second->id);
			//debug(first_vertex->id);
			//debug(second_vertex->id);
			dhalfedge* iteration_half = start_half->next;
			while(iteration_half->pair!=NULL)
			{
				iteration_half=iteration_half->pair->next;
			}
			dvertex* border_on_second_vertex = iteration_half->destination();

			iteration_half = start_half->previous;
			while(iteration_half->pair!=NULL)
			{
				iteration_half=iteration_half->pair->previous;
			}
			dvertex* border_on_first_vertex = iteration_half->origin;

			new_vertex->coords = (first_vertex->coords*9+second_vertex->coords*9
				+(border_on_first_vertex->coords+border_on_second_vertex->coords)*(-1))/16;
			new_vertex->normal = (first_vertex->normal*9+second_vertex->normal*9
				-border_on_first_vertex->normal-border_on_second_vertex->normal)/16;
		}

		else if (valence_of_first!=6&&valence_of_second==6)
		{
			butterfly_singularity(start_half, new_vertex->coords, new_vertex->normal);
		}
		else if (valence_of_first==6&&valence_of_second!=6)
		{
			butterfly_singularity(start_half->pair, new_vertex->coords, new_vertex->normal);
		}
		else
		{
			//debug("bu zhengzhe");
			//debug(it->second->id);
			//debug(first_vertex->id);
			//debug(second_vertex->id);
			point3f coords1, coords2;
			vector3f normal1, normal2;
			butterfly_singularity(start_half, coords1, normal1);
			butterfly_singularity(start_half->pair, coords2, normal2);
			new_vertex->coords = (coords1+coords2)/2;
			new_vertex->normal = (normal1+normal2)/2;
		}
		//
		//create and insert new left half edge
		//update new_vertex->half
		//
		dhalfedge* left_half = start_half;
		dhalfedge* right_half=left_half->pair;
		dhalfedge* new_left_half = new dhalfedge();
		dedge* new_edge = new dedge();
		new_left_half->id=++new_half_id;
		new_left_half->previous=left_half;              //1.previous
		new_left_half->next=left_half->next;            //2.next
		left_half->next->previous = new_left_half;
		left_half->next=new_left_half;
		new_left_half->origin=new_vertex;               //3.origin
		new_vertex->half=new_left_half;
		new_left_half->left=left_half->left;            //4.left
		_half_edges[new_left_half->id]=new_left_half;
		new_left_half->edge=new_edge;                  //6.edge

		//
		//create new edge
		//
		new_edge->id=++new_edge_id;
		new_edge->half=new_left_half;
		new_edges[new_edge->id]=new_edge;

		if (right_half!=NULL)
		{
			//
			//create and insert new right half edge
			//
			dhalfedge* new_right_half = new dhalfedge();
			new_right_half->id=++new_half_id;
			new_right_half->previous=right_half;
			new_right_half->next=right_half->next;
			right_half->next->previous=new_right_half;
			right_half->next=new_right_half;
			new_right_half->origin=new_vertex;
			new_right_half->left=right_half->left;
			_half_edges[new_right_half->id]=new_right_half;

			//
			//update half edge's pair
			//
			new_left_half->pair=right_half;              //5.pair
			new_right_half->pair=left_half;
			left_half->pair=new_right_half;
			right_half->pair=new_left_half;
			//
			//update new half edge's edge
			//
			new_right_half->edge=left_half->edge;

			right_half->edge = new_edge;	//范飞龙这里忘记了。叶张翔于2011年3月3日0:05补充。
		}
	}
	//
	//copy the new edeges created above to the dcel::solie::edeges
	//
	for (auto it=new_edges.begin();it!=new_edges.end();++it)
	{
		_edges[it->first]=it->second;
	}
	//
	//create new face-edge
	//create new face
	//
	map<int,dface*> new_faces;
	for (auto it=_faces.begin();it!=_faces.end();++it)
	{
		dface* face=it->second;
		dhalfedge* half = face->half;
		dhalfedge* start_new_half=half->next;
		dhalfedge* current_half=start_new_half;

		//
		//create middle face
		//
		dface* middle_face = face;
		middle_face->half=NULL;

		//
		//prepare new_left_half
		//
		dhalfedge* last_new_left_half=NULL;

		//TODO:dead loop
		int new_vertex_count(0);
		do 
		{
			//
			//get the edge-vertex
			//
			dhalfedge* diagonal_half=current_half->next->next;
			dhalfedge* current_next_half=current_half->next;
			dvertex* new_orign_vertex=current_half->origin;
			dvertex* new_dest_vertex=diagonal_half->origin;

			//
			//create new face 
			//
			dface* new_face=new dface();
			new_face->id=++new_face_id;
			new_face->half=current_half;
			new_faces[new_face->id]=new_face;

			//
			//create new edge
			//
			dedge* new_edge=new dedge();
			new_edge->id=++new_edge_id;
			_edges[new_edge->id]=new_edge;


			//
			//create new left half
			//
			dhalfedge* new_left_half=new dhalfedge();
			new_left_half->id=++new_half_id;                 //id
			new_left_half->origin=new_orign_vertex;                 //origin
			new_left_half->edge=new_edge;                         //edge
			new_left_half->left=middle_face;                        //left

			new_edge->half=new_left_half;

			if (middle_face->half==NULL)
			{
				middle_face->half=new_left_half;
			}

			if (last_new_left_half!=NULL)
			{
				last_new_left_half->next=new_left_half;             //next
				new_left_half->previous=last_new_left_half;         //previous
			}

			last_new_left_half=new_left_half;
			_half_edges[new_left_half->id]=new_left_half;  

			//
			//create new right half
			//
			dhalfedge* new_right_half=new dhalfedge();
			new_right_half->id=++new_half_id;                //id
			new_right_half->origin=new_dest_vertex;                 //origin
			new_right_half->next=current_half;                      //next
			new_right_half->left=new_face;                          //left
			new_right_half->previous=current_next_half;             //previous
			new_right_half->edge=new_edge;                        //edge
			_half_edges[new_right_half->id]=new_right_half;


			//
			//update new half's pair
			//
			new_left_half->pair=new_right_half;
			new_right_half->pair=new_left_half;

			//
			//update current_half
			//
			current_half->left=new_face;
			current_half->previous=new_right_half;

			//
			//update current_next_half
			//
			current_next_half->next=new_right_half;
			current_next_half->left=new_face;

			//
			//update status
			//
			new_vertex_count++;
			current_half=diagonal_half;

		} while (current_half!=start_new_half);

		//
		//update the last_new_left_half's next
		//
		last_new_left_half->next=middle_face->half;
		middle_face->half->previous=last_new_left_half;
	}
	//
	//copy new_faces to faces
	//
	for (auto it=new_faces.begin();it!=new_faces.end();++it)
	{
		_faces[it->first]=it->second;
	}
	debug("end Modified Butterfly subdivision!");

	end_suspend();
}
void dsolid::butterfly_singularity(dhalfedge* start_half, point3f &coords, vector3f& normal)
{
	dhalfedge* current_half = start_half;
	bool is_on_boundary = false;
	int valence=0;
	vector<dvertex*> adjoin_vertexs;
	vector<dvertex*> reverse_adjoin_vertexs;
	do
	{
		valence++;
		adjoin_vertexs.push_back(current_half->destination());
		if (current_half->pair == NULL)
		{
			is_on_boundary = true;
			break;
		}
		current_half = current_half->pair->next;
	}
	while (current_half != start_half);
	if (is_on_boundary)   //如果在边界上，从开始半边方向遍历三角形
	{
		start_half = start_half->previous;
		current_half = start_half;
		do
		{
			valence++;
			reverse_adjoin_vertexs.push_back(current_half->origin);
			if (current_half->pair == NULL)
			{
				break;
			}
			current_half = current_half->pair->previous;
		}
		while (current_half != start_half);
		for (auto it = reverse_adjoin_vertexs.rbegin(); it!=reverse_adjoin_vertexs.rend(); ++it)
		{
			adjoin_vertexs.push_back(*it);
		}
	}
	dvertex* origin_vertex = start_half->origin;
	switch (valence)
	{
	case 2:
		debug("imposible,valence_of_first=2");
	case 3:
		coords = (adjoin_vertexs[0]->coords*5+adjoin_vertexs[1]->coords*(-1)
			+adjoin_vertexs[2]->coords*(-1) + origin_vertex->coords*9)/12;
		normal = (adjoin_vertexs[0]->normal*5-adjoin_vertexs[1]->normal
			-adjoin_vertexs[2]->normal + origin_vertex->normal*9 )/12;
		break;
	case 4:
		coords = (adjoin_vertexs[0]->coords*3+adjoin_vertexs[2]->coords*(-1) + origin_vertex->coords*6)/8;
		normal = (adjoin_vertexs[0]->normal*3-adjoin_vertexs[2]->normal + origin_vertex->normal*6)/8;
		break;
	default:
		int index = 0;
		double coeffcient = 0;
		for (auto vit = adjoin_vertexs.begin(); vit!=adjoin_vertexs.end(); ++vit, index++)
		{
			double s_i = (1/4+cos(2*PI*index/valence)+cos(4*PI*index/valence)/2)/valence;
			coeffcient += s_i;
			coords = coords+(*vit)->coords*s_i;
			normal = normal+(*vit)->normal*s_i;
		}
		coords = coords+ origin_vertex->coords*(1-coeffcient);
		normal = normal+ origin_vertex->normal*(1-coeffcient);
	}
}
void dsolid::caculate_vertex_normal()
{
    for (auto it = _vertexs.begin(); it != _vertexs.end(); it++)
    {
        if (it->second->half==NULL)   //该点没有在面数据中用到
        {
            continue;
        }
        dhalfedge* start_half = it->second->half;
        dhalfedge* current_half = start_half;
        vector<pair<float, vector3f>> normals;
        vector3f first_vector = current_half->previous->origin->coords-it->second->coords;
        bool is_on_boundary = false;
        float area_sum=0;
        do
        {
            //dcel_half_edge* next_half = current_half->pair->next;
            vector3f second_vector = current_half->destination()->coords-it->second->coords;
            //求法向外积
            vector3f normal_triangle = second_vector ^ first_vector;
            float area_triangle = normal_triangle.norm();
			if (area_triangle!=0)
			{
				normal_triangle = normal_triangle/area_triangle;
				area_triangle = area_triangle/2;
				area_sum+=area_triangle;
				normals.push_back(pair<double, vector3f> (area_triangle, normal_triangle));
			}
            if (current_half->pair == NULL)
            {
                is_on_boundary = true;
                break;
            }
            current_half = current_half->pair->next;
            first_vector = second_vector;
        }
        while (current_half != start_half);
        if (is_on_boundary)   //如果在边界上，从开始半边方向遍历三角形
        {
            if (start_half->previous->pair!=NULL)   //说明该点只有这么一个三角形
            {
                start_half = start_half->previous->pair;
                current_half = start_half;
                first_vector = current_half->previous->origin->coords-it->second->coords;
                do
                {
                    //dcel_half_edge* next_half = current_half->pair->next;
                    vector3f second_vector = current_half->destination()->coords-it->second->coords;
                    //求法向外积
                    vector3f normal_triangle = second_vector ^ first_vector;
                    float area_triangle = normal_triangle.norm();
					if (area_triangle!=0)
					{
						normal_triangle = normal_triangle/area_triangle;
						area_triangle = area_triangle/2;
						area_sum+=area_triangle;
						normals.push_back(pair<float, vector3f> (area_triangle, normal_triangle));
					}

                    if (current_half->previous->pair == NULL)
                    {
                        break;
                    }
                    current_half = current_half->previous->pair;
                    first_vector = second_vector;
                }
                while (current_half != start_half);
            }
        }
		if (area_sum!=0)//如果有顶点就两条边，且两条边在一条直线上就会出现area_sum==0，这种情况顶点法向没法算
		{
			vector3f normal_current_point;
			for (auto iter = normals.begin(); iter!=normals.end(); iter++)
			{
				normal_current_point += iter->second*iter->first;
			}
			normal_current_point /= area_sum;
			it->second->normal = normal_current_point;
		}
    }
}
void dsolid::caculate_center_parameter()
{
    reset_center_parameter();
    float min_x(0.0),max_x(0.0);
    float min_y(0.0),max_y(0.0);
    for(auto vit=_vertexs.begin(); vit!=_vertexs.end(); ++vit)
    {
        //update the top vertex
        float vertex_coord_x=vit->second->coords.x();
        float vertex_coord_y=vit->second->coords.y();
        float vertex_coord_z=vit->second->coords.z();

        //x
        _center.set_x(vertex_coord_x+_center.x());
        if(min_x>vertex_coord_x) min_x=vertex_coord_x;
        if(max_x<vertex_coord_x) max_x=vertex_coord_x;

        //y
        _center.set_y(vertex_coord_y+_center.y());
        if(min_y>vertex_coord_y) min_y=vertex_coord_y;
        if(max_y<vertex_coord_y) max_y=vertex_coord_y;

        //z
        if(vertex_coord_z>_center.z()) _center.set_z(vertex_coord_z);
    }

    float weight=1.0f/_vertexs.size();
    _center=_center.handman(vector3f(weight,weight,1));
    _width=abs(max_x-min_x);
    _height=abs(max_y-min_y);
}
void dsolid::turn_to_trimesh()
{
	begin_suspend();

	int new_half_id = _half_edges.rbegin()->first,
		new_face_id = _faces.rbegin()->first,
		new_edge_id = _edges.rbegin()->first ;
	for (auto it=_faces.begin(); it!=_faces.end(); ++it)
	{
		dface* current_face = it->second;
		dhalfedge* start_half = current_face->half;
		dhalfedge* current_half = start_half->next;
		while(current_half->next->next!=start_half)
		{
			dhalfedge* new_half1 = new dhalfedge;
			dhalfedge* new_half2 = new dhalfedge;
			dedge* new_edge = new dedge;
			dface* new_face = new dface;

			new_half1->id = ++new_half_id;
			_half_edges[new_half1->id] = new_half1;
			new_half1->edge = new_edge;
			new_half1->left = new_face;
			new_half1->next = current_half;
			new_half1->previous = current_half->next;
			new_half1->pair = new_half2;
			new_half1->origin = current_half->next->destination();

			new_half2->id = ++new_half_id;
			_half_edges[new_half2->id] = new_half2;
			new_half2->edge = new_edge;
			new_half2->left = current_face;
			new_half2->next = current_half->next->next;
			new_half2->previous = start_half;
			new_half2->pair = new_half1;
			new_half2->origin = current_half->origin;

			new_edge->id = ++new_edge_id;
			_edges[new_edge->id] = new_edge;
			new_edge->half = new_half1;

			new_face->id = ++new_face_id;
			_faces[new_face->id] = new_face;
			new_face->half = new_half1;

			start_half->next = new_half2;
			current_half->next->next->previous = new_half2;

			current_half->left = new_face;
			current_half->previous = new_half1;
			current_half->next->left = new_face;
			current_half->next->next = new_half1;

			current_half = new_half2;

		}
	}
	end_suspend();
}
void dsolid::test_solid()
{
	//输出点，边，半边，面的个数
	debug("test begin");
	stringstream ss;

	string vertex_num;
	ss<<_vertexs.size();
	ss>>vertex_num;
	ss.clear();

	string edge_num;
	ss<<_edges.size();
	ss>>edge_num;
	ss.clear();

	string half_num;
	ss<<_half_edges.size();
	ss>>half_num;
	ss.clear();

	string face_num;
	ss<<_faces.size();
	ss>>face_num;
	ss.clear();
	debug("There are " + vertex_num+"vertexs, "
		+edge_num+"edges, " + half_num+
		"half edges, " + face_num+"faces.");

	//输出边的检测信息
	int num_edge_half_null=0;
	int num_edge_half_edge=0;

	for (auto it = _edges.begin(); it!=_edges.end(); it++)
	{
		if (it->second->half==NULL)
		{
			num_edge_half_null++;
		}
		else if (it->second->half->edge!=it->second)
		{
			num_edge_half_edge++;
		}
	}
	string num_err;
	if (num_edge_half_null)
	{
		ss << num_edge_half_null;
		ss >> num_err;
		ss.clear();
		debug(num_err+"edges' half is null");
	}
	if (num_edge_half_edge)
	{
		ss << num_edge_half_edge;
		ss >> num_err;
		ss.clear();
		debug(num_err+"edges' half's edge is not itself");
	}

	//测试面
	int num_face_half_null=0;
	int num_face_half_left=0;
	for (auto it = _faces.begin(); it!=_faces.end(); it++)
	{
		if (it->second->half==NULL)
		{
			num_face_half_null++;
		}
		else
		{
			dhalfedge* start_half = it->second->half;
			dhalfedge* current_half = start_half;

			do 
			{
				if (current_half->left!=it->second)
				{
					num_face_half_left++;
				}
			} while (current_half!=start_half);
		}
	}
	if (num_face_half_left)
	{
		ss << num_face_half_left;
		ss >> num_err;
		ss.clear();
		debug(num_err+"faces has half's left not itself");
	}
	if (num_face_half_null)
	{
		ss << num_face_half_null;
		ss >> num_err;
		ss.clear();
		debug(num_err+"faces' half is null");
	}

	//测试点
	int num_vertex_half_null=0;
	int num_vertex_half_origin=0;
	for (auto it = _vertexs.begin(); it!= _vertexs.end(); it++)
	{
		if (it->second->half==NULL)
		{
			num_vertex_half_null++;
		}
		else if (it->second->half->origin!=it->second)
		{
			num_vertex_half_origin++;
		}
	}
	if (num_vertex_half_null)
	{
		ss << num_vertex_half_null;
		ss >> num_err;
		ss.clear();
		debug(num_err+"vertexs' half is null");
	}
	if (num_vertex_half_origin)
	{
		ss << num_vertex_half_origin;
		ss >> num_err;
		ss.clear();
		debug(num_err+"vertexs' half's origin is not itself");
	}

	//测试半边
	for (auto it = _half_edges.begin(); it!= _half_edges.end(); it++)
	{
		bool is_half_right=true;
		string half_id;
		ss << it->first;
		ss >> half_id;
		ss.clear();
		if (it->second->previous ==NULL)
		{
			is_half_right=false;
			half_id+="'s previous is null. ";
		}
		else if (it->second->previous->next!=it->second)
		{
			is_half_right=false;
			half_id+="'s previous's next is not itself. ";
		}
		if (it->second->next ==NULL)
		{
			is_half_right=false;
			half_id+="'s next is null. ";
		}
		else if (it->second->next->previous!=it->second)
		{
			is_half_right=false;
			half_id+="'s nest's previous is not itself. ";
		}
		if (it->second->origin ==NULL)
		{
			is_half_right=false;
			half_id+="'s origin is null. ";
		}
		if (it->second->edge ==NULL)
		{
			is_half_right=false;
			half_id+="'s edge is null. ";
		}
		if (it->second->left ==NULL)
		{
			is_half_right=false;
			half_id+="'s left is null. ";
		}
		if (it->second->pair ==NULL)
		{
			//debug("pair is null");
			//debug(it->first);
		}
		else
		{
			if (it->second->pair->pair !=it->second)
			{
				is_half_right=false;
				half_id+="'s pair is wrong. ";
			}
			if (it->second->pair->edge !=it->second->edge)
			{
				is_half_right=false;
				half_id += "'s edge pair is wrong. ";
			}
		}
		if (!is_half_right)
		{
			debug(half_id);
		}
	}
	debug("test over");
}
void dsolid::doo_sabin_subdivision()
{
    begin_suspend();
    debug("begin Doo Sabin subdivision...");
    vector<dvertex*> old_vertex_table;

    //储存新点
    typedef map<int, dvertex*> vertexs_on_one_face;
    map<int, vertexs_on_one_face> new_vertexs;
    //储存新边
    map<int, dedge*> new_edges;
    //储存新半边
    map<int, dhalfedge*> new_half_edges;

    int new_vertex_id = 0, new_edge_id = 0, new_half_id = 0;

    //1. 循环每个面生成n个点n条边n条半边
    for (auto it=_faces.begin(); it!=_faces.end(); ++it)
    {
        dhalfedge* start_half = it->second->half;
        dhalfedge* current_half = start_half;
        vertexs_on_one_face new_vertex_table;	//放置该面的新点

        vector<dvertex*> vertexs_on_this_face;
        do
        {
            vertexs_on_this_face.push_back(current_half->origin);
            current_half = current_half->next;
        }
        while (current_half!=start_half);

        int n = vertexs_on_this_face.size();
        dhalfedge* last_new_half = NULL;
        dhalfedge* first_new_half = NULL;
        for (int i = 0; i < n; i++)
        {
            dvertex* new_vertex = new dvertex();
            dhalfedge *new_half = new dhalfedge();
            dedge *new_edge = new dedge();

            point3f coords;
            vector3f normal;
            for (int j = 0; j < n; j++)
            {
                float omega;
                if (i == j)
                {
                    omega = (n+5.0f)/(4.0f*n);
                }
                else
                {
                    omega = (3.0f+2.0f*(float)(cos(2.0f*(i-j)*PI/n)))/(4.0f*n);
                }
                coords += vertexs_on_this_face[j]->coords*omega;
                normal += vertexs_on_this_face[j]->normal*omega;
            }
            new_vertex->coords = coords;			//1.coords
            new_vertex->normal = normal;			//2.normal
            new_vertex->id	   = ++new_vertex_id;	//3.id
            new_vertex->half   = new_half;			//4.half

            //保存点！
            new_vertex_table[vertexs_on_this_face[i]->id] = new_vertex;	//int是对应的旧点的id

            new_edge->id      = ++new_edge_id;	//1.id
            new_edge->half    = new_half;			//2.half

            //保存边
            new_edges[new_edge->id] = new_edge;

            new_half->edge    = new_edge;			//1.edge
            new_half->id       = ++new_half_id;		//2.id
            new_half->left     = it->second;		//3.left
            new_half->origin   = new_vertex;		//4.orgin
            if (last_new_half!=NULL)
            {
                new_half->previous = last_new_half;	//5.previous
                last_new_half->next = new_half;		//6.next
            }
            else
            {
                first_new_half = new_half;
            }

            //保存半边
            new_half_edges[new_half->id] = new_half;

            //更新last_new_half
            last_new_half = new_half;
        }
        //处理第一条半边和最后一条半边的 next & previous
        last_new_half->next = first_new_half;		//5.previous
        first_new_half->previous = last_new_half;	//6.next

        //更新该面信息id不变只改half即可。这里忘了更新害我找错误找了半体，这个错误太隐蔽了！
        it->second->half = first_new_half;

        //保存该面的点到new_vertexs
        new_vertexs[it->second->id] = new_vertex_table;	//索引是该面的id
    }

    //2. 循环每条边，生成2条边，四条半边，1个面
    for (auto it = _edges.begin(); it != _edges.end(); it++)
    {
        dedge* edge= it->second;
        dhalfedge* half = edge->half;
        if (half->pair != NULL)   //当前边不是边界上的边才有这个操作
        {
            dvertex* vertex_1 = new_vertexs[half->left->id][half->pair->origin->id];
            dvertex* vertex_2 = new_vertexs[half->left->id][half->origin->id];
            dvertex* vertex_3 = new_vertexs[half->pair->left->id][half->origin->id];
            dvertex* vertex_4 = new_vertexs[half->pair->left->id][half->pair->origin->id];

            dface* new_face = new dface();

            dedge* edge_1 = new dedge();
            dedge* edge_2 = new dedge();

            dhalfedge* half_1 = new dhalfedge();
            dhalfedge* half_2 = new dhalfedge();
            dhalfedge* half_3 = new dhalfedge();
            dhalfedge* half_4 = new dhalfedge();

            //给半边赋值
            half_1->edge    = vertex_2->half->edge;	//1.edge
            half_1->id       = ++new_half_id;	    //2.id
            half_1->left     = new_face;				//3.left
            half_1->origin   = vertex_1;				//4.orgin
            half_1->previous = half_4;				//5.previous
            half_1->next     = half_2;				//6.next
            half_1->pair	 = vertex_2->half;			//7.pair
            vertex_2->half->pair = half_1;			//补上步骤1中的pair

            half_2->edge    = edge_1;				//1.edge
            half_2->id       = ++new_half_id;		//2.id
            half_2->left     = new_face;				//3.left
            half_2->origin   = vertex_2;				//4.orgin
            half_2->previous = half_1;				//5.previous
            half_2->next     = half_3;				//6.next

            half_3->edge    = vertex_4->half->edge;	//1.edge
            half_3->id       = ++new_half_id;		//2.id
            half_3->left     = new_face;				//3.left
            half_3->origin   = vertex_3;				//4.orgin
            half_3->previous = half_2;				//5.previous
            half_3->next     = half_4;				//6.next
            half_3->pair	 = vertex_4->half;			//7.pair
            vertex_4->half->pair = half_3;			//补上步骤1中的pair

            half_4->edge    = edge_2;				//1.edge
            half_4->id       = ++new_half_id;		//2.id
            half_4->left     = new_face;				//3.left
            half_4->origin   = vertex_4;				//4.orgin
            half_4->previous = half_3;				//5.previous
            half_4->next     = half_1;				//6.next

            //保存半边
            new_half_edges[half_1->id] = half_1;
            new_half_edges[half_2->id] = half_2;
            new_half_edges[half_3->id] = half_3;
            new_half_edges[half_4->id] = half_4;

            //给边赋值
            edge_1->id   = ++new_edge_id;	        //1.id
            edge_2->id   = ++new_edge_id;	        //1.id
            edge_1->half = half_2;			        //2.half
            edge_2->half = half_4;			        //2.half

            //保存边
            new_edges[edge_1->id] = edge_1;
            new_edges[edge_2->id] = edge_2;

            //给面赋值
            new_face->id = _faces.size()+1;
            new_face->half = half_1;

            //保存面
            _faces[new_face->id] = new_face;
        }

    }

    //3. 循环每个点，生成n条半边，1个面
    for (auto it = _vertexs.begin(); it!=_vertexs.end(); it++)
    {
        dvertex* vertex = it->second;
        if (vertex->half!=NULL)
        {
            vector<dvertex*> vertex_on_vertex_face;
            dhalfedge* start_half = vertex->half;
            dhalfedge* current_half = start_half;
            dface* new_face = new dface();
            bool is_on_boundary = false;
            do
            {
                if (current_half->pair != NULL)   //当前边不是边界边
                {
                    vertex_on_vertex_face.push_back(new_vertexs[current_half->left->id][vertex->id]);
                    current_half = current_half->pair->next;
                }
                else
                {
                    is_on_boundary = true;//点是在边界上
                    break;
                }
            }
            while (current_half!=start_half);
            if (!is_on_boundary)   //点不在边界上才生成新面点
            {
                //生成n个半边，赋值半边
                dhalfedge* last_new_half = NULL;
                dhalfedge* first_new_half = NULL;
                for (auto it = vertex_on_vertex_face.rbegin(); it != vertex_on_vertex_face.rend(); it++)
                {
                    dvertex* destination;
                    if (it+1 != vertex_on_vertex_face.rend())
                    {
                        destination = *(it+1);
                    }
                    else
                    {
                        destination = *(vertex_on_vertex_face.rbegin());
                    }
                    dhalfedge* new_half = new dhalfedge();
                    new_half->edge    = destination->half->pair->next->edge;	//1.edge
                    new_half->id       = ++new_half_id;				        //2.id
                    new_half->left     = new_face;					        //3.left
                    new_half->origin   = *it;						        //4.origin
                    if (last_new_half!=NULL)
                    {
                        new_half->previous  = last_new_half;		            //5.previous
                        last_new_half->next = new_half;				        //6.next
                    }
                    else
                    {
                        first_new_half = new_half;
                    }
                    new_half->pair	   = destination->half->pair->next;		//7.pair
                    destination->half->pair->next->pair  = new_half;			//补上步骤2中的pair

                    //保存半边
                    new_half_edges[new_half->id] = new_half;
                    last_new_half = new_half;
                }
                last_new_half->next = first_new_half;
                first_new_half->previous = last_new_half;

                //赋值面
                new_face->id = _faces.size()+1;
                new_face->half = first_new_half;

                //保存面
                _faces[new_face->id] = new_face;
            }
        }
    }

    //释放原来的点，边，半边的空间
    delete_values(&_vertexs);
    delete_values(&_half_edges);
    delete_values(&_edges);

    //对vertexs,half_edges,dedges重新赋值
    for (auto it=new_half_edges.begin(); it!=new_half_edges.end(); ++it)
    {
        _half_edges[it->first]=it->second;
    }
    for (auto it=new_edges.begin(); it!=new_edges.end(); ++it)
    {
        _edges[it->first]=it->second;
    }
    for (auto it = new_vertexs.begin(); it!=new_vertexs.end(); it++)
    {
        vertexs_on_one_face vertexs_table = it->second;
        for (auto iter = vertexs_table.begin(); iter != vertexs_table.end(); iter++)
        {
            _vertexs[iter->second->id] = iter->second;
        }
    }
    debug("end Doo Sabin subdivision!");

    end_suspend();
}

//重构后的cc细分代码
void dsolid::catmull_clark_subdivision()
{
    begin_suspend();
    debug("begin Catmull Clark subdivision...");
    int new_vertex_id = 0, new_edge_id = 0, new_half_id = 0, new_face_id = 0;
    //产生新面点，先放在new_face_vertexs中
	vector<dvertex*> new_vertexs;

    map <dface*, dvertex*> new_face_vertexs;
    for (auto it=_faces.begin(); it!=_faces.end(); ++it)
    {
        dhalfedge* start_half = it->second->half;
        dhalfedge* current_half = start_half;

        int vertex_count(0);
        point3f coords;
        vector3f normal;
        do
        {
            coords+=current_half->origin->coords;
            normal+=current_half->origin->normal;

            vertex_count++;
            current_half = current_half->next;
        }
        while (current_half!=start_half);
        dvertex* new_face_vertex = new dvertex();
        new_face_vertex->coords = coords/(float)(vertex_count);
        new_face_vertex->normal = normal/(float)(vertex_count);
        new_face_vertexs[it->second] = new_face_vertex;
		new_vertexs.push_back(new_face_vertex);
    }

    //产生新边点，先放在new_edge_vertexs中
    map <dedge*, dvertex*> new_edge_vertexs;
    for (auto it=_edges.begin(); it!=_edges.end(); ++it)
    {
        dhalfedge* current_half = it->second->half;
        if (current_half->pair!=NULL)//如果当前边不是边界才产生新边点
        {
            point3f coords;
            vector3f normal;
            coords = current_half->origin->coords + current_half->destination()->coords
                     + new_face_vertexs[current_half->left]->coords
                     + new_face_vertexs[current_half->pair->left]->coords;
            normal = current_half->origin->normal + current_half->destination()->normal
                     + new_face_vertexs[current_half->left]->normal
                     + new_face_vertexs[current_half->pair->left]->normal;

            dvertex* new_edge_vertex = new dvertex();
            new_edge_vertex->coords = coords/4;
            new_edge_vertex->normal = normal/4;
            new_edge_vertexs[it->second] = new_edge_vertex;
			new_vertexs.push_back(new_edge_vertex);
        }
    }

    //产生新点点
    map <dvertex*, dvertex*> new_vertex_vertexs;
    for (auto it=_vertexs.begin(); it!=_vertexs.end(); ++it)
    {
        dvertex* vertex = it->second;
        dhalfedge* half=vertex->half;
        if (half!=NULL)
        {
            dhalfedge* current_half=half;

            //算共边顶点的平均和新面点的平均
            point3f average_face_vetex_coords;
            point3f	average_coterminal_vetex_coords;
            vector3f average_face_vetex_normal;
            vector3f average_coterminal_vetex_normal;

            bool is_on_boundary = false;
            int adjecent_vertex_count(0);
            do
            {
                if (current_half->pair!=NULL)   //如果点不在边界上
                {
                    dvertex* current_vertex = current_half->destination();
                    average_coterminal_vetex_coords+=current_vertex->coords;
                    average_coterminal_vetex_normal+=current_vertex->normal;

                    dvertex* current_face_vertex = new_face_vertexs[current_half->left];
                    average_face_vetex_coords += current_face_vertex->coords;
                    average_face_vetex_normal += current_face_vertex->normal;

                    adjecent_vertex_count++;
                    current_half=current_half->pair->next;
                }
                else
                {
                    is_on_boundary = true;
                    break;
                }
            }
            while (current_half!=half);

            if (!is_on_boundary)   //如果点不在边界上产生新点点
            {
                average_coterminal_vetex_coords = average_coterminal_vetex_coords/(float)(adjecent_vertex_count);
                average_coterminal_vetex_normal = average_coterminal_vetex_normal/(float)(adjecent_vertex_count);

                average_face_vetex_coords = average_face_vetex_coords/(float)(adjecent_vertex_count);
                average_face_vetex_normal = average_face_vetex_normal/(float)(adjecent_vertex_count);

                dvertex* new_vertex_vertex = new dvertex;

                new_vertex_vertex->coords = (vertex->coords*(float)(adjecent_vertex_count-2)
                                             +average_coterminal_vetex_coords
                                             +average_face_vetex_coords)
                                            /(float)(adjecent_vertex_count);

                new_vertex_vertex->normal = (vertex->normal*(float)(adjecent_vertex_count-2)
                                             +average_coterminal_vetex_normal
                                             +average_face_vetex_normal)
                                            /(float)(adjecent_vertex_count);

                new_vertex_vertexs[vertex] = new_vertex_vertex;
				new_vertexs.push_back(new_vertex_vertex);
            }
        }
    }

    map<int, dhalfedge*> new_half_edges;
    map<dhalfedge*, dhalfedge*> face2edge_half_edges;
    map<dhalfedge*, dhalfedge*> edge2face_half_edges;
    map<int, dedge*> new_edges;
    map<int, dface*> new_faces;

    //遍历新点点，与相邻边对应的新边点产生四个半边一个边一个面，这一步可以在上面的产生新点点里面做，效率更高，先在这里做，代码结构更清晰
    for (auto it = new_vertex_vertexs.begin(); it != new_vertex_vertexs.end(); it++)
    {
        dvertex* old_vertex = it->first;
        dvertex* new_vertex_vertex = it->second;

        dhalfedge* start_half = old_vertex->half;
        dhalfedge* current_half = start_half;
        dedge* last_new_edge = NULL;
        dhalfedge* last_vertex2edge = NULL;
        dhalfedge* first_edge2vertex = NULL;
        do
        {
            dvertex* corres_edge_vertex = new_edge_vertexs[current_half->edge];
            dvertex* corres_edge_previous_vertex = new_edge_vertexs[current_half->previous->edge];
            dvertex* corres_face_vertex = new_face_vertexs[current_half->left];

            dedge* new_edge = new dedge;
            dface* new_face = new dface;
            dhalfedge* vertex2edge = new dhalfedge;
            dhalfedge* edge2vertex = new dhalfedge;
            dhalfedge* face2edge = new dhalfedge;
            dhalfedge* edge2face = new dhalfedge;

            new_edge->id = ++new_edge_id;		               //1.id
            new_edges[new_edge->id]=new_edge;	               //存储新边
            new_edge->half = vertex2edge;		               //2.half

            new_face->id = ++new_face_id;
            new_faces[new_face->id] = new_face;		           //存储新面
            new_face->half = vertex2edge;

            vertex2edge->id = ++new_half_id;		               //1.id
            new_half_edges[vertex2edge->id]=vertex2edge;       //存储新半边
            vertex2edge->edge = new_edge;		               //2.edge
            vertex2edge->origin = new_vertex_vertex;           //4.origin
            vertex2edge->left = new_face;				       //5.face
            vertex2edge->next = edge2face;			           //6.next
            vertex2edge->previous = edge2vertex;		           //7.previous

            edge2vertex->id = ++new_half_id;		               //1.id
            new_half_edges[edge2vertex->id]=edge2vertex;	       //存储新半边
            if (last_new_edge!=NULL)
            {
                edge2vertex->edge = last_new_edge;		       //2.edge
                edge2vertex->pair = last_vertex2edge;	       //3.pair
                last_vertex2edge->pair = edge2vertex;	       //3.pair
            }
            else
                first_edge2vertex = edge2vertex;
            edge2vertex->origin = corres_edge_previous_vertex;	//4.origin
            //新边点在这里的half在这里赋值，每个新边点可能会被赋值两次（临边界的只有一次），
            //但是应该不是非常影响效率，应该不会存在新边点的half没有赋值。
            corres_edge_previous_vertex->half = edge2vertex;	   //3.half
            edge2vertex->left = new_face;				       //5.face
            edge2vertex->next = vertex2edge;			           //6.next
            edge2vertex->previous = face2edge;		           //7.previous

            face2edge->id = ++new_half_id;
            new_half_edges[face2edge->id]=face2edge;	//存储新半边
            face2edge_half_edges[current_half->previous]=face2edge;	//为face2edge和edge2face的edge和pair做准备
            face2edge->left = new_face;
            face2edge->next = edge2vertex;
            face2edge->origin = corres_face_vertex;
            face2edge->previous = edge2face;

            edge2face->id = ++new_half_id;
            new_half_edges[edge2face->id]=edge2face;	//存储新半边
            edge2face_half_edges[current_half]=edge2face;	//为face2edge和edge2face的edge和pair做准备
            edge2face->left = new_face;
            edge2face->next = face2edge;
            edge2face->origin = corres_edge_vertex;
            edge2face->previous = vertex2edge;

            last_vertex2edge = vertex2edge;
            last_new_edge = new_edge;
            current_half = current_half->pair->next;
        }
        while (current_half != start_half);
        first_edge2vertex->edge = last_new_edge;		//2.edge
        first_edge2vertex->pair = last_vertex2edge;	//3.pair
        last_vertex2edge->pair = first_edge2vertex;	//3.pair
        new_vertex_vertex->half = last_vertex2edge;
    }

    //遍历新面点，产生face_vertex和edge_vertex之间的edge以及处理face2edge和edge2face的edge
    for (auto it = new_face_vertexs.begin(); it != new_face_vertexs.end(); it++)
    {
        dvertex* face_vertex = it->second;
        dhalfedge* start_half = it->first->half;
        dhalfedge* current_half = start_half;

        dhalfedge* face2edge = NULL;
        dhalfedge* edge2face = NULL;
        dhalfedge* face_half = NULL;
        do
        {
            if (current_half->pair!=NULL)  	//当前边不在边界上，就生成一个edge
            {
                face2edge = face2edge_half_edges[current_half];
                edge2face = edge2face_half_edges[current_half];
                if (face_half==NULL&&face2edge!=NULL)
                {
                    face_half = face2edge;
                }
                if (face2edge==NULL&&edge2face==NULL)
                {
                    //debug("all null really exit!");
                }
                else
                {
                    dedge* new_edge = new dedge;
                    new_edge->id = ++new_edge_id;
                    new_edges[new_edge->id]=new_edge;

                    if (face2edge!=NULL)
                    {
                        face2edge->edge = new_edge;
                        new_edge->half = face2edge;
                    }
                    if (edge2face!=NULL)
                    {
                        edge2face->edge = new_edge;
                        new_edge->half = edge2face;
                    }
                    if (face2edge!=NULL&&edge2face!=NULL)
                    {
                        edge2face->pair = face2edge;
                        face2edge->pair = edge2face;
                    }
                }
            }
            current_half=current_half->next;
        }
        while (current_half!=start_half);
        if (face_half==NULL)
        {
            //debug("imposibal! face vertex's half is null!");
            //delete_new_face_vertexs.push_back(it);
            //new_face_vertexs.erase(it);//似乎不能直接这样删除吧
        }
        else
            face_vertex->half = face_half;
    }
    //释放原来的点，边，半边，面的空间
    delete_values(&_vertexs);
    delete_values(&_half_edges);
    delete_values(&_edges);
	delete_values(&_faces);

    //对half_edges,dedges,faces,vertexs重新赋值
    for (auto it=new_half_edges.begin(); it!=new_half_edges.end(); ++it)
    {
        _half_edges[it->first]=it->second;
    }
    for (auto it=new_edges.begin(); it!=new_edges.end(); ++it)
    {
        _edges[it->first]=it->second;
    }
    for (auto it=new_faces.begin(); it!=new_faces.end(); ++it)
    {
        _faces[it->first]=it->second;
    }
	for (auto it = new_vertexs.begin(); it!=new_vertexs.end(); it++)
	{
		dvertex* current_vertex = *it;
		if (current_vertex->half!=NULL)
		{
			current_vertex->id = ++new_vertex_id;	//点的id在这里赋值，保证id的连续
			_vertexs[current_vertex->id] = current_vertex;
		}
	}

    debug("end Catmull Clark subdivision!");
    end_suspend();
}
void dsolid::loop_subdivision()
{
    begin_suspend();
    debug("begin loop subdivision...");

    int new_vertex_id = 0, new_edge_id = 0, new_half_id = 0, new_face_id = _faces.rbegin()->first;

    //
    //caculate new vertex-vertex's coords and normals
    //
    //产生新点点
    map <dvertex*, dvertex*> new_vertex_vertexs;
    vector<dvertex*> new_vertexs;	//存放所有新点，用vector就够了
    for (auto it=_vertexs.begin(); it!=_vertexs.end(); ++it)
    {
        dvertex* vertex=it->second;
        dhalfedge* half=vertex->half;
        if (half!=NULL)
        {
            dhalfedge* current_half=half;

            int adjecent_vertex_count(0);
            point3f coords;
            vector3f normal;

            bool is_on_boundary = false;
            do
            {
                if (current_half->pair!=NULL)
                {
                    dvertex* current_vertex = current_half->destination();

                    coords+=current_vertex->coords;
                    normal+=current_vertex->normal;

                    adjecent_vertex_count++;
                    current_half=current_half->pair->next;
                }
                else
                {
                    is_on_boundary = true;
                    break;
                }
            }
            while (current_half!=half);

            if (!is_on_boundary)   //如果点不在边界上产生新点点
            {
                float alpha=(float)(5.0f/8-pow((3.0f+2.0f*cos((2*PI)/adjecent_vertex_count))/8,2));
                float beta = alpha/adjecent_vertex_count;

                dvertex* new_vertex_vertex = new dvertex;
                //new_vertex_vertex->id = ++new_vertex_id;//id最后生成，以防中间可能要删除新边点导致id不连续
                new_vertex_vertex->coords=vertex->coords*(1.0f-alpha)+coords*beta;
                new_vertex_vertex->normal=vertex->normal*(1.0f-alpha)+normal*beta;
                new_vertex_vertexs[vertex] = new_vertex_vertex;
                new_vertexs.push_back(new_vertex_vertex);
            }
        }
    }

    //产生新边点，先放在new_edge_vertexs中
    map <dedge*, dvertex*> new_edge_vertexs;
    for (auto it=_edges.begin(); it!=_edges.end(); ++it)
    {
        dhalfedge* left_half = it->second->half;
        if (left_half->pair!=NULL)//如果当前边不是边界才产生新边点
        {
            //
            //get the left and right half
            //
            dhalfedge* right_half=left_half->pair;
            int adjacency_count(0);

            //
            //accumulate the other vertexs in the left face
            //
            dhalfedge* left_current_half=left_half->next;
            point3f left_adjacency_coords;
            vector3f left_adjacency_normal;
            while (left_current_half!=left_half->previous)
            {
                dvertex* left_current_dest_vertex=left_current_half->destination();
                left_adjacency_coords+=left_current_dest_vertex->coords;
                left_adjacency_normal+=left_current_dest_vertex->normal;
                adjacency_count++;
                left_current_half=left_current_half->next;
            }

            //
            //accumulate the other vertexs in the right face
            //
            dhalfedge* right_current_half=right_half->next;
            point3f right_adjacency_coords;
            vector3f right_adjacency_normal;
            while (right_current_half!=right_half->previous)
            {
                dvertex* right_current_dest_vertex=right_current_half->destination();
                right_adjacency_coords+=right_current_dest_vertex->coords;
                right_adjacency_normal+=right_current_dest_vertex->normal;
                adjacency_count++;
                right_current_half=right_current_half->next;
            }

            //
            //create new edge-vertex
            //
            dvertex* new_edge_vertex = new dvertex();
            //new_vertex->id=++new_vertex_id;//id最后生成，以防中间可能要删除新边点导致id不连续

            float factor_v=3.0/8;
            float factor_q=(1-3.0f/4)/adjacency_count;

            new_edge_vertex->coords=
                (left_half->origin->coords+right_half->origin->coords)*factor_v
                +(left_adjacency_coords+right_adjacency_coords)*factor_q;

            new_edge_vertex->normal=
                (left_half->origin->normal+right_half->origin->normal)*factor_v
                +(left_adjacency_normal+right_adjacency_normal)*factor_q;

            new_edge_vertexs[it->second]=new_edge_vertex;
            new_vertexs.push_back(new_edge_vertex);
        }
    }
    map<int, dhalfedge*> new_half_edges;
    map<dhalfedge*, dhalfedge*> edge2edge_half_edges;
    map<int, dedge*> new_edges;
    queue<dface*> singular_faces;

    for (auto it = _faces.begin(); it!=_faces.end(); it++)
    {
        //get the face vertex table
        vector<pair<dvertex*, dhalfedge*>> vertex_table;
        dface* current_face = it->second;
        dhalfedge* start_half = current_face->half;
        dhalfedge* current_half = start_half;

        do
        {
            dvertex* corres_edge_vertex = new_edge_vertexs[current_half->edge];//难道说这里会给每个edge作为key产生一个value？真的是这样的！！
            if (corres_edge_vertex!=NULL)
            {
                vertex_table.push_back(pair<dvertex*, dhalfedge*>(corres_edge_vertex, current_half));
            }
            current_half = current_half->next;
        }
        while (current_half!=start_half);
        if (vertex_table.size()>2)
        {
            dhalfedge* first_half = NULL;
            dhalfedge* last_half =NULL;

            for (auto iter = vertex_table.begin(); iter!=vertex_table.end(); iter++)
            {
                dvertex* current_vertex = iter->first;
                dhalfedge* new_half = new dhalfedge;
                dedge* new_edge = new dedge;

                new_edge->id = ++new_edge_id;		    //1.id
                new_edge->half = new_half;			    //2.half
                new_edges[new_edge->id] = new_edge;		//储存边

                new_half->edge = new_edge;		        //1.edge
                new_half->origin = current_vertex;	    //2.origin
                if (current_vertex->half==NULL)
                {
                    current_vertex->half=new_half;		//4.half
                }
                new_half->id = ++new_half_id;					//3.id
                new_half_edges[new_half->id] = new_half;			//储存半边
                edge2edge_half_edges[iter->second] = new_half;	//为pair准备
                new_half->left = current_face;					//4.left
                if (last_half!=NULL)
                {
                    new_half->previous=last_half;				//5.previous
                    last_half->next = new_half;					//6.next
                }
                else
                {
                    first_half = new_half;
                }
                last_half = new_half;
            }
            first_half->previous=last_half;				//5.previous
            last_half->next = first_half;				//6.next
            current_face->half = first_half;
        }
        else// if (vertex_table.size()==2)
        {
            singular_faces.push(current_face);
        }
    }

    //遍历新点点，与相邻边对应的新边点产生三个半边一个边一个面
    for (auto it = new_vertex_vertexs.begin(); it != new_vertex_vertexs.end(); it++)
    {
        dvertex* old_vertex = it->first;
        dvertex* new_vertex_vertex = it->second;

        dhalfedge* start_half = old_vertex->half;
        dhalfedge* current_half = start_half;
        dedge* last_new_edge = NULL;
        dhalfedge* last_vertex2edge = NULL;
        dhalfedge* first_edge2vertex = NULL;
        do
        {
            dvertex* corres_edge_vertex = new_edge_vertexs[current_half->edge];
            dvertex* corres_edge_previous_vertex = new_edge_vertexs[current_half->previous->edge];

            dedge* new_edge = new dedge;
            dface* new_face = NULL;
            //auto list_it = find(singular_faces.begin(), singular_faces.end(), current_half->left);
            //if (list_it != singular_faces.end())
            //{
            //    new_face = current_half->left;
            //    singular_faces.erase(list_it);
            //}
			if (!singular_faces.empty())
			{
				new_face = singular_faces.front();
				singular_faces.pop();
			}
            else
            {
                new_face = new dface;
                new_face->id = ++new_face_id;
                _faces[new_face->id] = new_face;		//存储新面
            }
            dhalfedge* vertex2edge = new dhalfedge;
            dhalfedge* edge2vertex = new dhalfedge;
            dhalfedge* edge2edge = new dhalfedge;

            new_edge->id = ++new_edge_id;		//1.id
            new_edges[new_edge->id]=new_edge;	//存储新边
            new_edge->half = vertex2edge;		//2.half

            new_face->half = vertex2edge;

            vertex2edge->id = ++new_half_id;		//1.id
            new_half_edges[vertex2edge->id]=vertex2edge;	//存储新半边
            vertex2edge->edge = new_edge;		//2.edge
            vertex2edge->origin = new_vertex_vertex;	//4.origin
            vertex2edge->left = new_face;				//5.face
            vertex2edge->next = edge2edge;			//6.next
            vertex2edge->previous = edge2vertex;		//7.previous

            edge2vertex->id = ++new_half_id;		//1.id
            new_half_edges[edge2vertex->id]=edge2vertex;	//存储新半边
            if (last_new_edge!=NULL)
            {
                edge2vertex->edge = last_new_edge;		//2.edge
                edge2vertex->pair = last_vertex2edge;		//3.pair
                last_vertex2edge->pair = edge2vertex;		//3.pair
            }
            else
                first_edge2vertex = edge2vertex;
            edge2vertex->origin = corres_edge_previous_vertex;	//4.origin
			if (corres_edge_previous_vertex->half == NULL)	//这种情况发生在sigular_face
			{
				corres_edge_previous_vertex->half = edge2vertex;	//3.half
			}
			edge2vertex->left = new_face;				//5.face
            edge2vertex->next = vertex2edge;			//6.next
            edge2vertex->previous = edge2edge;		//7.previous

            edge2edge->id = ++new_half_id;
            new_half_edges[edge2edge->id]=edge2edge;	//存储新半边
            edge2edge->left = new_face;
            edge2edge->next = edge2vertex;
            edge2edge->origin = corres_edge_vertex;
            edge2edge->previous = vertex2edge;
            dhalfedge* edge2edge_pair = edge2edge_half_edges[current_half->previous];
            edge2edge->pair = edge2edge_pair;
            if (edge2edge_pair==NULL)
            {
                dedge* edge2edge_edge = new dedge;
                edge2edge_edge->id = ++new_edge_id;
                edge2edge_edge->half = edge2edge;
                new_edges[edge2edge_edge->id] = edge2edge_edge;
                edge2edge->edge = edge2edge_edge;
            }
            else
            {
                edge2edge_pair->pair = edge2edge;
                edge2edge->edge = edge2edge_pair->edge;
            }

            last_vertex2edge = vertex2edge;
            last_new_edge = new_edge;
            current_half = current_half->pair->next;
        }
        while (current_half != start_half);
        first_edge2vertex->edge = last_new_edge;		//2.edge
        first_edge2vertex->pair = last_vertex2edge;	//3.pair
        last_vertex2edge->pair = first_edge2vertex;	//3.pair
        new_vertex_vertex->half = last_vertex2edge;
    }

    //释放原来的点，边，半边的空间
	delete_values(&_vertexs);
	delete_values(&_half_edges);
	delete_values(&_edges);

	while (!singular_faces.empty())
	{
		dface* redundant_face = singular_faces.front();
		_faces.erase(_faces.find(redundant_face->id));
		singular_faces.pop();
	}
    //对vertexs,half_edges,dedges重新赋值
    for (auto it=new_half_edges.begin(); it!=new_half_edges.end(); ++it)
    {
        _half_edges[it->first]=it->second;
    }
    for (auto it=new_edges.begin(); it!=new_edges.end(); ++it)
    {
        _edges[it->first]=it->second;
    }
    for (auto it = new_vertexs.begin(); it!=new_vertexs.end(); it++)
    {
        dvertex* current_vertex = *it;
        if (current_vertex->half!=NULL)
        {
            current_vertex->id = ++new_vertex_id;	//点的id在这里赋值，保证id的连续
            _vertexs[current_vertex->id] = current_vertex;
        }
    }

    debug("end");
    end_suspend();
}