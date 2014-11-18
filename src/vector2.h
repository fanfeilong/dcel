#ifndef DCEL_VECTOR_2_H
#define DCEL_VECTOR_2_H

#include <assert.h>
#include <cmath>
#include <sstream>
#include <string>

template<typename T>
class vector2
{
public:
	vector2(T const & x_,T const & y_)
		:_x(x_),_y(y_)
	{

	}
	vector2()
		:_x(0),_y(0)
	{

	}
	vector2(vector2<T> const & v)
	{
		_x=v._x;
		_y=v._y;
	}
	~vector2() {}
	std::string str()
	{
		std::stringstream sstr;
		sstr<<"x:"<<_x<<","
			<<"y:"<<_y<<std::endl;
		return sstr.str();
	}
public:
	T x() const
	{
		return _x;
	}
	T y() const
	{
		return _y;
	}
	void set_x(T const & x_)
	{
		this->_x = x_;
	}
	void set_y(T const & y_)
	{
		this->_y = y_;
	}
	void offset_x(T const & delta)
	{
		_x+=delta;
	}
	void offset_y(T const & delta)
	{
		_y+=delta;
	}
	void offset(T const& xd,T const& yd)
	{
		_x+=xd;
		_y+=yd;
	}
public:
	T& operator[](int i)
	{
		assert(i>=0&&i<2);
		if(i==0) return _x;
		else return _y;
	}
	vector2<T>& operator=(vector2<T> const & v)
	{
		if(&v==this)
			return *this;
		_x=v._x;
		_y=v._y;
		return *this;
	}
	bool operator > (vector2<T> const & v) const
	{
		if(_x>v._x) return true;
		else return false;

		if(_y>v._y) return true;
		else return false;

		return false;
	}
	bool operator < (vector2<T> const & v) const
	{
		if(_x<v._x) return true;
		else return false;

		if(_y<v._y) return true;
		else return false;

		return false;
	}
	bool operator == (vector2<T> const & v) const
	{
		if(_x!=v._x) return false;
		if(_y!=v._y) return false;
		return true;
	}
	bool operator != (vector2<T> const & v) const
	{
		if(_x!=v._x) return true;
		if(_y!=v._y) return true;
		return false;
	}

	vector2<T> operator+(vector2<T> const & other) const
	{
		vector2<T> result(_x+other._x,_y+other._y);
		return result;
	}
	vector2<T> operator-(vector2<T> const & other) const
	{
		vector2<T> result(_x-other._x,_y-other._y);
		return result;
	}
	vector2<T> operator*(T const & k) const
	{
		vector2<T> result(_x*k,_y*k);
		return result;
	}
	vector2<T> operator/(T const & k) const
	{
		assert(k!=0);
		vector2<T> result(_x/k,_y/k);
		return result;
	}

	vector2<T>& operator+=(vector2<T> const & other)
	{
		_x+=other._x;
		_y+=other._y;
		return *this;
	}
	vector2<T>& operator-=(vector2<T> const & other)
	{
		_x-=other._x;
		_y-=other._y;
		return *this;
	}
	vector2<T>& operator*=(T const & k)
	{
		_x*=k;
		_y*=k;
		return *this;
	}
	vector2<T>& operator/=(T const & k)
	{
		assert(k!=0);
		_x/=k;
		_y/=k;
		return *this;
	}

	T operator*(vector2<T> const & v)
	{
		return this->dot(v);
	}
	//vector2<T> operator^(vector2<T> const & v)
	//{
	//	return this->cross(v);
	//}
public:
	void zero()
	{
		_x = _y = (T)0;
	}
	T norm()
	{
		return (T)std::sqrt(_x*_x+_y*_y);
	}
	void vector2<T>::normalize()
	{
		T len;
		len = (T)sqrt(_x*_x + _y*_y);
		if (len == 0) len = (T)1.0;
		_x /= len;
		_y /= len;
	}
	T dot(vector2<T> const & v)
	{
		return _x*v._x+_y*v._y;
	}
	//vector2<T> cross(vector2<T> const & v)
	//{
	//	vector2<T> t;
	//	t._x = ;
	//	t._y = ;
	//	return t;
	//}
	vector2<T> handman(vector2<T> const & v)
	{
		return vector2<T>(this->_x*v._x,this->_y*v._y);
	}

	static T norm(const vector2<T> &v)
	{
		T len;
		len = (T)sqrt(v._x*v._x + v._y*v._y);
		return len;
	}
	static vector2<T> normalize(const vector2<T> &v)
	{
		T len;
		len = (T)sqrt(v._x*v._x + v._y*v._y);
		if (len == 0) len = (T)1.0;
		return vector2<T>(v._x/len, v._y/len);
	}
	static T dot(vector2<T> const & v1,vector2<T> const & v2)
	{
		return v1._x*v2._x+v1._y*v2._y;
	}
	//static vector2<T> cross(vector2<T> const & v1,vector2<T> const & v2)
	//{
	//	vector2<T> t;
	//	t._x = v1._y * v2._z - v1._z * v2._y;
	//	t._y = v1._z * v2._x - v1._x * v2._z;
	//	t._z = v1._x * v2._y - v1._y * v2._x;
	//	return t;
	//}
	static vector2<T> handman(vector2<T> const & v1,vector2<T> const & v2)
	{
		vector2<T> t;
		t._x = v1._x * v2._x;
		t._y = v1._y * v2._y;
		return t;
	}
private:
	T _x,_y;
};

typedef vector2<int> vector2i;
typedef vector2<float> vector2f;
typedef vector2<double> vector2d;
#endif