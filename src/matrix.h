/*******************************************************************************
 File:				Matrix.h

 Author: 			Gaspard Petit (gaspardpetit@gmail.com)
 Last Revision:		March 14, 2007
 
 This code may be reused without my permission as long as credits are given to
 the original author.  If you find bugs, please send me a note...
*******************************************************************************/
#ifndef DCEL_MK_GEOMETRY_MATRIX_H
#define DCEL_MK_GEOMETRY_MATRIX_H

//==============================================================================
//	EXTERNAL DECLARATIONS
//==============================================================================
#include <string>

// uncomment if you have MatrixUtils.h
//#define USE_MATRIXUTILS

//==============================================================================
//	CLASS Matrix
//==============================================================================
template<class _T, int _WIDTH, int _HEIGHT>
class Matrix
{
public:
	typedef _T Type;

public:
	Matrix();
	Matrix(const _T *data);
	Matrix(const Matrix<_T, _WIDTH, _HEIGHT> &m);

	template<class _U>
	Matrix& operator = (const Matrix<_U, _WIDTH, _HEIGHT> &m);
	
	static const Matrix& identityMatrix();
	static const Matrix& zeroMatrix();

	template <int _MW, int _MH>
	static Matrix subMatrix(int r, int c, const Matrix<_T, _MW, _MH> &m);

	bool operator == (const Matrix &m) const;
	bool operator != (const Matrix &m) const;

	const _T* operator [] (int row) const;
	_T* operator [] (int row);

	int width() const;
	int height() const;

	const _T* ptr() const;
	_T* ptr();

	template<class _U>
	Matrix& copy(const _U *data);

	Matrix operator + (const Matrix &rhs) const;
	Matrix& operator += (const Matrix &rhs);
	Matrix operator - (const Matrix &rhs) const;
	Matrix& operator -= (const Matrix &rhs);
	Matrix& operator - ();

	template<int _RHSWIDTH>
	Matrix<_T, _RHSWIDTH, _HEIGHT> operator * (const Matrix<_T, _RHSWIDTH, _WIDTH> &rhs) const;
	Matrix operator *= (const Matrix<_T, _WIDTH, _HEIGHT> &rhs);

	Matrix operator * (const _T &rhs) const;
	Matrix& operator *= (const _T &rhs);

	Matrix operator / (const _T &rhs) const;
	Matrix& operator /= (const _T &rhs);
	
	Matrix<_T, _HEIGHT, _WIDTH> transposed() const;

	//	will work only if the matrix is square
	void transpose();

#ifdef USE_MATRIXUTILS
	void inverse();
	Matrix inversed() const;
#endif // USE_MATRIXUTILS

	bool has_nan() const;
	bool has_inf() const;
	
	std::string serialize() const;
	static Matrix deSerialize(const std::string &str);

	template<int _MW, int _MH>
	Matrix<_T, _WIDTH, _HEIGHT>& copy(int r, int c, const Matrix<_T, _MW, _MH> &m);

	Matrix<_T, _WIDTH, _HEIGHT>& copy_row(int index, const Matrix<_T, _WIDTH, 1> &m);
	Matrix<_T, _WIDTH, _HEIGHT>& copy_col(int index, const Matrix<_T, 1, _HEIGHT> &m);

	Matrix<_T, _WIDTH, 1> row(int index) const;
	Matrix<_T, 1, _HEIGHT> col(int index) const;

	Matrix<_T, _WIDTH, _HEIGHT+1> add_back_row(const Matrix<_T, _WIDTH, 1> &r ) const;
	Matrix<_T, _WIDTH+1, _HEIGHT> add_back_col(const Matrix<_T, 1, _HEIGHT> &c) const;

	Matrix<_T, _WIDTH, _HEIGHT+1> add_front_row(const Matrix<_T, _WIDTH, 1> &r ) const;
	Matrix<_T, _WIDTH+1, _HEIGHT> add_front_col(const Matrix<_T, 1, _HEIGHT> &c) const;

	Matrix<_T, _WIDTH, _HEIGHT+1> add_row(int index, const Matrix<_T, _WIDTH, 1> &r ) const;
	Matrix<_T, _WIDTH+1, _HEIGHT> add_col(int index, const Matrix<_T, 1, _HEIGHT> &c) const;

	Matrix<_T, _WIDTH-1, _HEIGHT> remove_back_col() const;
	Matrix<_T, _WIDTH-1, _HEIGHT> remove_front_col() const;
	Matrix<_T, _WIDTH-1, _HEIGHT> remove_col(int index) const;

	Matrix<_T, _WIDTH, _HEIGHT-1> remove_back_row() const;
	Matrix<_T, _WIDTH, _HEIGHT-1> remove_front_row() const;
	Matrix<_T, _WIDTH, _HEIGHT-1> remove_row(int index) const;

public:
	_T element[_WIDTH*_HEIGHT];

private:
	size_t size() const;
	int count() const;
};

//==============================================================================
//	TYPE DECLARATIONS
//==============================================================================
typedef Matrix<float, 1, 1> matrix1f;
typedef Matrix<float, 2, 2> matrix2f;
typedef Matrix<float, 3, 3> matrix3f;
typedef Matrix<float, 4, 4> matrix4f;
typedef Matrix<double, 1, 1> matrix1d;
typedef Matrix<double, 2, 2> matrix2d;
typedef Matrix<double, 3, 3> matrix3d;
typedef Matrix<double, 4, 4> matrix4d;

//==============================================================================
//	INLINED CODE
//==============================================================================
#include "matrix.inl.h"

#endif // __MK_GEOMETRY_MATRIX__
