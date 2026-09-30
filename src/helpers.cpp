#include "helpers.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

const std::vector<char> read_spirv_file(const std::string& filename)
{
    const auto abs_file_path = std::filesystem::read_symlink("/proc/self/exe").parent_path().concat("/shaders/" + filename).string();

    std::ifstream file(
        abs_file_path,
        std::ios::ate | std::ios::binary
    );

    if (!file.is_open()) {
        throw std::runtime_error("failed to open " + filename);
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);
    file.close();

    return buffer;
}
