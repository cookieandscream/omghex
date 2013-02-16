#ifndef TRAINS_MESH_H
#define TRAINS_MESH_H

class Mesh {
    public:
        Mesh() = 0;
        ~Mesh() = 0;

        // some sort of interface to get the vertices
        // some sort of interface to get the elements
        // some sort of interface to get the texture data
        
        void render() const;  // take render target as param?

    private:

};

#endif
