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
    int level;
};

class HexMap /* : public Mesh */ {
public:

private:
    const unsigned m_map_edge;
    const float m_tile_radius;

    std::vector<int16_t> m_tile_index;
    std::vector<tile> m_tiles;

    std::vector<vertex> m_vertices;
    std::vector<GLuint> m_elements;

    GLuint m_vao;
    GLuint m_vertex_buffer;
    GLuint m_element_buffer;
};

#endif
