#ifndef SHADER_COMPILATION_ERROR_HPP
#define SHADER_COMPILATION_ERROR_HPP

#include <stdexcept>
#include <string>

class ShaderCompilationError : public std::runtime_error {
public:
    ShaderCompilationError(std::string const & message, std::string info_log);

    std::string const & info_log() const;

private:
    std::string info_log_;
};

#endif  // SHADER_COMPILATION_ERROR_HPP
