#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

class ResourceManager {
public:
    ResourceManager();

    void load_file(std::string name, std::string const & filename);
    std::vector<std::byte> const & get_file(std::string const & name) const;

private:
    std::unordered_map<std::string, std::vector<std::byte>> files_;
};

#endif  // RESOURCE_MANAGER_HPP
