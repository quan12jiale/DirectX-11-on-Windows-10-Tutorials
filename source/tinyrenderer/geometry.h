#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vec
{
	double data[n] = { 0 };
	double& operator[](const int i)		 { assert(i >= 0 && i < n); return data[i]; }
	double operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
	double ret = 0;
	for (int i = 0; i < n; i++)
	{
		ret += lhs[i] * rhs[i];
	}
	return ret;
}

template<int n> vec<n> operator*(const vec<n>& lhs, const double rhs) {
	vec<n> ret = lhs;
	for (int i = 0; i < n; i++)
	{
		ret[i] /= rhs;
	}
	return ret;
}

template<> struct vec<3> 
{
	double x = 0, y = 0, z = 0;
	double& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (i==1 ? y :z) : x; }
	double operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (i == 1 ? y : z) : x; }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

template<int n> double norm(const vec<n>& v) {
	return std::sqrt(v*v);
}

template<int n> vec<n> normalized(const vec<n>& v) {
	return v / norm(v);
}

/*
叉积定义
对于两个三维向量：

v1 = (x1, y1, z1)
v2 = (x2, y2, z2)
叉积 v1 × v2 = (y₁z₂ - z₁y₂, z₁x₂ - x₁z₂, x₁y₂ - y₁x₂)
Note: 这里inline为了避免重定义
*/
inline vec3 cross(const vec3& v1, const vec3& v2) {
	return { v1.y * v2.z - v1.z * v2.y,v1.z * v2.x - v1.x * v2.z,v1.x * v2.y - v1.y * v2.x };
}

template<int nrows, int ncols> struct mat
{
	vec<ncols> rows[nrows] = { {} };
	vec<ncols>& operator[](const int idx)	   { assert(idx >= 0 && idx < nrows); return rows[idx]; }
	vec<ncols> operator[](const int idx) const { assert(idx >= 0 && idx < nrows); return rows[idx]; }
};