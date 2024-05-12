#include "resource_manager.hpp"

#include "file_loading_error.hpp"

#include <cassert>
#include <cstddef>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace {

std::vector<std::byte> load_file_contents(std::string const & filename) {
    std::vector<std::byte> contents;

    std::ifstream ifstream;
    ifstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        ifstream.open(filename, std::ifstream::binary);

        ifstream.seekg(0, std::ifstream::end);
        contents.resize(ifstream.tellg());

        ifstream.seekg(0, std::ifstream::beg);
        ifstream.read(reinterpret_cast<char *>(contents.data()), contents.size());

        ifstream.close();
    }
    catch (std::ifstream::failure const &) {
        throw FileLoadingError{"Failed to load file contents.", filename};
    }

    return contents;
}

}

ResourceManager::ResourceManager() {
    // empty
}

void ResourceManager::load_file(std::string name, std::string const & filename) {
    assert(files_.find(name) == files_.end());

    files_.insert(std::make_pair(std::move(name), load_file_contents(filename)));
}

std::vector<std::byte> const & ResourceManager::get_file(std::string const & name) const {
    assert(files_.find(name) != files_.end());

    return files_.at(name);
}
