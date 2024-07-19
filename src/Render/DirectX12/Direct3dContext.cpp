#include "Direct3dContext.h"
#include "Core/Direct3dDriver.h"
#include "Render/RenderResource.h"

namespace sb
{
    Direct3dContext::Direct3dContext(Driver* in_driver) : m_direct3dDriver(static_cast<Direct3dDriver*>(in_driver))
    {
    }

    void Direct3dContext::Initialize()
    {
    }

    void Direct3dContext::SwapBuffers()
    {
    }

    void Direct3dContext::Render()
    {
    }

    void Direct3dContext::VertexProcess(SPtr<VertexRenderResource>& in_vertexResource)
    {
    }

    void Direct3dContext::StaticMeshProcess(SPtr<StaticMeshRenderResource>& in_staticMeshResource)
    {
    }

    void Direct3dContext::CreateBuffer()
    {
    }

    void Direct3dContext::BindBuffer()
    {
    }

    void Direct3dContext::AddData(const void* in_data)
    {
    }

    void Direct3dContext::CommitData()
    {
    }
} // namespace sb