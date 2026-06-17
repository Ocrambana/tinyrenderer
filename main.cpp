#include <cstdlib>
#include "model.h"
#include "our_gl.h"

extern mat<4,4> ModelView, Viewport, Perspective;
extern double *zBuffer;

struct PhongShader : IShader
{
    const Model &model;
    vec3 tri[3];
    vec3 norm[3];
    vec3 l;
    double ambient;
    double specularPower;
    
    PhongShader(const Model &m, const vec3 light) : model(m) 
    {
        l = normalized((ModelView*vec4{light.x,light.y,light.z,0.}).xyz());
    }
    
    virtual vec4 vertex(const int face, const int vert)
    {
        vec3 v = model.vert(face,vert);
        vec4 gl_Position = ModelView * vec4{v.x,v.y,v.z,1.};
        tri[vert] = gl_Position.xyz();

        vec3 n = model.norm(face,vert);
        norm[vert] =(ModelView.invert().transpose() * vec4{n.x,n.y,n.z,0.}).xyz();
        
        return Perspective * gl_Position;
    }
    
    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const
    {
        vec3 n =  normalized(
            norm[0] * bar[0]+
            norm[1] * bar[1]+
            norm[2] * bar[2]
        );

        float diffuse = std::max(0., n*l);

        vec3 r =  n * (n*l) * 2. - l;
        r = normalized(r);
        double specular = std::pow(std::max(0., r.z), specularPower);
        
        double sum = std::min(1., ambient + .4 * diffuse + .9 * specular);
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
    constexpr vec3  light{ 1, 1, 1};
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
        PhongShader shader(m,light);
        shader.ambient = .3;
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
