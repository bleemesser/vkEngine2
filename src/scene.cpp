#include "scene.hpp"

Scene::Scene() {
	positions.insert({ meshTypes::VOXEL, {} });
	positions.insert({ meshTypes::GROUND, {} });
	positions.insert({ meshTypes::SKULL, {} });
	positions[meshTypes::VOXEL].push_back(glm::vec3(5.f, 0.0f, 0.0f));
	positions[meshTypes::VOXEL].push_back(glm::vec3(0.0f, 5.0f, 0.0f));
	positions[meshTypes::VOXEL].push_back(glm::vec3(0.0f, 0.0f, 5.0f));
	// positions[meshTypes::GROUND].push_back(glm::vec3(10.0f, 0.0f, 0.0f));
	// positions[meshTypes::SKULL].push_back(glm::vec3(15.0f, -5.0f, 1.0f));
	// positions[meshTypes::SKULL].push_back(glm::vec3(15.0f, 5.0f, 1.0f));

};