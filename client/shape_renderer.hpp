#ifndef SHAPE_RENDERER_HPP
#define SHAPE_RENDERER_HPP

#include "convex_polygon.hpp"
#include "program.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>

class ShapeRenderer {
public:
    ShapeRenderer(Program program);
    ShapeRenderer(ShapeRenderer const &) = delete;
    ShapeRenderer & operator=(ShapeRenderer const &) = delete;
    ~ShapeRenderer() noexcept;

    void draw_shape(ConvexPolygon const & convex_polygon, glm::mat4 const & view, glm::mat4 const & projection, glm::vec4 const & color) const;

    void draw_circle(glm::vec2 const & center, float radius, glm::mat4 const & view, glm::mat4 const & projection, glm::vec4 const & color) const;
    void draw_polygon(std::vector<glm::vec2> const & vertices, glm::mat4 const & view, glm::mat4 const & projection, glm::vec4 const & color) const;

private:
    static constexpr std::array<GLfloat, 4> vertex_data{0.0f, 0.0f, 1.0f, 0.0f};

    void draw_line(glm::vec2 const & start, glm::vec2 const & end, glm::mat4 const & view, glm::mat4 const & projection, glm::vec4 const & color) const;

    Program program_;

    GLuint vertex_buffer_name_;
    GLuint vertex_array_name_;
};

#endif  // SHAPE_RENDERER_HPP
