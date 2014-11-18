#ifndef DCEL_POINT_3_H
#define DCEL_POINT_3_H

#include <assert.h>
#include <cmath>
#include <string>
#include <sstream>
#include "vector3.h"

template<typename T> 
class point3
{
public:
	point3(T const & x_,T const & y_,T const & z_):_x(x_),_y(y_),_z(z_){}
	point3():_x(0),_y(0),_z(0){}
	point3(point3<T> const & p)
	{
		_x=p._x;
		_y=p._y;
		_z=p._z;
	}
	~point3(){}

public:
	T x()const{return _x;}
	T y()const{return _y;}
	T z()const{return _z;}
	void set_x(T const & x){this->_x = x;}
	void set_y(T const & y){this->_y = y;}
	void set_z(T const & z){this->_z = z;}

	void set_from_c_array3(T array[])
	{
		assert(len(array)==3);
		_x=array[0];
		_y=array[1];
		_z=array[2];
	}
	T* c_array3()
	{
		_array3[0]=_x;
		_array3[1]=_y;
		_array3[2]=_z;
		return _array3;
	}
	T* c_array4()
	{
		_array4[0]=_x;
		_array4[1]=_y;
		_array4[2]=_z;
		_array4[3]=(T)1.0;
		return _array4;
	}
	std::string str()
	{
		std::stringstream sstr;
		sstr<<"x:"<<_x<<","
			<<"y:"<<_y<<","
			<<"z:"<<_z<<std::endl;
		return sstr.str();
	}

public:
	T& operator[](int index)
	{
		assert(index>=0&&index<3);
		if(index==0)
			return _x;
		else if(index==1)
			return _y;
		else
			return _z;
	}
	point3<T>& operator=(point3<T> const & p)
	{
		if(&p==this)
			return *this;

		_x=p._x;
		_y=p._y;
		_z=p._z;
		return *this;
	}
	vector3<T> operator-(point3<T> const & other) const
	{
		vector3<T> result(_x-other._x,_y-other._y,_z-other._z);
		return result;
	}
	point3<T> operator-(vector3<T> const & other) const
	{
		point3<T> result(_x-other.x(),_y-other.y(),_z-other.z());
		return result;
	}
	point3<T>& operator+=(vector3<T> const & v)
	{
		_x+=v.x();
		_y+=v.y();
		_z+=v.z();
		return *this;
	}
	point3<T> operator+(vector3<T> const & v) const
	{
		point3<T> t;
		t._x=this->_x+v.x();
		t._y=this->_y+v.y();
		t._z=this->_z+v.z();
		return t;
	}

	point3<T>& operator+=(point3<T> const & p)
	{
		_x+=p.x();
		_y+=p.y();
		_z+=p.z();
		return *this;
	}
	point3<T> operator+(point3<T> const & p) const
	{
		point3<T> t;
		t._x=this->_x+p.x();
		t._y=this->_y+p.y();
		t._z=this->_z+p.z();
		return t;
	}

	bool operator==(point3<T> const & p) const
	{
		return _x==p._x&&_y==p._y&&_z==p._z;
	}
	bool operator!=(point3<T> const & p)const
	{
		return _x!=p._x||_y!=p._y||_z!=p._z;
	}
	point3<T> operator*(T const & k) const
	{
		point3<T> result(_x*k,_y*k,_z*k);
		return result;
	}
	point3<T>& operator*=(T const & k)
	{
		_x*=k;
		_y*=k;
		_z*=k;
		return *this;
	}
	point3<T> operator/(T const & k) const
	{
		point3<T> result(_x/k,_y/k,_z/k);
		return result;
	}
	point3<T>& operator/=(T const & k)
	{
		_x/=k;
		_y/=k;
		_z/=k;
		return *this;
	}
private:
	T _x,_y,_z;
	T _array3[3];
	T _array4[4];
};

typedef point3<int> point3i;
typedef point3<float> point3f;
typedef point3<double> point3d;

#endif