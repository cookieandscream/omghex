#include <glm/glm.hpp>

#include "color_table.h"
#include "utils.h"

#include "hexmap.hpp"

const static float sqrt_3 = 1.732051f;

HexMap::~HexMap() {
    fprintf(stderr, "destructing a hexmap\n");
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_element_buffer);
}

void HexMap::generate_tiles() {
    fprintf(stderr, "generating geometry\n");

    const unsigned square_edge = 2 * m_map_edge - 1;
    const unsigned tile_count = 3 * m_map_edge * (m_map_edge - 1) + 1;

    m_tile_index.clear();
    m_tile_index.resize(square_edge * square_edge, -1);

    m_tiles.clear();
    m_tiles.reserve(tile_count);

    m_vertices.clear();
    m_vertices.reserve(7 * tile_count);

    for (int map_y = 0; map_y < square_edge; map_y ++) {
        for (int map_x = 0; map_x < square_edge; map_x ++) {
            if (map_y - map_x >= (int) m_map_edge || map_x - map_y >= (int) m_map_edge) {
                continue;
            }

            tile tmp;
            tmp.id = m_tiles.size();
            m_tiles.push_back(tmp);

            tile &t = m_tiles.back();
            m_tile_index[map_y * square_edge + map_x] = t.id;

            const unsigned v_pos = m_vertices.size();
            const unsigned color_index = 3 * (t.id % color_table_num_colors);

            glm::vec4 color;
            if (map_y == m_map_edge - 1 && map_x == map_y) {
                // chuck a black one in the middle to make orientation of large map more obvious
                color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
            else {
                color = glm::vec4(
                                  float(color_table[color_index + 0]) / 255.0f,
                                  float(color_table[color_index + 1]) / 255.0f,
                                  float(color_table[color_index + 2]) / 255.0f,
                                  1.0f
                                  );
            }

            const glm::vec4 tile_origin = glm::vec4(
                                                    float(2 * map_x - map_y),
                                                    float(3 * map_y) / sqrt_3,
                                                    0.0f,
                                                    0.0f
                                                    ) * m_tile_radius;

            const float r = m_tile_radius;
            const float r_sqrt_3 = r / sqrt_3;
            const float r_2_sqrt_3 = 2.0f * r / sqrt_3;

            const vertex vertices[] = {
                { tile_origin + glm::vec4(0.0f),                            color },
                { tile_origin + glm::vec4(r, -r_sqrt_3, 0.0f, 0.0f),        color },
                { tile_origin + glm::vec4(r,  r_sqrt_3, 0.0f, 0.0f),        color },
                { tile_origin + glm::vec4(0.0f, r_2_sqrt_3, 0.0f, 0.0f),    color },
                { tile_origin + glm::vec4(-r, r_sqrt_3, 0.0f, 0.0f),        color },
                { tile_origin + glm::vec4(-r, -r_sqrt_3, 0.0f, 0.0f),       color },
                { tile_origin + glm::vec4(0.0f, -r_2_sqrt_3, 0.0f, 0.0f),   color },
            };

            for (unsigned i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
                m_vertices.push_back(vertices[i]);
                t.elements[i] = v_pos + i;
            }
        }
    }

    fprintf(stderr, "generated %zu vertices\n", m_vertices.size());
    fprintf(stderr, "generated %zu tiles\n", m_tiles.size());
}

void HexMap::generate_height_data_from_tile_color(int mod, int offset) {
    for (std::vector<tile>::iterator t = m_tiles.begin(); t != m_tiles.end(); ++ t) {
        unsigned color = 3 * (t->id % color_table_num_colors);
        t->height = color % mod + offset;
    }
}

void HexMap::add_heights_to_vertices_simple(float step) {
    for (std::vector<tile>::const_iterator t = m_tiles.begin(); t != m_tiles.end(); ++t) {
        for (unsigned i = 0; i < ARRLEN(t->elements); i++) {
            m_vertices[t->elements[i]].position += glm::vec4(0.0f, 0.0f, step * t->height, 0.0f);
        }
    }
}

void HexMap::gl_setup() {
    fprintf(stderr, "setting up gl objects\n");
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

    std::vector<GLuint> elements;
    elements.reserve(9 * m_tiles.size());
    for (std::vector<tile>::const_iterator t = m_tiles.begin(); t != m_tiles.end(); ++ t) {
        // add elements for the hex fan
        const unsigned element_order[] = { 0, 1, 2, 3, 4, 5, 6, 1 };
        for (int i = 0; i < sizeof(element_order) / sizeof(element_order[0]); i++) {
            elements.push_back(t->elements[element_order[i]]);
        }
        elements.push_back((GLuint) -1);

        // FIXME add elements for risers, if any
    }
    m_element_count = elements.size();

    glGenBuffers(1, &m_element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(elements[0]), elements.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) (4 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void HexMap::draw() const {
    glBindVertexArray(m_vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex((GLuint) -1);
    glDrawElements(GL_TRIANGLE_FAN, m_element_count, GL_UNSIGNED_INT, (void *) 0);
    glDisable(GL_PRIMITIVE_RESTART);
    glBindVertexArray(0);
}

glm::vec3 HexMap::get_centre() const {
    const float map_half_radius = m_tile_radius * m_map_edge - 1;
    return glm::vec3(
                     map_half_radius,
                     sqrt_3 * map_half_radius,
                     0.0f
                     );
}
