#include <cmath>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>
#include <algorithm>

constexpr int width  = 400;
constexpr int height = 400;

constexpr vec3    eye{-1, 0, 2};
constexpr vec3 center{ 0, 0, 0};
constexpr vec3     up{ 0, 1, 0};

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

mat<4,4> ModelView, Viewport, Perspective;

void line(int ax, int ay, int bx, int by, TGAImage &img, const TGAColor &color);
void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy,  int cz,TGAImage &img, double* zBuf, const TGAColor &color);
void rasterize(const vec4 clip[3], double *zbuffer, TGAImage &img, const TGAColor color);

void viewport(const int x, const int y, const int w, const int h)
{
    Viewport = {{
        {w * .5,      0,    0, x + w * .5},
        {     0, h * .5,    0, y + h * .5},
        {     0,      0,    1,          0},
        {     0,      0,    0,          1}
    }};
}

void perspective(double f)
{
    Perspective = {{
        {1, 0,    0, 0},
        {0, 1,    0, 0},
        {0, 0,    1, 0},
        {0, 0, -1/f, 1}
    }};
}

void lookAt(const vec3 &eye, const vec3 &center, const vec3 &up)
{
    vec3 n = normalized(eye - center);
    vec3 l = normalized(cross(up, n));
    vec3 m = normalized(cross(n,l));
    
    mat<4,4> base = {{
        {l.x, l.y, l.z, 0},
        {m.x, m.y, m.z, 0},
        {n.x, n.y, n.z, 0},
        {  0,   0,   0, 1}
    }};

    mat<4,4> trans = {{
        {1, 0, 0, -center.x},
        {0, 1, 0, -center.y},
        {0, 0, 1, -center.z},
        {0, 0, 0,         1},
    }};
    
    ModelView = base * trans;

}

vec3 rotate(vec3 v)
{
    constexpr double a = M_PI /6;
    const mat<3,3> Ry = {{{std::cos(a),0,std::sin(a)},{0,1,0},{-std::sin(a), 0 , std::cos(a)}}};
    return Ry*v;
}

vec3 perspective(vec3 v)
{
    constexpr double c = 3; // distanza del far plane
    return v / (1-v.z/c);
}

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

    lookAt(eye, center, up);
    perspective(normalized(eye-center));
    viewport(width/16,height/16,width*7/8,height*7/8);

    TGAImage framebuffer(width, height, TGAImage::RGB);
    double zBufArray[width*height];
    std::fill(std::begin(zBufArray),std::end(zBufArray), 0.0);
    
    Model m(argv[1]);
    std::srand(std::time({}));
    for (int i = 0; i < m.nfaces(); i++)
    {
        vec4 clip[3];
        for(int d : {0,1,2})
        {
            vec3 v = m.vert(i,d);
            clip[d] = Perspective * ModelView * vec4{v.x,v.y,v.z,1.};
        }
        TGAColor rnd;
        rnd[0] = std::rand()%255;
        rnd[1] = std::rand()%255;
        rnd[2] = std::rand()%255;
        rasterize(clip,zBufArray,framebuffer,rnd);
    }
    
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

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy,  int cz,TGAImage &img, double* zBuf, const TGAColor &color)
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
            double z = alpha * az + beta * bz + gamma * cz;
            
            if(zBuf[x+y*width] >= z) continue;
            zBuf[x+y*width] = z;
            img.set(x,y,color);
        }
        
}

void rasterize(const vec4 clip[3], double *zbuffer, TGAImage &img, const TGAColor color)
{
    vec4 ndc[3] = { clip[0]/clip[0].w, clip[1]/clip[1].w, clip[2]/clip[2].w};
    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy()};

    mat<3,3> ABC = {{
        {screen[0].x,screen[0].y,1.},
        {screen[1].x,screen[1].y,1.},
        {screen[2].x,screen[2].y,1.}
    }};
    if(ABC.det() < 1) return;

    auto [bbminx,bbmaxx] = std::minmax({screen[0].x,screen[1].x,screen[2].x});
    auto [bbminy,bbmaxy] = std::minmax({screen[0].y,screen[1].y,screen[2].y});
#pragma omp parallel for
    for(int x=std::max<int>(bbminx,0); x <= std::min<int>(bbmaxx, img.width() -1 ); x++)
        for(int y=std::max<int>(bbminy,0); y <= std::min<int>(bbmaxy, img.height() -1 ); y++)
        {
            vec3 bc = ABC.invert().transpose() * vec3{static_cast<double>(x),static_cast<double>(y),1.};
            if(bc.x<0||bc.y<0||bc.z<0) continue;
            double z = bc * vec3{ndc[0].z, ndc[1].z,ndc[2].z};
            if(z <= zbuffer[x+y*img.width()]) continue;
            zbuffer[x+y*img.width()] = z;
            img.set(x,y,color);
        }
}