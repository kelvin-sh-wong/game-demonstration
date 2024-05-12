#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "shader.hpp"

#include <glad/gl.h>

#include <vector>

class Program {
public:
    Program(std::vector<Shader const *> const & shaders);
    Program(Program const &) = delete;
    Program(Program && program) noexcept;
    Program & operator=(Program const &) = delete;
    Program & operator=(Program && program) noexcept;
    ~Program() noexcept;

    GLuint name() const;

private:
    static constexpr GLsizei max_info_log_size{1024};

    GLuint name_;
};

#endif  // PROGRAM_HPP
