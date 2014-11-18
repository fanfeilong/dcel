#ifndef DCEL_TRIANGLE_H
#define DCEL_TRIANGLE_H

#include "vector3.h"

template<typename T>
class triangle3
{
public:
	triangle3(){}
	triangle3(triangle3 const& rhs)
	{
		first=rhs.first;
		second=rhs.second;
		third=rhs.third;
	}
	~triangle3(){}
	triangle3& operator=(triangle3 const& rhs)
	{
		if (&rhs==this)
			return *this;
		return triangle3(rhs);
	}
private:
	point3<T> first;
	point3<T> second;
	point3<T> third;
};

#endif