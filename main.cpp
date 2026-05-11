#include <cmath>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>

constexpr int width  = 128;
constexpr int height = 128;

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &img, const TGAColor &color);
void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &img, const TGAColor &color);

int main(int argc, char** argv) 
{

    TGAImage framebuffer(width, height, TGAImage::RGB);
    
    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);

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

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &img, const TGAColor &color)
{
    if(ay>by)
    {
        std::swap(ax,bx);
        std::swap(ay,by);
    }
    if(ay>cy)
    {
        std::swap(cx,ax);
        std::swap(cy,ay);
    }
    if(by>cy)
    {
        std::swap(cx,bx);
        std::swap(cy,by);
    }
    int total_height = cy-ay;

    if(ay != by)
    {
        int segment_height = by - ay;
        for (int y = ay; y <= by; y++)
        {
            int x1 = ax + ((cx-ax) * (y-ay)) / total_height;
            int x2 = ax + ((bx-ax) * (y-ay)) / segment_height;
            for(int x = std::min(x1,x2); x < std::max(x1,x2); x++)
            {
                img.set(x,y,color);
            }
        }
    }
    if(by != cy)
    {
        int segment_height = cy - by;
        for (int y = by; y <= cy; y++)
        {
            int x1 = ax + ((cx-ax) * (y-ay)) / total_height;
            int x2 = bx + ((cx-bx) * (y-by)) / segment_height;
            for(int x = std::min(x1,x2); x < std::max(x1,x2); x++)
            {
                img.set(x,y,color);
            }
        }
    }
}
