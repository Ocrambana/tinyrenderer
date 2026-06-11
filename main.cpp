#include <cstdlib>
#include "model.h"
#include "our_gl.h"

extern mat<4,4> ModelView, Viewport, Perspective;
extern double *zBuffer;

struct RandomShader : IShader
{
    const Model &model;
    TGAColor color = {};
    vec3 tri[3];

    RandomShader(const Model &m) : model(m) {}

    virtual vec4 vertex(const int face, const int vert)
    {
        vec3 v = model.vert(face,vert);
        vec4 gl_Position = ModelView * vec4{v.x,v.y,v.z,1.};
        tri[vert] = gl_Position.xyz();
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const
    {

        return {false, color};
    }
};

struct PhongShader : IShader
{
    const Model &model;
    vec3 tri[3];
    vec3 sunPos;
    double ambient;
    double specularPower;
    vec3 cameraPos;
    
    PhongShader(const Model &m) : model(m) {}
    
    virtual vec4 vertex(const int face, const int vert)
    {
        vec3 v = model.vert(face,vert);
        vec4 gl_Position = ModelView * vec4{v.x,v.y,v.z,1.};
        tri[vert] = gl_Position.xyz();
        return Perspective * gl_Position;
    }
    
    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const
    {
        vec3 ab = tri[1] - tri[0];
        vec3 ac = tri[2] - tri[0];
        vec3 n =  normalized(cross(ab,ac));
        // vec3 center = (tri[0] + tri[1] + tri[2]) / 3.;

        // vec3 l = normalized(bar - sunPos);
        vec3 l = sunPos;
        double cos_alpha = n * l;
        float diffuse = std::max(0., cos_alpha);

        vec3 r = 2.0 * n * cos_alpha - l;
        r = normalized(r);
        vec3 v = normalized(cameraPos - bar);
        double cos_beta = r * v;
        double specular = std::pow(std::max(0., r.z), specularPower);
        
        double sum = ambient + .4 * diffuse + .9 * specular;
        sum *= 255;
        TGAColor color = {sum,sum,sum, 255};
        return {false, color};
    }
};

int main(int argc, char** argv) 
{
    if(argc < 2)
    {
        std::cerr << "usage: "<< argv[0] << " path/to/model.obj" << std::endl;
        return 1;
    }
    
    constexpr int width  = 400;
    constexpr int height = 400;
    
    constexpr vec3    eye{-1, 0, 2};
    constexpr vec3 center{ 0, 0, 0};
    constexpr vec3     up{ 0, 1, 0};

    lookat(eye, center, up);
    init_perspective(norm(eye-center));
    init_viewport(width/16,height/16,width*7/8,height*7/8);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    for (int i = 1; i < argc; i++)
    {
        Model m(argv[i]);
        PhongShader shader(m);
        shader.sunPos = {-2,0,0};
        shader.ambient = .1;
        shader.cameraPos = eye;
        shader.specularPower = 32.0;
        for (int f = 0; f < m.nfaces(); f++)
        {
            Triangle clip = {
                shader.vertex(f,0),
                shader.vertex(f,1),
                shader.vertex(f,2)
            };
            rasterize(clip, shader, framebuffer);
        }
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
