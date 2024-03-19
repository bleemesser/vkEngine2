#pragma once

#include "libs.hpp"

namespace ASHModel {
    class Obj {
        public:
            std::vector<float> vertices;
            std::vector<uint32_t> indices;

            std::unordered_map<std::string, uint32_t> vertexCache;
            std::unordered_map<std::string, glm::vec3> colorCache;

            glm::vec3 currentColor;


            std::vector<glm::vec3> v, vn;
            std::vector<glm::vec2> vt;
            glm::mat4 preTransform;

            Obj(const char* path, const char* mtlPath, glm::mat4 preTransform);

            void readVertex(const std::vector<std::string>& words); // v
            void readTexCoord(const std::vector<std::string>& words); // vt
            void readNormal(const std::vector<std::string>& words); // vn
            void readFace(const std::vector<std::string>& words); // f
            void readCorner(const std::string& words); // from readFace
    };
}