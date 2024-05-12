#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/gl.h>

#include <cstddef>
#include <vector>

class Shader {
public:
    Shader(GLenum type, std::vector<std::byte> const & source);
    Shader(Shader const &) = delete;
    Shader(Shader && shader) noexcept;
    Shader & operator=(Shader const &) = delete;
    Shader & operator=(Shader && shader) noexcept;
    ~Shader() noexcept;

    GLuint name() const;

private:
    static constexpr GLsizei max_info_log_size{1024};

    GLuint name_;
};

#endif  // SHADER_HPP
