#include <fstream>
#include <sstream>
#include "model.h"

Model::Model(const std::string filename)
{
    std::ifstream source;
    source.open(filename, std::ifstream::in);
    if(source.fail()) return;

    std::string line;
    while(!source.eof())
    {
        std::getline(source, line);
        std::istringstream ss(line.c_str());
        char trash;
        if(!line.compare(0,2,"v "))
        {
            ss >> trash;
            vec3 v;
            for(int i:{0,1,2}) ss >> v[i];
            verts.push_back(v);
        }
        else if(!line.compare(0,3,"vn "))
        {
            ss >> trash >> trash;
            vec3 v;
            for(int i:{0,1,2}) ss >> v[i];
            norms.push_back(normalized(v));
        }
        else if(!line.compare(0,3,"vt "))
        {
            ss >> trash >> trash;
            vec3 v;
            for(int i:{0,1,2}) ss >> v[i];
            uvs.push_back(v);
        }
        else if(!line.compare(0,2,"f "))
        {
            int f,t,n,cnt=0;
            ss >> trash;
            while(ss >> f >> trash >> t >> trash >> n)
            {
                facet_vert.push_back(--f);
                facet_norms.push_back(--n);
                facet_uv.push_back(--t);
                cnt++;
            }
            if(3!=cnt)
            {
                std::cerr << "Errore: il file obj dovrebbe essere triangolato" << std::endl;
                return;
            }
        }
    }
    std::cout << "# v# " << nverts() << " f# " << nfaces() << " n# " << nnorms() << std::endl;
}
