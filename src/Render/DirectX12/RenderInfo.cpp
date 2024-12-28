#include "RenderInfo.h"

namespace sb
{
    RenderInfo::RenderInfo()
    {
    }

    void RenderInfo::UpdateTransform(const Transform& transform)
    {
        _transform = transform;
        UpdateWorldMatrix();
    }

    void RenderInfo::UpdateWorldMatrix()
    {
        Vector3d& rot = _transform.m_rotation;
        Vector3d& pos = _transform.m_translation;

        _worldMatrix = XMMatrixRotationRollPitchYaw(rot.X, rot.Y, rot.Z) * XMMatrixTranslation(pos.X, pos.Y, pos.Z);
        XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(0.f, rot.Y, 0.f);
    }

    RenderInfo::~RenderInfo()
    {
    }

} // namespace sb