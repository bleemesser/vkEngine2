#include "obj.hpp"

ASHModel::Obj::Obj(const char* path, const char* mtlPath, glm::mat4 preTransform) {
    this->preTransform = preTransform;

    std::ifstream file;
    file.open(mtlPath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(mtlPath));
    }

    #ifdef DEBUG
    std::cout << "Reading MTL file: " << mtlPath << std::endl;
    #endif
    
    std::string line;
    std::string materialName;
    std::vector<std::string> words;

    while (std::getline(file, line)) {
        words = split(line, " ");

        if (!words[0].compare("newmtl")) {
            materialName = words[1];
        }

        if (!words[0].compare("Kd")) {
            currentColor = glm::vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]));
            colorCache.insert({materialName, currentColor});
        }
    }

    file.close();

    file.open(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(path));
    }

    #ifdef DEBUG
    std::cout << "Reading OBJ file: " << path << std::endl;
    #endif

    while (std::getline(file, line)) {
        words = split(line, " ");

        if (!words[0].compare("v")) {
            readVertex(words);
        }

        if (!words[0].compare("vt")) {
            readTexCoord(words);
        }

        if (!words[0].compare("vn")) {
            readNormal(words);
        }

        if (!words[0].compare("usemtl")) {
            if (colorCache.contains(words[1])) {
                currentColor = colorCache[words[1]];
            } else {
                currentColor = glm::vec3(1.0f, 1.0f, 1.0f);
            }
        }

        if (!words[0].compare("f")) {
            readFace(words);
        }
    }

    file.close();
}

void ASHModel::Obj::readVertex(const std::vector<std::string>& words) {
    glm::vec4 newVertex = glm::vec4(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]), 1.0f);
    glm::vec3 transformedVertex = glm::vec3(preTransform * newVertex);
    v.push_back(transformedVertex);
}

void ASHModel::Obj::readTexCoord(const std::vector<std::string>& words) {
    glm::vec2 newTexCoord = glm::vec2(std::stof(words[1]), std::stof(words[2]));
    vt.push_back(newTexCoord);
}

void ASHModel::Obj::readNormal(const std::vector<std::string>& words) {
    glm::vec4 newNormal = glm::vec4(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]), 0.0f);
    glm::vec3 transformedNormal = glm::vec3(preTransform * newNormal);
    vn.push_back(transformedNormal);
}

void ASHModel::Obj::readFace(const std::vector<std::string>& words) {
    size_t triangleCount = words.size() - 3;

    for (int i = 0; i < triangleCount; ++i) {
        readCorner(words[1]);
        readCorner(words[2 + i]);
        readCorner(words[3 + i]);
    }
}

void ASHModel::Obj::readCorner(const std::string& description) {
    if (vertexCache.contains(description)) {
        indices.push_back(vertexCache[description]);
        return;
    }

    uint32_t index = static_cast<uint32_t>(vertexCache.size());
    vertexCache.insert({description, index});
    indices.push_back(index);

    std::vector<std::string> values = split(description, "/");

    glm::vec3 pos = v[std::stol(values[0]) - 1];
    vertices.push_back(pos[0]);
    vertices.push_back(pos[1]);
    vertices.push_back(pos[2]);

    vertices.push_back(currentColor[0]);
    vertices.push_back(currentColor[1]);
    vertices.push_back(currentColor[2]);

    glm::vec2 uv = glm::vec2(0.0f, 0.0f);
    if (values.size() == 3 && values[1].size() > 0) {
        uv = vt[std::stol(values[1]) - 1];
    }
    vertices.push_back(uv[0]);
    vertices.push_back(uv[1]);
}
