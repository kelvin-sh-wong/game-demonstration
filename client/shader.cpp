#include "shader.hpp"

#include "shader_compilation_error.hpp"

#include <glad/gl.h>

#include <cassert>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace {

bool is_shader_type(GLenum value) {
    switch (value) {
    case GL_VERTEX_SHADER:
        return true;
    case GL_FRAGMENT_SHADER:
        return true;
    default:
        return false;
    }
}

}

Shader::Shader(GLenum type, std::vector<std::byte> const & source) {
    assert(is_shader_type(type));
    assert(source.size() <= std::numeric_limits<GLint>::max());

    GLchar const * source_data{reinterpret_cast<GLchar const *>(source.data())};
    GLint source_size{static_cast<GLint>(source.size())};

    name_ = glCreateShader(type);

    glShaderSource(name_, 1, &source_data, &source_size);
    glCompileShader(name_);

    GLint compile_status;
    glGetShaderiv(name_, GL_COMPILE_STATUS, &compile_status);

    if (compile_status == GL_FALSE) {
        GLchar info_log[max_info_log_size];
        glGetShaderInfoLog(name_, max_info_log_size, nullptr, info_log);

        glDeleteShader(name_);

        throw ShaderCompilationError{"Failed to compile an OpenGL shader.", info_log};
    }
}

Shader::Shader(Shader && shader) noexcept : name_{std::exchange(shader.name_, 0)} {
    // empty
}

Shader & Shader::operator=(Shader && shader) noexcept {
    name_ = std::exchange(shader.name_, 0);

    return *this;
}

Shader::~Shader() noexcept {
    glDeleteShader(name_);
}

GLuint Shader::name() const {
    return name_;
}
