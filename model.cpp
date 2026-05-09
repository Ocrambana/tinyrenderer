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
        else if(!line.compare(0,2,"f "))
        {
            int f,t,n,cnt=0;
            ss >> trash;
            while(ss >> f >> trash >> t >> trash >> n)
            {
                facet_vert.push_back(--f);
                cnt++;
            }
            if(3!=cnt)
            {
                std::cerr << "Errore: il file obj dovrebbe essere triangolato" << std::endl;
                return;
            }
        }
    }
    std::cout << "# v# " << nverts() << " f# " << nfaces() << std::endl;
}
