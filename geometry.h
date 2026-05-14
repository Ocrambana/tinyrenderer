#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vec
{
    double data[n] = {0};
    double& operator[](const int i)       {assert(i>=0 && i<n); return data[i];}
    double  operator[](const int i) const {assert(i>=0 && i<n); return data[i];}
};

template <int n> vec<n> operator-(const vec<n> &rhs)
{
    vec<n> v; 
    for(size_t i =0; i<n;i++) 
        v[i] = - rhs[i]; 
    return v;
}

template <int n> vec<n> operator+(const vec<n> &lhs, const vec<n> &rhs)
{
    vec<n> v; 
    for(size_t i =0; i<n;i++) 
        v[i] = lhs[i] + rhs[i]; 
    return v;
}

template <int n> vec<n> operator-(const vec<n> &lhs, const vec<n> &rhs)
{
    vec<n> v; 
    for(size_t i =0; i<n;i++) 
        v[i] = lhs[i] - rhs[i]; 
    return v;
}

template <int n> double operator*(const double &lhs, const vec<n> &rhs)
{
    double v; 
    for(size_t i =0; i<n;i++) 
        v += lhs * rhs[i]; 
    return v;
}

template <int n> double operator*(const vec<n> &lhs, const double &rhs)
{
    return rhs * lhs;
}

// dot product
template <int n> double operator*(const vec<n> &lhs, const vec<n> &rhs)
{
    double v; 
    for(size_t i =0; i<n;i++) 
        v += lhs[i] * rhs[i]; 
    return v;
}

template <int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v)
{
    for(int i=0; i<n; i++) out << v[i] << " ";
    return out;
}

template<> struct vec<3>
{
    double x=0, y=0, z=0;
    double& operator[](const int i)       {assert(i>=0 && i<3); return i? (1==i ? y : z) : x;}
    double  operator[](const int i) const {assert(i>=0 && i<3); return i? (1==i ? y : z) : x;}
};
typedef vec<3> vec3;

inline vec<3> cross(vec<3> &lhs, vec<3> &rhs)
{
    vec3 v;
    v.x = lhs.y * rhs.z - lhs.z * rhs.y;
    v.y = lhs.z * rhs.x - lhs.x * rhs.z;
    v.z = lhs.x * rhs.y - lhs.y * rhs.x;
    return v;
}

template<> struct vec<2>
{
    double x=0, y=0;
    double& operator[](const int i)       {assert(i>=0 && i<2); return i? y : x;}
    double  operator[](const int i) const {assert(i>=0 && i<2); return i? y : x;}
};
typedef vec<2> vec2;

template<> struct vec<4>
{
    double x=0, y=0, z=0, w=0;
    double& operator[](const int i)       {assert(i>=0 && i<4); return i? (1==i ? y : (2==i ? z : w)) : x;}
    double  operator[](const int i) const {assert(i>=0 && i<4); return i? (1==i ? y : (2==i ? z : w)) : x;}
};
typedef vec<4> vec4;

// template<int n> struct mat
// {
//     double data[n*n] = {0};
//     double& operator[](const int i, const int j)
//     {
//         assert(i>=0 && i<n && j >=0 && j<n); 
//         return data[i+j*n];
//     }

//     double  operator[](const int i, const int j) const 
//     {
//         assert(i>=0 && i<n && j >=0 && j<n); 
//         return data[i+j*n];
//     }
// };



// template<> struct mat<4>
// {
//     double  m_00, m_10, m_20,
//             m_01, m_11, m_21,
//             m_02, m_12, m_22;

//     static double& operator[](const int i, const int j)
//     {
//         assert(i>=0 && i<4 && j >=0 && j<4); 
        
//     }

//     static double  operator[](const int i, const int j) const 
//     {
//         assert(i>=0 && i<n && j >=0 && j<n); 
//         return data[i+j*n];
//     }
// };