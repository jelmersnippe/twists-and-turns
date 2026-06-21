#include "level/level_loader.hpp"

#include "raylib.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

LevelFileInfo get_level_file_info(std::string_view str) {
    const auto pos3 = str.rfind('_');
    const auto pos2 = str.rfind('_', pos3 - 1);
    const auto pos1 = str.rfind('_', pos2 - 1);

    return {.index = std::stoi(str.substr(pos1 + 1, pos2 - pos1 - 1).data()),
            .chunk_size_x = std::stoi(str.substr(pos2 + 1, pos3 - pos2 - 1).data()),
            .chunk_size_y = std::stoi(str.substr(pos3 + 1).data())};
}

LevelInfo parse_level_file(const std::filesystem::path& path) {
    const char* file_string = path.c_str();
    Image image = LoadImage(file_string);
    Color* colors = LoadImageColors(image);

    std::vector<Color> level_colors;

    const auto [index, chunk_size_x, chunk_size_y] = get_level_file_info(path.stem().string());
    const Vec2 size = {.x = image.width, .y = image.height};

    assert(image.width % chunk_size_x == 0 && "Level width is not dividable by chunk size x");
    assert(image.height % chunk_size_y == 0 && "Level height is not dividable by chunk size y");

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            // Colors is a straight array so we have to convert to row-oriented index
            const int i = y * image.width + x;
            const Color& color = colors[i];

            level_colors.push_back(color);
        }
    }

    UnloadImageColors(colors);
    UnloadImage(image);

    return {.index = index, .size = size, .chunk_size = {.x = chunk_size_x, .y = chunk_size_y}, .colors = level_colors};
}

std::vector<LevelInfo> load_levels() {
    std::vector<LevelInfo> levels = {};
    const std::filesystem::path target_path{"res/levels"};

    try {
        for (std::filesystem::directory_entry const& dir_entry : std::filesystem::directory_iterator{target_path}) {
            if (!std::filesystem::is_regular_file(dir_entry.path())) { continue; }

            levels.push_back(parse_level_file(dir_entry.path().string()));
            std::cout << "Pushed level " << dir_entry.path().filename().string() << std::endl;
        }
    } catch (std::filesystem::filesystem_error const& ex) {
        std::cout << "Error occured during file operation!\n" << ex.what() << std::endl;
    }

    std::ranges::sort(levels, std::ranges::less{}, &LevelInfo::index);

    return levels;
}
