#pragma once

// #define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform2.hpp>

namespace Math {

inline bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale) {
    glm::mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if(glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<float>()))
        return false;

    if(glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<float>(0), glm::epsilon<float>()) ||
       glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<float>(0), glm::epsilon<float>()) ||
       glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<float>(0), glm::epsilon<float>()))
    {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<float>(0);
        LocalMatrix[3][3] = static_cast<float>(1);
    }

    for(glm::length_t i = 0; i < 4; ++i)
    for(glm::length_t j = 0; j < 4; ++j)
        LocalMatrix[i][j] /= LocalMatrix[3][3];

    // Translation
    translation = glm::vec4(LocalMatrix[3]);
    LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

    glm::vec3 Row[3], Pdum3;

    // Scale
    for(glm::length_t i = 0; i < 3; ++i)
    for(glm::length_t j = 0; j < 3; ++j)
        Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    scale.y = length(Row[1]);
    scale.z = length(Row[2]);

    Row[0] = glm::detail::scale(Row[0], static_cast<float>(1));
    Row[1] = glm::detail::scale(Row[1], static_cast<float>(1));
    Row[2] = glm::detail::scale(Row[2], static_cast<float>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
    Pdum3 = glm::cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
    if(glm::dot(Row[0], Pdum3) < 0) {
        for(glm::length_t i = 0; i < 3; i++) {
            scale[i] *= static_cast<float>(-1);
            Row[i] *= static_cast<float>(-1);
        }
    }

    // Rotation
    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0) {
        rotation.x = atan2(Row[1][2], Row[2][2]);
        rotation.z = atan2(Row[0][1], Row[0][0]);
    } else {
        rotation.x = atan2(-Row[2][0], Row[1][1]);
        rotation.z = 0;
    }
    return true;
}

} // namespace Math