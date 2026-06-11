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

template <int n> vec<n> operator*(const double &lhs, const vec<n> &rhs)
{
    vec<n> v; 
    for(size_t i =0; i<n;i++) 
        v [i]= lhs * rhs[i]; 
    return v;
}

template <int n> vec<n> operator*(const vec<n> &lhs, const double &rhs)
{
    return rhs * lhs;
}

// dot product
template <int n> double operator*(const vec<n> &lhs, const vec<n> &rhs)
{
    double v = 0; 
    for(size_t i =0; i<n;i++) 
        v += lhs[i] * rhs[i]; 
    return v;
}

template <int n> vec<n> operator/(const vec<n> lhs, const double& rhs)
{
    vec<n> v = lhs;
    for(size_t i=0;i<n;i++)
        v[i] /= rhs;
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
    double& operator[](const int i)       {assert(i>=0 && i<4); return i<2? (i? y : x) : (2==i? z : w);}
    double  operator[](const int i) const {assert(i>=0 && i<4); return i<2? (i? y : x) : (2==i? z : w);}
    vec<2> xy()  const {return {x,y};   }
    vec<3> xyz() const {return {x,y,z}; }
};
typedef vec<4> vec4;

template<int n> double norm(const vec<n> &v)
{
    return std::sqrt(v*v);
}

template<int n> vec<n> normalized(const vec<n> &v)
{
    return v / norm(v);    
}

inline vec<3> cross(const vec<3> &lhs, const vec<3> &rhs)
{
    return {    
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    };
}

template <int n> struct dt;

template<int nrows, int ncols> struct mat
{
    vec<ncols> rows[nrows] = {{}};

          vec<ncols>& operator[] (const int idx)       {assert(idx>=0 && idx <nrows); return rows[idx];}
    const vec<ncols>& operator[] (const int idx) const {assert(idx>=0 && idx <nrows); return rows[idx];}

    double det() const
    {
        return dt<ncols>::det(*this);
    }

    double cofactor(const int row, const int col) const
    {
        size_t row_t = static_cast<size_t>(row);
        size_t col_t = static_cast<size_t>(col);
        mat<nrows-1,ncols-1> submat;
        for (size_t i = 0; i < nrows-1; i++)
            for (size_t j = 0; j < ncols-1; j++)
            {
                submat[i][j] =  rows[i+int(i>=row_t)][j+int(j>=col_t)];               
            }
        return submat.det() * ((row+col)%2? -1 : 1);
    }

    mat<ncols,nrows> transpose() const
    {
        mat<ncols,nrows> mat;
        for (size_t i = 0; i < ncols; i++)
            for (size_t j = 0; j < nrows; j++)
            {
                mat[i][j] =  rows[j][i];               
            }
        return mat;
    }

    mat<ncols,nrows> adjoint() const
    {
        mat<ncols,nrows> adj;
        for (size_t i = 0; i < ncols; i++)
            for (size_t j = 0; j < nrows; j++)
            {
                adj[i][j] =  cofactor(j,i);               
            }
        return adj;
    }

    mat<nrows,ncols> invert()
    {
        return adjoint() / det();
    }
};

template<int nrows, int ncols> vec<ncols> operator*(const vec<nrows> &lhs, const mat<nrows,ncols> &rhs)
{
    return (mat<1,nrows>{{lhs}} * rhs)[0];
}

template<int nrows, int ncols> vec<nrows> operator*(const mat<nrows,ncols> &lhs, const vec<ncols> &rhs)
{
    vec<nrows> res;
    for(int i =0; i < ncols; i++)
    {
        res[i] = lhs[i] * rhs; 
    }
    return res;
}

template <int nrows, int ncol1, int ncol2> mat<nrows, ncol2> operator*(const mat<nrows,ncol1> &lhs, const mat<ncol1,ncol2> &rhs)
{
    mat<nrows,ncol2> res;
    for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncol2; j++)
            for (size_t k = 0; k < ncol1; k++)
            {
                res[i][j] += lhs[i][k]*rhs[k][j];
            }
    return res;                        
}

template<int nrows, int ncols> mat<nrows,ncols> operator*(const mat<nrows, ncols> &lhs, double &rhs)
{
    mat<nrows,ncols> res;
    for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
        {
            res[i][j] = lhs[i][j] * rhs;
        }
    return res;            
}

template<int nrows, int ncols> mat<nrows,ncols> operator*(const double &lhs, const mat<nrows, ncols> &rhs)
{
    return rhs*lhs;
}

template<int nrows, int ncols> mat<nrows,ncols> operator/(const mat<nrows, ncols> &lhs, const double &rhs)
{
    mat<nrows,ncols> res;
    for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
        {
            res[i][j] = lhs[i][j] / rhs;
        }
    return res;            
}

template <int nrows, int ncols> mat<nrows, ncols> operator+(const mat<nrows,ncols> &lhs, const mat<nrows,ncols> &rhs)
{
    mat<nrows,ncols> res;
    for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
        {
            res[i][j] = lhs[i][j]+rhs[i][j];
        }
    return res;                        
}

template <int nrows, int ncols> mat<nrows, ncols> operator-(const mat<nrows,ncols> &lhs, const mat<nrows,ncols> &rhs)
{
    mat<nrows,ncols> res;
    for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
        {
            res[i][j] = lhs[i][j]-rhs[i][j];
        }
    return res;                        
}

template <int nrows, int ncols> std::ostream& operator<<(std::ostream &out, const mat<nrows,ncols> &m)
{
    for (size_t i = 0; i < nrows; i++)
    {
        out << m[i] << std::endl;
    }
    return out;
}


template <int n> struct dt
{
    static double det(const mat<n,n> &src)
    {
        double det = 0.;
        for (size_t i = 0; i < n; i++)
        {
            det += src[0][i] * src.cofactor(0,i);
        }
        return det;
    }
};

template<> struct dt<1>
{
    static double det(const mat<1,1> &src)
    {
        return src[0][0];
    }
    
};