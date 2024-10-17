#include "GraphicsContext.hpp"

#include "GraphicsDevice.hpp"

#include <assert.h>

namespace zrn
{

GraphicsDevice* GraphicsContext::GetDevice()
{
    return m_Device;
}

Internal::GraphicsDevice* GraphicsContext::GetInternalDevice()
{
    return &m_Device->m_Device;
}

} // namespace zrn