#include "level/level_loader.hpp"

#include "raylib.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

void parse_level_file(const std::string& filename, const ColorMap& map) {
    Image image = LoadImage(filename.c_str());
    Color* colors = LoadImageColors(image);

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            // Colors is a straight array so we have to convert to row-oriented index
            const int i = y * image.width + x;
            const Color& color = colors[i];

            const auto it = map.find(color);

            if (color.a == 0) continue;

            assert(it != map.end() && "Invalid color found");

            it->second({x,y});
        }
    }

    UnloadImageColors(colors);
    UnloadImage(image);
}

void load_level(const std::string name, const ColorMap& map) {
    const std::filesystem::path target_path{"res/levels"};

    try {
        bool file_found = false;
        for (std::filesystem::directory_entry const& dir_entry : std::filesystem::directory_iterator{target_path}) {
            if (!std::filesystem::is_regular_file(dir_entry.path())) { continue; }

            const auto& path = dir_entry.path();

            if (path.stem() != name) continue;

            file_found = true;

            parse_level_file(path.string(), map);
        }

        assert(file_found && "Level file could not be found");
    } catch (std::filesystem::filesystem_error const& ex) {
        std::cout << "Error occured during file operation!\n" << ex.what() << std::endl;
    }
}
