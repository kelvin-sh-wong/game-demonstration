#include "shape_renderer.hpp"

#include "convex_polygon.hpp"
#include "program.hpp"
#include "vector2.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

ShapeRenderer::ShapeRenderer(Program program) : program_{std::move(program)} {
    glGenBuffers(1, &vertex_buffer_name_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_name_);

    glGenVertexArrays(1, &vertex_array_name_);
    glBindVertexArray(vertex_array_name_);

    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(GLfloat), vertex_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void *>(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
}

ShapeRenderer::~ShapeRenderer() noexcept {
    glDeleteVertexArrays(1, &vertex_array_name_);
    glDeleteBuffers(1, &vertex_buffer_name_);
}

void ShapeRenderer::draw_shape(ConvexPolygon const & convex_polygon, glm::mat4 const & view, glm::mat4 const & projection, glm::vec4 const & color) const {
    std::vector<glm::vec2> vertices;

    for (Vector2 vertex : convex_polygon.vertices()) {
        vertices.push_back(glm::vec2{vertex.x, vertex.y});
    }

    draw_polygon(vertices, view, projection, color);
}

void ShapeRenderer::draw_circle(glm::vec2 const & center, float radius, glm::mat4 const & view, glm::mat4 const & projection, glm::vec4 const & color) const {
    assert(radius >= 0.0f);
    assert(radius <= std::numeric_limits<std::size_t>::max());

    std::vector<glm::vec2> vertices;

    std::size_t vertex_count{static_cast<std::size_t>(glm::max(radius * 0.5f, 16.0f))};

    for (std::size_t i{0}; i < vertex_count; ++i) {
        float angle{glm::two_pi<float>() * i / vertex_count + glm::half_pi<float>()};
        glm::vec2 vertex{center.x + radius * glm::cos(angle), center.y + radius * glm::sin(angle)};

        vertices.push_back(std::move(vertex));
    }

    draw_polygon(vertices, view, projection, color);
}

void ShapeRenderer::draw_polygon(std::vector<glm::vec2> const & vertices, glm::mat4 const & view, glm::mat4 const & projection, glm::vec4 const & color) const {
    assert(vertices.size() >= 3);

    for (std::size_t i{0}; i < vertices.size(); ++i) {
        std::size_t j{(i + 1) % vertices.size()};

        draw_line(vertices[i], vertices[j], view, projection, color);
    }
}

void ShapeRenderer::draw_line(glm::vec2 const & start, glm::vec2 const & end, glm::mat4 const & view, glm::mat4 const & projection, glm::vec4 const & color) const {
    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{start, 0.0f});
    model = glm::rotate(model, glm::atan(end.y - start.y, end.x - start.x), glm::vec3{0.0f, 0.0f, 1.0f});
    model = glm::scale(model, glm::vec3{glm::distance(start, end), 1.0f, 1.0f});

    glUseProgram(program_.name());
    glUniformMatrix4fv(glGetUniformLocation(program_.name(), "model"), 1, false, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program_.name(), "view"), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program_.name(), "projection"), 1, false, glm::value_ptr(projection));
    glUniform4fv(glGetUniformLocation(program_.name(), "color"), 1, glm::value_ptr(color));

    glBindVertexArray(vertex_array_name_);
    glDrawArrays(GL_LINES, 0, 2);
}
