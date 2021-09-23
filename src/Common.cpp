#include "Common.h"

std::string ReadFile(const std::string& filename)
{
    std::ifstream file("../data/" + filename);

    if (file.fail())
        std::cerr << "Could not open " << filename << std::endl;

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> ReadLines(const std::string& filename)
{
    std::ifstream file("../data/" + filename);

    if (file.fail())
        std::cerr << "Could not open " << filename << std::endl;

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line))
        lines.emplace_back(line);

    return lines;
}