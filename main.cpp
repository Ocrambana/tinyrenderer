#include <cmath>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>

constexpr int width  = 800;
constexpr int height = 800;

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &img, const TGAColor &color);
std::tuple<int,int> project(vec3 v); 
void draw_model(const Model &m, TGAImage &img, const TGAColor &pointc, const TGAColor &linec);

int main(int argc, char** argv) 
{

    TGAImage framebuffer(width, height, TGAImage::RGB);
    Model *m= new Model(argv[1]);
    draw_model(*m,framebuffer,white,red);
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

void line(int ax, int ay, int bx, int by, TGAImage &img, const TGAColor &color)
{
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if(steep)
    {
        std::swap(ax,ay);
        std::swap(bx,by);
    }
    if(ax>bx)
    {
        std::swap(ax,bx);
        std::swap(ay,by);
    }

    int y = ay;
    int ierr = 0;
    for(int x = ax; x<=bx; x++)
    {
        if(steep)
        {
            img.set(y,x,color);
        }
        else
        {
            img.set(x,y,color);
        }

        ierr += 2 * std::abs(by-ay);
        y += (by > ay ? 1 : -1) * (ierr > bx-ax);
        ierr -= 2 *(bx - ax) * (ierr > bx-ax);
    }
}

std::tuple<int,int> project(vec3 v)
{
    return {
        (v.x + 1.) * width/2,
        (v.y + 1.) * height/2,
    };
}

void draw_model(const Model &m, TGAImage &img, const TGAColor &pointc, const TGAColor &linec)
{
    vec3 v1,v2,v3;
    for(int i=0; i <= m.nfaces(); i++)
    {
        v1 = m.vert(i,0);
        v2 = m.vert(i,1);
        v3 = m.vert(i,2);

        auto [ax,ay] = project(v1);
        
        auto [bx,by] = project(v2);
        auto [cx,cy] = project(v3);
        line(ax,ay,bx,by,img,linec);
        line(ax,ay,bx,by,img,linec);
        line(cx,cy,bx,by,img,linec);
        img.set(ax,ay,pointc);
        img.set(bx,by,pointc);
        img.set(cx,cy,pointc);
    }
}