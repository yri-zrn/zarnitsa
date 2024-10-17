#pragma once

#include "Common/Math.hpp"

#include <string>
#include <vector>
#include <flecs.h>

#include "Mesh.hpp"
#include "Material.hpp"

namespace zrn
{

struct GlobalTag { };

struct LocalTag { };

struct EntityTag { };

struct RootTag { };

struct LeafTag { };

struct NameComponent
{
    NameComponent() = default;
    NameComponent(const std::string& name)
        : Name(name) { }
    std::string Name = "Unnamed";
};

struct TransformComponent
{
    TransformComponent() = default;
    TransformComponent(const TransformComponent& other) = default;
    TransformComponent(const glm::vec3& translation)
        : Translation(translation) { }

    glm::vec3 Translation { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation    { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale       { 1.0f, 1.0f, 1.0f };
    glm::vec3 Skew        { 0.0f, 0.0f, 0.0f };
    
    glm::mat4 Tranfsorm() const
    {
        // glm::shearX3D();

        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        return glm::translate(glm::mat4{1.0f}, Translation)
            //  * glm::shearX3D(glm::mat4{1.0f}, Skew.x, 0.0f)
            //  * glm::shearY3D(glm::mat4{1.0f}, Skew.y, 0.0f)
            //  * glm::shearZ3D(glm::mat4{1.0f}, Skew.z, 0.0f)
             * rotation
             * glm::scale(glm::mat4{1.0f}, Scale);


        // glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        // return glm::translate(glm::mat4{1.0f}, Translation)
        //      * rotation
        //      * glm::scale(glm::mat4{1.0f}, Scale);
    }
};

struct MeshComponent
{
    Mesh Mesh;
};

struct MaterialComponent
{
    Material Material;
};

// struct ObjectComponent
// {

// };

} // namespace zrn