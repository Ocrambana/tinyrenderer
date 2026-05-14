#pragma once
#include <vector>
#include "geometry.h"

class Model 
{
public:
    Model() = delete;
    Model(Model&) = delete;
    Model operator=(Model&) = delete;

    Model(const std::string filename);
    int nverts() const {return verts.size();}
    int nfaces() const {return facet_vert.size()/3;}
    vec3 vert(const int i) const {return verts[i];}
    vec3 vert(const int iface, const int nthvert) const {return verts[facet_vert[iface*3+nthvert]];}
private:
    std::vector<vec3> verts = {};
    std::vector<int> facet_vert = {};
};