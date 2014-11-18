#include "vertex.h"
#include "halfedge.h"
#include <string>

using std::string;

dvertex::dvertex():id(-1),is_picked(false)
{
	half = NULL;
	initialize_beg();
	initialize_end();
}
dvertex::dvertex(dvertex const& v)
{
	id     = v.id;
	coords = v.coords;
	normal = v.normal;
	half   = v.half;

	initialize_beg();
	initialize_end();
}
dvertex::~dvertex()
{

}

dvertex& dvertex::operator=(dvertex const& v)
{
	if (&v==this)
	{
		return *this;
	}

	id     = v.id;
	coords = v.coords;
	normal = v.normal;
	half   = v.half;

	initialize_beg();
	initialize_end();

	return *this;
}
dvertex::operator string() const
{
	std::stringstream sstr;
	sstr<<"Vertex"<<" "
		<<id<<" "
		<<coords.x()<<" "
		<<coords.y()<<" "
		<<coords.z()<<" "
		<<"{normal=("
		<<normal.x()<<" "
		<<normal.y()<<" "
		<<normal.z()
		<<")}";
	return sstr.str();
}

std::istream& operator>>(std::istream& in,dvertex& v)
{
	std::string str;
	std::getline(in,str);
	v.init_from_string(str);
	return in;
}
std::ostream& operator<<(std::ostream& out,dvertex const& v)
{
	out<<static_cast<string>(v);
	return out;
}
std::ostream& operator<<(std::ostream& out,dvertex const* v)
{
	out<<static_cast<string>(*v);
	return out;
}

void dvertex::init_from_string(std::string const& line)
{
	string str;
	stlpache::string_token_iterator iter(line, " \n");

	//Read Id
	str = *(++iter);
	id=atoi(str.c_str());

	//Read Coords
	for(int i=0; i<3; i++)
	{
		str=*(++iter);
		coords[i]=static_cast<float>(atof(str.c_str()));
	}

	//Read Normal
	int nbeg = line.find_first_of("(");
	int nend = line.find_first_of(")");

	if( nbeg >= 0 && nend >= 0 )
	{
		string str_normal = line.substr( nbeg+1, nend-nbeg-1 );
		stlpache::string_token_iterator niter(str_normal, " \n");
		for (int i=0; i<3; i++)
		{
			str=*(niter++);
			normal[i]=(static_cast<float>(atof(str.c_str())));
		}
	}
}
