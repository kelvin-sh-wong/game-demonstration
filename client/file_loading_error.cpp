#include "file_loading_error.hpp"

#include <stdexcept>
#include <string>
#include <utility>

FileLoadingError::FileLoadingError(std::string const & message, std::string filename) : std::runtime_error{message}, filename_{std::move(filename)} {
    // empty
}

std::string const & FileLoadingError::filename() const {
    return filename_;
}
