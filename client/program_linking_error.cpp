#include "program_linking_error.hpp"

#include <stdexcept>
#include <string>
#include <utility>

ProgramLinkingError::ProgramLinkingError(std::string const & message, std::string info_log) : std::runtime_error{message}, info_log_{std::move(info_log)} {
    // empty
}

std::string const & ProgramLinkingError::info_log() const {
    return info_log_;
}
