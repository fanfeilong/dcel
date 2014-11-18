#ifndef DCEL_QUATERNION_H
#define DCEL_QUATERNION_H

#include <assert.h>
#include "matrix.h" 

template<typename T> 
class quaternion
{
public:
	quaternion()
	{

	}
	quaternion(T const &  x_,T const & y_,T const & z_,T const & w_)
		:x(x_),y(y_),z(z_),w(w_)
	{

	}
	quaternion(quaternion<T> const & q)
		:x(q.x),y(q.y),z(q.z),w(q.w)
	{

	}
	~quaternion()
	{

	}
public:
	T& operator[](int i)
	{
		assert(i>=0&&i<4);
		if(i==0)
			return x;
		else if(i==1)
			return y;
		else if(i==2)
			return z;
		else
			return w;
	}
	bool operator == (quaternion<T> const & q) const
	{
		return x==q.x&&y==q.y&&z==q.z&&w==q.w;
	}
	bool operator != (quaternion<T> const & q) const
	{
		return !(q==(*this));
	}
	quaternion<T> operator + (quaternion<T> const & q) const
	{
		quaternion<T> result(x+q.x,y+q.y,z+q.z,w+q.w);
		return result;
	}
	quaternion<T> operator - (quaternion<T> const & q) const
	{
		quaternion<T> result(x-q.x,y-q.y,z-q.z,w-q.w);
		return result;
	}
	quaternion<T> operator-() const
	{
		quaternion<T> result(-x,-y,-z,-w);
		return result;
	}
	quaternion<T> operator * (T const & k) const
	{
		quaternion<T> result(x*k,y*k,z*k,w*k);
		return result;
	}
	quaternion<T> operator * (quaternion<T> const & q) const
	{
		T i,j,k,r;
		i = w*q.x + x*q.w + y*q.z - z*q.y;
		j = w*q.y + y*q.w + z*q.x - x*q.z;
		k = w*q.z + z*q.w + x*q.y - y*q.x;
		r = w*q.w - x*q.x - y*q.y - z*q.z;
		return quaternion<T>(i,j,k,r);
	}
	quaternion<T> operator / (T const & k) const
	{
		quaternion<T> result(x/k,y/k,z/k,w/k);
	}
	
	quaternion<T>& operator=(quaternion<T> const & q)
	{
		if(&q==this)
			return *this;
		x=q.x;
		y=q.y;
		z=q.z;
		w=q.w;
		return *this;
	}
	quaternion<T>& operator+=(quaternion<T> const & q)
	{
		x+=q.x;
		y+=q.y;
		z+=q.z;
		w+=q.w;
		return *this;
	}
	quaternion<T>& operator-=(quaternion<T> const & q)
	{
		x-=q.x;
		y-=q.y;
		z-=q.z;
		w-=q.w;
		return *this;
	}
	quaternion<T>& operator*=(T const & k)
	{
		x*=k;
		y*=k;
		z*=k;
		w*=k;
		return *this;
	}
	quaternion<T>& operator*=(quaternion<T> const & q)
	{
		T i,j,k,r;
		i = w*q.x + x*q.w + y*q.z - z*q.y;
		j = w*q.y + y*q.w + z*q.x - x*q.z;
		k = w*q.z + z*q.w + x*q.y - y*q.x;
		r = w*q.w - x*q.x - y*q.y - z*q.z;
		x=i;
		y=j;
		z=k;
		w=r;
		return *this;
	}

public:
	quaternion<T> conjugate() const
	{
		return quaternion<T>(-x,-y,-z,w);
	}
	quaternion<T> inverse() const
	{
		T n=normal();
		if(n==(T)0)
			n=(T)1;
		return conjugate()/n;
	}
	T normal() const
	{
		return (T)(x*x+y*y+z*z+w*w);
	}
	T normal2() const
	{
		return (T)(x*x+y*y+z*z);
	}
	void normalize()
	{
		T n = normal();
		if(n==(T)0.0)
		{
			w=(T)1.0;
			x=y=z=(T)0.0;
			return;
		}
		x/=n;
		y/=n;
		z/=n;
		w/=n;
	}

	//Convert to 4x4 rotation Matrix 
	void to_4x4_matrix(Matrix<T, 4, 4> *outMatrix)
	{
		// avoid depending on Matrix.h
		T* m = reinterpret_cast<T*>(outMatrix);

		float xx = x*x;	float xy = x*y;
		float xz = x*z;	float xw = x*w;

		float yy = y*y;	float yz = y*z;
		float yw = y*w;

		float zz = z*z;	float zw = z*w;

		m[0*4+0] = 1-2*(yy+zz); m[1*4+0] =   2*(xy-zw); m[2*4+0] =   2*(xz+yw); m[3*4+0] = 0;
		m[0*4+1] =   2*(xy+zw); m[1*4+1] = 1-2*(xx+zz); m[2*4+1] =   2*(yz-xw); m[3*4+1] = 0;
		m[0*4+2] =   2*(xz-yw); m[1*4+2] =   2*(yz+xw); m[2*4+2] = 1-2*(xx+yy); m[3*4+2] = 0;
		m[0*4+3] =           0; m[1*4+3] =           0; m[2*4+3] =           0; m[3*4+3] = 1;
	}

	//Convert to 3x3 rotation Matrix
	void to_3x3_matrix(Matrix<T, 3, 3> *outMatrix)
	{
		// avoid depending on Matrix.h
		T* m = reinterpret_cast<T*>(outMatrix);

		float xx = x*x;	float xy = x*y;
		float xz = x*z;	float xw = x*w;

		float yy = y*y;	float yz = y*z;
		float yw = y*w;

		float zz = z*z;	float zw = z*w;

		m[0*3+0] = 1-2*(yy+zz); m[1*3+0] =   2*(xy-zw); m[2*3+0] =   2*(xz+yw);
		m[0*3+1] =   2*(xy+zw); m[1*3+1] = 1-2*(xx+zz); m[2*3+1] =   2*(yz-xw);
		m[0*3+2] =   2*(xz-yw); m[1*3+2] =   2*(yz+xw); m[2*3+2] = 1-2*(xx+yy);
	}


	// Create a unit quaternion representing the rotation of the 3D vector v by
	// an angle 2*theta about the 3D axis u. The quaternion is q = cos(theta)+u*sin(theta)
	void from_axis_angle(vector3<T> const  &axis, float angle)
	{
		float c2 = cos(0.5f*angle);
		float s2 = sin(0.5f*angle);

		vector3<T> naxis = axis;
		naxis.normalize();

		w = c2;
		x = naxis.x() * (T)s2;
		y = naxis.y() * (T)s2;
		z = naxis.z() * (T)s2;
		return;
	}

	// Extract the rotation and the axis of rotation in a quaternion representation
	void to_axis_angle(vector3<T> &axis, float &angle)
	{
		angle = 2.0f * acos(w);

		if (angle == 0.0f) 
		{
			axis.set_x((T)0);
			axis.set_y((T)0);
			axis.set_z((T)0);
			return;
		}

		axis.set_x(x);
		axis.set_y(y);
		axis.set_z(z);
		axis.normalize();
		return;
	}

	// Rotate the vector
	void rotate_vector(vector3<T> &v) const
	{
		float vlen = v.norm();
		if (vlen == 0.0f)
			return;

		quaternion<T> vq(v.x(), v.y(), v.z(), (T)0);
		quaternion<T> conj = this->conjugate();
		quaternion<T> temp = (*this)*vq;
		temp = temp*conj;

		v.set_x(temp.x); 
		v.set_y(temp.y); 
		v.set_z(temp.z);

		v.normalize();
		v *= vlen;
		return;
	}

private:
	T x,y,z,w;
};

typedef quaternion<int> quaternioni;
typedef quaternion<float> quaternionf;
typedef quaternion<double> quaterniond;

#endif
