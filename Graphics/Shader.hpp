#pragma once

#include "Internal/GraphicsDevice.hpp"

#include <vector>

namespace zrn
{

class GraphicsContext;

class Shader
{
public:
    Shader() = default;

    Status Create(GraphicsContext* context, const std::vector<char>& vertex_shader_source, const std::vector<char>& fragment_shader_source);
    Status Create(GraphicsContext* context, const std::vector<uint32_t>& vertex_shader_source, const std::vector<uint32_t>& fragment_shader_source);

    void Bind();

private:
    GraphicsContext* m_Context = nullptr;

    Internal::ShaderProgram m_Program;
    Internal::Shader m_VertexShader;
    Internal::Shader m_FragmentShader;
};

} // namespace zrn