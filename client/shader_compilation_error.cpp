#include "shader_compilation_error.hpp"

#include <stdexcept>
#include <string>
#include <utility>

ShaderCompilationError::ShaderCompilationError(std::string const & message, std::string info_log) : std::runtime_error{message}, info_log_{std::move(info_log)} {
    // empty
}

std::string const & ShaderCompilationError::info_log() const {
    return info_log_;
}
