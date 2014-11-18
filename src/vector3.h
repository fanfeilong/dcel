#ifndef DCEL_VECTOR_3_H
#define DCEL_VECTOR_3_H

#include <assert.h>
#include <cmath>
#include <sstream>
#include <string>

template<typename T>
class vector3
{
public:
    vector3(T const & x_,T const & y_,T const & z_)
        :_x(x_),_y(y_),_z(z_)
    {

    }
    vector3()
        :_x(0),_y(0),_z(0)
    {

    }
    vector3(vector3<T> const & v)
    {
        _x=v._x;
        _y=v._y;
        _z=v._z;
    }
    ~vector3() {}
    std::string str()
    {
        std::stringstream sstr;
        sstr<<"x:"<<_x<<","
            <<"y:"<<_y<<","
            <<"z:"<<_z<<std::endl;
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
    T z() const
    {
        return _z;
    }
    void set_x(T const & x_)
    {
        this->_x = x_;
    }
    void set_y(T const & y_)
    {
        this->_y = y_;
    }
    void set_z(T const & z_)
    {
        this->_z = z_;
    }
    void offset_x(T const & delta)
    {
        _x+=delta;
    }
    void offset_y(T const & delta)
    {
        _y+=delta;
    }
    void offset_z(T const & delta)
    {
        _z+=delta;
    }
    void offset(T const& xd,T const& yd,T const& zd)
    {
        _x+=xd;
        _y+=yd;
        _z+=zd;
    }
public:
    T& operator[](int i)
    {
        assert(i>=0&&i<3);
        if(i==0) return _x;
        else if(i==1) return _y;
        else return _z;
    }
    vector3<T>& operator=(vector3<T> const & v)
    {
        if(&v==this)
            return *this;
        _x=v._x;
        _y=v._y;
        _z=v._z;
        return *this;
    }
    bool operator > (vector3<T> const & v) const
    {
        if(_x>v._x) return true;
        else return false;

        if(_y>v._y) return true;
        else return false;

        if(_z>v._z) return true;
        else return false;

        return false;
    }
    bool operator < (vector3<T> const & v) const
    {
        if(_x<v._x) return true;
        else return false;

        if(_y<v._y) return true;
        else return false;

        if(_z<v._z) return true;
        else return false;

        return false;
    }
    bool operator == (vector3<T> const & v) const
    {
        if(_x!=v._x) return false;
        if(_y!=v._y) return false;
        if(_z!=v._z) return false;
        return true;
    }
    bool operator != (vector3<T> const & v) const
    {
        if(_x!=v._x) return true;
        if(_y!=v._y) return true;
        if(_z!=v._z) return true;
        return false;
    }

    vector3<T> operator+(vector3<T> const & other) const
    {
        vector3<T> result(_x+other._x,_y+other._y,_z+other._z);
        return result;
    }
    vector3<T> operator-(vector3<T> const & other) const
    {
        vector3<T> result(_x-other._x,_y-other._y,_z-other._z);
        return result;
    }
    vector3<T> operator*(T const & k) const
    {
        vector3<T> result(_x*k,_y*k,_z*k);
        return result;
    }
    vector3<T> operator/(T const & k) const
    {
        assert(k!=0);
        vector3<T> result(_x/k,_y/k,_z/k);
        return result;
    }

    vector3<T>& operator+=(vector3<T> const & other)
    {
        _x+=other._x;
        _y+=other._y;
        _z+=other._z;
        return *this;
    }
    vector3<T>& operator-=(vector3<T> const & other)
    {
        _x-=other._x;
        _y-=other._y;
        _z-=other._z;
        return *this;
    }
    vector3<T>& operator*=(T const & k)
    {
        _x*=k;
        _y*=k;
        _z*=k;
        return *this;
    }
    vector3<T>& operator/=(T const & k)
    {
        assert(k!=0);
        _x/=k;
        _y/=k;
        _z/=k;
        return *this;
    }

    T operator*(vector3<T> const & v)
    {
        return this->dot(v);
    }
    vector3<T> operator^(vector3<T> const & v)
    {
        return this->cross(v);
    }
public:
    void zero()
    {
        _x = _y = _z = (T)0;
    }
    T norm()
    {
        return (T)std::sqrt(_x*_x+_y*_y+_z*_z);
    }
    void vector3<T>::normalize()
    {
        T len;
        len = (T)sqrt(_x*_x + _y*_y + _z*_z);
        if (len == 0) len = (T)1.0;
        _x /= len;
        _y /= len;
        _z /= len;
    }
    T dot(vector3<T> const & v)
    {
        return _x*v._x+_y*v._y+_z*v._z;
    }
    vector3<T> cross(vector3<T> const & v)
    {
        vector3<T> t;
        t._x = _y * v._z - _z * v._y;
        t._y = _z * v._x - _x * v._z;
        t._z = _x * v._y - _y * v._x;
        return t;
    }
    vector3<T> handman(vector3<T> const & v)
    {
        return vector3<T>(this->_x*v._x,this->_y*v._y,this->_z*v._z);
    }

    static T norm(const vector3<T> &v)
    {
        T len;
        len = (T)sqrt(v._x*v._x + v._y*v._y + v._z*v._z);
        return len;
    }
    static vector3<T> normalize(const vector3<T> &v)
    {
        T len;
        len = (T)sqrt(v._x*v._x + v._y*v._y + v._z*v._z);
        if (len == 0) len = (T)1.0;
        return vector3<T>(v._x/len, v._y/len, v._z/len);
    }
    static T dot(vector3<T> const & v1,vector3<T> const & v2)
    {
        return v1._x*v2._x+v1._y*v2._y+v1._z*v2._z;
    }
    static vector3<T> cross(vector3<T> const & v1,vector3<T> const & v2)
    {
        vector3<T> t;
        t._x = v1._y * v2._z - v1._z * v2._y;
        t._y = v1._z * v2._x - v1._x * v2._z;
        t._z = v1._x * v2._y - v1._y * v2._x;
        return t;
    }
    static vector3<T> handman(vector3<T> const & v1,vector3<T> const & v2)
    {
        vector3<T> t;
        t._x = v1._x * v2._x;
        t._y = v1._y * v2._y;
        t._z = v1._z * v2._z;
        return t;
    }
private:
    T _x,_y,_z;
};

typedef vector3<int> vector3i;
typedef vector3<float> vector3f;
typedef vector3<double> vector3d;
#endif