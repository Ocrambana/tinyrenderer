#include <cmath>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>

constexpr int width  = 400;
constexpr int height = 400;

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &img, const TGAColor &color);
void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy,  int cz,TGAImage &img, TGAImage &zBuf, const TGAColor &color);

std::tuple<int,int,int> project(vec3 v)
{
    return {
        (v.x + 1.) * width/2,
        (v.y + 1.) * height/2,
        (v.z + 1.) * 255./2,
    };
}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy)
{
    return .5 * ((by-ay) * (bx+ax) + (cy-by) * (cx+bx) + (ay-cy)*(ax+cx));
}

int main(int argc, char** argv) 
{
    if(argc != 2)
    {
        std::cerr << "usage: "<< argv[0] << " path/to/model.obj" << std::endl;
        return 1;
    }

    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zBuffer(width, height, TGAImage::GRAYSCALE);
    Model m(argv[1]);

    std::srand(std::time({}));
    for (int i = 0; i < m.nfaces(); i++)
    {
        auto [ax,ay, az] = project(m.vert(i, 0));
        auto [bx,by, bz] = project(m.vert(i, 1));
        auto [cx,cy, cz] = project(m.vert(i, 2));
        TGAColor rnd;
        rnd[0] = std::rand()%255;
        rnd[1] = std::rand()%255;
        rnd[2] = std::rand()%255;
        triangle(ax,ay,az,bx,by,bz,cx,cy,cz, framebuffer, zBuffer,rnd);
    }
    

    framebuffer.write_tga_file("framebuffer.tga");
    zBuffer.write_tga_file("zbuffer.tga");
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

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy,  int cz,TGAImage &img, TGAImage &zBuf, const TGAColor &color)
{
    int bbminx = std::min(std::min(ax,bx),cx);
    int bbminy = std::min(std::min(ay,by),cy);
    int bbmaxx = std::max(std::max(ax,bx),cx);
    int bbmaxy = std::max(std::max(ay,by),cy);
    double total_area = signed_triangle_area(ax,ay,bx,by,cx,cy);
    if(total_area < 1.) return;

#pragma omp parallel for
    for(int x = bbminx; x <= bbmaxx; x++)
        for (int y = bbminy; y < bbmaxy; y++)
        {
            double alpha = signed_triangle_area(x,y,bx,by,cx,cy) / total_area;
            double beta = signed_triangle_area(x,y,cx,cy,ax,ay)/ total_area;
            double gamma = signed_triangle_area(x,y,ax,ay,bx,by)/ total_area;
            
            if(alpha < 0 || beta < 0 || gamma < 0) continue;
            unsigned char z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            
            if(zBuf.get(x,y)[0] >= z) continue;

            zBuf.set(x,y,{z});
            img.set(x,y,color);
        }
        
}
