#include "Camera.h"
#include "Core/Input.h"
#include "Core/Math/Vector.h"
#include "spdlog/spdlog.h"

namespace sb
{
    void Camera::UpdateViewMatrix()
    {
        const XMVECTOR forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        const XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        const XMVECTOR backwardVector = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
        const XMVECTOR leftVector = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
        const XMVECTOR rightVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

        XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(_rot.x, _rot.y, _rot.z);

        XMVECTOR target = XMVector3TransformCoord(forwardVector, rotMatrix);
        //target += _posVector; // ????

        XMVECTOR upDirection = XMVector3TransformCoord(upVector, rotMatrix);
        _viewMatrix = XMMatrixLookAtLH(_posVector, target, upDirection);

        XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.f, _rot.y, 0.f);
        _forward = XMVector3TransformCoord(forwardVector, vecRotationMatrix);
        _backward = XMVector3TransformCoord(backwardVector, vecRotationMatrix);
        _left = XMVector3TransformCoord(leftVector, vecRotationMatrix);
        _right = XMVector3TransformCoord(rightVector, vecRotationMatrix);
    }

    void Camera::SetProjection(float fov, float aspectRatio, float nearZ, float farZ)
    {
        float fovRadians = (fov / 360.f) * XM_PI;
        _projMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
    }

    void Camera::SetDirection(XMFLOAT3 direction)
    {
        if (_pos.x == direction.x && _pos.y == direction.y && _pos.z == direction.z)
        {
            return;
        }

        direction.x = _pos.x - direction.x;
        direction.y = _pos.y - direction.y;
        direction.z = _pos.z - direction.z;

        float pitch = 0.f;
        if (direction.y != 0.f)
        {
            float dist = std::sqrt(direction.x * direction.x + direction.z * direction.z);
            pitch = atan(direction.y / dist);
        }

        float yaw = 0.f;
        if (direction.z > 0.f)
        {
            yaw = atan(direction.x / direction.z);
        }

        if (direction.z > 0.f)
        {
            yaw += XM_PI;
        }

        SetRotation(pitch, yaw, 0.f);
    }

    void Camera::SetPosition(const XMFLOAT3& pos)
    {
        _pos = pos;
        _posVector = XMLoadFloat3(&_pos);
        UpdateViewMatrix();
    }

    void Camera::SetPosition(float x, float y, float z)
    {
        _pos = XMFLOAT3(x, y, z);
        _posVector = XMLoadFloat3(&_pos);
        UpdateViewMatrix();
    }

    void Camera::SetRotation(const XMFLOAT3& rot)
    {
        _rot = rot;
        _forward = XMLoadFloat3(&_rot);
        UpdateViewMatrix();
    }

    void Camera::SetRotation(float x, float y, float z)
    {
        _rot.x += x;
        _rot.y += y;
        _rot.z += z;
        _forward = XMLoadFloat3(&_rot);
        UpdateViewMatrix();
    }

    void Camera::AddPosition(const XMFLOAT3& delta)
    {
        _pos.x += delta.x;
        _pos.y += delta.y;
        _pos.z += delta.z;
        _posVector = XMLoadFloat3(&_pos);
        UpdateViewMatrix();
    }

    void Camera::AddRotation(const XMFLOAT3& delta)
    {
        _rot.x += delta.x;
        _rot.y += delta.y;
        _rot.z += delta.z;
        _forward = XMLoadFloat3(&_rot);
        UpdateViewMatrix();
    }

    void Camera::OnUpdate(float in_delta)
    {
        Vector2d mousePos{Input::GetMouseX(), Input::GetMouseY()};

        if (Input::IsMouseButtonDown(MouseButton::Right))
        {
            spdlog::info("Right Pressed/held");
        }
        else
        {
            spdlog::info("Right idle");
        }

        // spdlog::info("x:{}, y:{}", mousePos.X, mousePos.Y);
    }
} // namespace sb