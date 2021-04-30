#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include "Hittable.h"
#include <math.h>

using namespace glm;

class Sphere : public Hittable{

public: // it would be better to have some kind of protection on members...

	vec3 pos;
    float radius;
    vec3 color;
    
	// material
	float ambient;
    float diffuse;
	float phong;
    
public:
    Sphere(){};
    ~Sphere(){};

    bool Intersect(HitRecord& record, ray& ray, float tMin, float tMax) override {
        glm::vec3 oc = ray.origin() - pos;
        auto a = dot(ray.direction(), ray.direction());
        auto b = 2.0 * glm::dot(oc, ray.direction());
        auto c = dot(oc, oc) - radius * radius;
        auto discriminant = b * b - 4 * a * c;
        
        if (discriminant > 0)
        {
            float temp = (-b - std::sqrt(discriminant)) / 2;
            if (temp > tMin && temp < tMax)
            {
                record.t = temp;
                record.hitPoint = ray.at(record.t);
                record.hitNormal = (record.hitPoint - pos);
                record.hitNormal = glm::normalize(record.hitNormal);
                return true;
            }

            temp = (-b + std::sqrt(discriminant)) / 2;
            if (temp > tMin && temp < tMax)
            {
                record.t = temp;
                record.hitPoint = ray.at(record.t);
                record.hitNormal = (record.hitPoint - pos);
                record.hitNormal = glm::normalize(record.hitNormal);
                return true;
            }

        }

        return false;
    }
    
    void Draw()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glColor3f(color.x,color.y, color.z);
        glutWireSphere(radius, 10, 10);
        
        glPopMatrix();
    };
};

#endif