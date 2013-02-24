
#include <glm/glm.hpp>

#include "color_table.h"

#include "hexmesh.hpp"

const static float sqrt_3 = 1.732051f;

const static GLuint elements[] = {
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 5,
    0, 5, 6,
    0, 6, 1,
};

// it's possible to render a hexagon with only four triangles
//const static GLuint elements[] = {
//    1, 2, 4,
//    2, 3, 4,
//    4, 5, 1,
//    5, 6, 1,
//};

HexMesh::~HexMesh() {
    fprintf(stderr, "destructing a hexmesh\n");
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_element_buffer);
}

void HexMesh::generate_geometry() {
    fprintf(stderr, "generating geometry\n");

    const unsigned square_edge = 2 * m_map_edge - 1;

    m_vertices.clear();
    m_vertices.reserve(7 * square_edge * square_edge);

    m_elements.clear();
    m_elements.reserve(6 * 3 * square_edge * square_edge);

    for (int map_y = 0; map_y < square_edge; map_y ++) {
        for (int map_x = 0; map_x < square_edge; map_x ++) {
            if (map_y - map_x >= (int) m_map_edge)  continue;
            if (map_x - map_y >= (int) m_map_edge)  continue;

            const unsigned cell = map_y * square_edge + map_x;
            const unsigned v_pos = m_vertices.size();

            const unsigned color_index = 3 * (cell % color_table_num_colors);
            const glm::vec4 color = glm::vec4(
               float(color_table[color_index + 0]) / 255.0f,
               float(color_table[color_index + 1]) / 255.0f,
               float(color_table[color_index + 2]) / 255.0f,
               1.0f
            );

            const glm::vec4 cell_origin = glm::vec4(
                float(2 * map_x - map_y),
                float(3 * map_y) / sqrt_3,
                0.0f,
                0.0f
            ) * m_tile_radius;

            const float r = m_tile_radius;
            const float r_sqrt_3 = r / sqrt_3;
            const float r_2_sqrt_3 = 2.0f * r / sqrt_3;

            const vertex vertices[] = {
                { cell_origin + glm::vec4(0.0f),                            color },
                { cell_origin + glm::vec4(r, -r_sqrt_3, 0.0f, 0.0f),        color },
                { cell_origin + glm::vec4(r,  r_sqrt_3, 0.0f, 0.0f),        color },
                { cell_origin + glm::vec4(0.0f, r_2_sqrt_3, 0.0f, 0.0f),    color },
                { cell_origin + glm::vec4(-r, r_sqrt_3, 0.0f, 0.0f),        color },
                { cell_origin + glm::vec4(-r, -r_sqrt_3, 0.0f, 0.0f),       color },
                { cell_origin + glm::vec4(0.0f, -r_2_sqrt_3, 0.0f, 0.0f),   color },
            };

            for (unsigned i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
                m_vertices.push_back(vertices[i]);
            }

            for (unsigned i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
                m_elements.push_back(v_pos + elements[i]);
            }
        }
    }

    fprintf(stderr, "generated %zu vertices\n", m_vertices.size());
    fprintf(stderr, "generated %zu elements\n", m_elements.size());
}

void HexMesh::gl_setup() {
    fprintf(stderr, "setting up gl objects\n");
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_elements.size() * sizeof(m_elements[0]), m_elements.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) (4 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void HexMesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_elements.size() * sizeof(m_elements[0]), GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);
}

glm::vec3 HexMesh::get_centre() const {
    const float map_half_radius = m_tile_radius * m_map_edge - 1;
    return glm::vec3(
        map_half_radius,
        sqrt_3 * map_half_radius,
        0.0f
    );
}
