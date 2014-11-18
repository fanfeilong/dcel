#ifndef DCEL_VECTOR_4_H
#define DCEL_VECTOR_4_H

#include <assert.h>
#include <cmath>

template<typename T> 
class vector4
{
public:
	vector4(T const & x_,T const & y_,T const & z_,T const & w_)
		:_x(x_),_y(y_),_z(z_),_w(w_)
	{

	}
	vector4():_x(0),_y(0),_z(0),_w(0.0)
	{

	}
	vector4(vector4<T> const & v)
	{
		_x=v._x;
		_y=v._y;
		_z=v._z;
		_w=v._w;
	}
	~vector4(){}
public:
	T& operator[](int i){
		assert(i>=0&&i<4);
		if(i==0) return _x;
		else if(i==1) return _y;
		else if(i==2) return _z;
		else return _w;
	}
	vector4<T>& operator=(vector4<T> const & v)
	{
		if(&v==this)
			return *this;
		_x=v._x;
		_y=v._y;
		_z=v._z;
		_w=v._w;
		return *this;
	}

	bool operator == (vector4<T> const & v) const
	{
		if(_x!=v._x) return false;
		if(_y!=v._y) return false;
		if(_z!=v._z) return false;
		if(_w!=v._w) return false;
		return true;
	}
	bool operator != (vector4<T> const & v) const
	{
		return !((*this)==v);
	}

	vector4<T> operator+(vector4<T> const & other) const
	{
		vector4<T> result(_x+other._x,_y+other._y,_z+other._z,_w+other._w);
		return result;
	}
	vector4<T> operator-(vector4<T> const & other) const
	{
		vector4<T> result(_x-other._x,_y-other._y,_z-other._z,_w-other._w);
		return result;
	}
	vector4<T> operator*(T const & k) const
	{
		vector4<T> result(_x*k,_y*k,_z*k,_w*k);
		return result;
	}
	vector4<T> operator/(T const & k) const
	{
		assert(k!=0);
		vector4<T> result(_x/k,_y/k,_z/k,_w/k);
		return result;
	}

	vector4<T>& operator+=(vector4<T> const & other)
	{
		_x+=other._x;
		_y+=other._y;
		_z+=other._z;
		_w+=other._w;
		return *this;
	}
	vector4<T>& operator-=(vector4<T> const & other)
	{
		_x-=other._x;
		_y-=other._y;
		_z-=other._z;
		_w-=other._w;
		return *this;
	}
	vector4<T>& operator*=(T const & k)
	{
		_x*=k;
		_y*=k;
		_z*=k;
		_w*=k;
		return *this;
	}
	vector4<T>& operator/=(T const & k)
	{
		assert(k!=0);
		_x/=k;
		_y/=k;
		_z/=k;
		_w/=k;
		return *this;
	}

	T operator*(vector4<T> const & v)
	{
		return this->dot(v);
	}
public:
	void zero() {_x = _y = _z = _w = (T)0;}
	T norm()
	{
		return (T)std::sqrt(_x*_x+_y*_y+_z*_z+_w*_w);
	}
	void vector4<T>::normalize()
	{
		T len;
		len = (T)sqrt(_x*_x + _y*_y + _z*_z+_w*_w);
		if (len == 0) len = (T)1.0;
		_x /= len; _y /= len; _z /= len; _w/=len;
	}
	T dot(vector4<T> const & v)
	{
		return _x*v._x+_y*v._y+_z*v._z+_w*v._w;
	}

	static T norm(const vector4<T> &v)
	{
		T len;
		len = (T)sqrt(v._x*v._x + v._y*v._y + v._z*v._z+v._w*v._w);
		return len;
	}
	static vector4<T> normalize(const vector4<T> &v)
	{
		T len;
		len = (T)sqrt(v._x*v._x + v._y*v._y + v._z*v._z+v._w*v._w);
		if (len == 0) len = (T)1.0;
		return vector4<T>(v._x/len, v._y/len, v._z/len,v._w/len);
	}
	static T dot(vector4<T> const & v1,vector4<T> const & v2)
	{
		return v1._x*v2._x+v1._y*v2._y+v1._z*v2._z+v1._w*v2._w;
	}

public:
	void from_c_array4(T t[])
	{
		_x=t[0];
		_y=t[1];
		_z=t[2];
		_w=t[3];
	}
	T* to_c_array4() const
	{
		T t[4];
		t[0]=_x;
		t[1]=_y;
		t[2]=_z;
		t[3]=_w;
	}

private:
	T _x,_y,_z,_w;
};

typedef vector4<int> vector4i;
typedef vector4<float> vector4f;
typedef vector4<double> vector4d;

#endif