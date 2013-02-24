#ifndef TRAINS_MESH_HPP
#define TRAINS_MESH_HPP

#include <cassert>
#include <vector>

#include <glm/glm.hpp>

#include "gl3.h"
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

    glm::vec3 get_centre() const;

    void draw() const;

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
