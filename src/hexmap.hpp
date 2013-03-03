#ifndef TRAINS_HEXMAP_HPP
#define TRAINS_HEXMAP_HPP

#include <cassert>
#include <vector>

#include <glm/glm.hpp>

#include <stdint.h>

#include "gl3.h"
#include "vertex.hpp"

struct tile {
    unsigned id;
    int height;

    GLuint elements[7];
    std::vector<GLuint> extra_vertices; // FIXME
};

class HexMap /* : public Mesh */ {
public:
    HexMap(unsigned map_edge, float tile_radius)
        : m_map_edge(map_edge)
        , m_tile_radius(tile_radius)
    {
        assert(tile_radius > 0.0f);
        fprintf(stderr, "constructing a hexmap...\n");

        generate_tiles();
        gl_setup();

        fprintf(stderr, "ok\n");
    }

    ~HexMap();

    void generate_tiles();
    void gl_setup();

    glm::vec3 get_centre() const;

    void draw() const;

private:
    const unsigned m_map_edge;
    const float m_tile_radius;

    std::vector<unsigned> m_tile_index;
    std::vector<tile> m_tiles;

    std::vector<vertex> m_vertices;
    GLsizei m_element_count;

    GLuint m_vao;
    GLuint m_vertex_buffer;
    GLuint m_element_buffer;
};

#endif
