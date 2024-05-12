#include "program.hpp"

#include "program_linking_error.hpp"
#include "shader.hpp"

#include <glad/gl.h>

#include <cassert>
#include <utility>
#include <vector>

Program::Program(std::vector<Shader const *> const & shaders) {
    for (Shader const * shader : shaders) {
        assert(shader != nullptr);
    }

    name_ = glCreateProgram();

    for (Shader const * shader : shaders) {
        glAttachShader(name_, shader->name());
    }

    glLinkProgram(name_);

    for (Shader const * shader : shaders) {
        glDetachShader(name_, shader->name());
    }

    GLint link_status;
    glGetProgramiv(name_, GL_LINK_STATUS, &link_status);

    if (link_status == GL_FALSE) {
        GLchar info_log[max_info_log_size];
        glGetProgramInfoLog(name_, max_info_log_size, nullptr, info_log);

        glDeleteProgram(name_);

        throw ProgramLinkingError{"Failed to link an OpenGL program.", info_log};
    }
}

Program::Program(Program && program) noexcept : name_{std::exchange(program.name_, 0)} {
    // empty
}

Program & Program::operator=(Program && program) noexcept {
    name_ = std::exchange(program.name_, 0);

    return *this;
}

Program::~Program() noexcept {
    glDeleteProgram(name_);
}

GLuint Program::name() const {
    return name_;
}
