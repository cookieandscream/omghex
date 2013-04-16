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
    glm::ivec2 grid_position;

    GLuint elements[7];
    GLuint risers[6];
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
        generate_height_data_from_tile_color(11, -5);
//        add_heights_to_vertices_simple(0.2f);
        add_heights_to_vertices_joined(0.2f);
//        add_heights_to_vertices_ridged(1.0f, 4);
        gl_setup();

        fprintf(stderr, "ok\n");
    }

    ~HexMap();

    void generate_tiles();
    void generate_height_data_from_tile_color(int mod, int offset);
    void add_heights_to_vertices_simple(float step);
    void add_heights_to_vertices_joined(float step);
    void add_heights_to_vertices_ridged(float step, int threshold);

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

    unsigned tile_index_at_x_y(int x, int y) const;
    const tile &tile_at_x_y(int x, int y) const;
    tile &tile_at_x_y(int x, int y);
};

inline unsigned HexMap::tile_index_at_x_y(int x, int y) const {
    const unsigned square_edge = 2 * m_map_edge - 1;

    if (x < 0 || y < 0)  return (unsigned) -1;
    if (x >= square_edge || y >= square_edge)  return (unsigned) -1;

    return m_tile_index[y * square_edge + x];
}

inline tile &HexMap::tile_at_x_y(int x, int y) {
    const unsigned index = tile_index_at_x_y(x, y);
    assert(index != (unsigned) -1);
    assert(index < m_tiles.size());

    return m_tiles[index];
}

inline const tile &HexMap::tile_at_x_y(int x, int y) const {
    return tile_at_x_y(x, y);
}

#endif
