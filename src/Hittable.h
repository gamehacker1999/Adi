#pragma once
#include <glm/glm.hpp>
#include "Ray.h"

struct HitRecord {
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;
	glm::vec3 color;
	float t;
};

class Hittable {
public:
	 Hittable() {};
	 virtual bool Intersect(HitRecord& record,  ray& ray, float tMin, float tMax) = 0;
};