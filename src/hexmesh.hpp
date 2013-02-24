#ifndef TRAINS_MESH_HPP
#define TRAINS_MESH_HPP

#include <cassert>
#include <vector>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include "SDL.h"

#if defined(__MACOSX__)
#include <OpenGL/gl3.h>
//#include <OpenGL/gl3ext.h>
#else
#include "SDL_opengl.h"
#endif

#include "vertex.hpp"

class HexMesh /* public Mesh */ {
public:
    HexMesh(unsigned map_edge, float tile_radius)
        : m_map_edge(map_edge)
        , m_tile_radius(tile_radius)
        , m_vertices()
        , m_elements()
    {
        assert(tile_radius > 0.0f);
        fprintf(stderr, "constructing a hexmesh\n");

        generate_geometry();
        gl_setup();

        fprintf(stderr, "ok\n");
    }

    ~HexMesh();

    void generate_geometry();
    void gl_setup();

    void draw();

private:
    const unsigned m_map_edge;
    const float m_tile_radius;

    std::vector<vertex> m_vertices;
    std::vector<GLuint> m_elements;

    GLuint m_vao;
    GLuint m_vertex_buffer;
    GLuint m_element_buffer;
};


#endif
