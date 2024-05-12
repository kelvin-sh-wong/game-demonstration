#ifndef PROGRAM_LINKING_ERROR_HPP
#define PROGRAM_LINKING_ERROR_HPP

#include <stdexcept>
#include <string>

class ProgramLinkingError : public std::runtime_error {
public:
    ProgramLinkingError(std::string const & message, std::string info_log);

    std::string const & info_log() const;

private:
    std::string info_log_;
};

#endif  // PROGRAM_LINKING_ERROR_HPP
