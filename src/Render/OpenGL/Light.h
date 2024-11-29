#pragma once

#include <glm/glm.hpp>

namespace sb
{
    struct Light
    {
        // TODO : 타입이 Opengl/dx12 둘 다 받을 수 있는걸로 해야함.
        // 지금은 우선 opengl 타입을 이용.
        glm::vec3 m_direction = glm::vec3(-1.0f, -1.0f, -1.0f);
        glm::vec2 m_cutoff = glm::vec2(20.0f, 5.f);
        glm::vec3 m_position = glm::vec3(2.0f, 2.0f, 2.0f);
        glm::vec3 m_ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 m_diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::vec3 m_specular = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_distance = 32.f;
    };
} // namespace sb