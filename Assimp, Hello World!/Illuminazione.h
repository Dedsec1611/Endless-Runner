#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_m.h"

class SpotlightController {
public:
    struct Params {
        glm::vec3 position{ 0.0f, 1.2f,  6.0f };   
        glm::vec3 direction{ 0.0f, 0.0f, -1.0f };   
        float innerDeg = 22.0f;                   
        float outerDeg = 36.0f;                    
        glm::vec3 ambient{ 0.04f, 0.04f, 0.04f };
        glm::vec3 diffuse{ 0.85f, 0.85f, 0.85f };
        glm::vec3 specular{ 1.00f, 1.00f, 1.00f };
        float constant = 1.0f;
        float linear = 0.045f;
        float quadratic = 0.0075f;
        float strength = 1.0f;
    }p ;

   
    void setBehindPlayer(const glm::vec3& playerPos,
        const glm::vec3& forwardDir,
        float backOffset = 6.0f,
        float height = 1.2f)
    {
        p.position = playerPos + glm::vec3(0.0f, height, +backOffset);
        p.direction = glm::normalize(forwardDir);
    }

    void applyTo(Shader& s) const {
        s.use();
        s.setVec3("spotlight.position", p.position);
        s.setVec3("spotlight.direction", p.direction);
        s.setFloat("spotlight.cutOff", cosDeg(p.innerDeg));
        s.setFloat("spotlight.outerCutOff", cosDeg(p.outerDeg));
        s.setVec3("spotlight.ambient", p.ambient);
        s.setVec3("spotlight.diffuse", p.diffuse);
        s.setVec3("spotlight.specular", p.specular);
        s.setFloat("spotlight.constant", p.constant);
        s.setFloat("spotlight.linear", p.linear);
        s.setFloat("spotlight.quadratic", p.quadratic);

        s.setFloat("spotStrength", p.strength);
    }

private:
    static float cosDeg(float deg) {
        return glm::cos(glm::radians(deg));
    }
};
