#pragma once

#include "libs.hpp"

class Scene {
    public:
        Scene();

        std::unordered_map<meshTypes, std::vector<glm::vec3>> positions; // TODO: change to mat4 for position, rotation, scale

};