#ifndef FILE_LOADING_ERROR_HPP
#define FILE_LOADING_ERROR_HPP

#include <stdexcept>
#include <string>

class FileLoadingError : public std::runtime_error {
public:
    FileLoadingError(std::string const & message, std::string filename);

    std::string const & filename() const;

private:
    std::string filename_;
};

#endif  // FILE_LOADING_ERROR_HPP
