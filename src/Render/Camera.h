#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "corepch.h"

namespace sb
{
    struct Camera
    {
        XMVECTOR _posVector;
        XMVECTOR _rotVector;
        XMFLOAT3 _pos = {0.f, 0.f, 0.f};
        XMFLOAT3 _rot = {0.f, 0.f, 0.f};

        XMMATRIX _viewMatrix;
        XMMATRIX _projMatrix;

        XMVECTOR _forward;
        XMVECTOR _left;
        XMVECTOR _right;
        XMVECTOR _backward;

        void UpdateViewMatrix(); // may be private
        void SetProjection(float fov, float aspectRatio, float nearZ, float farZ);

        void SetDirection(XMFLOAT3 direction);
        void SetPosition(const XMFLOAT3& pos);
        void SetPosition(float x, float y, float z);
        void SetRotation(const XMFLOAT3& rot);
        void SetRotation(float x, float y, float z);
        void AddPosition(const XMFLOAT3& delta);
        void AddRotation(const XMFLOAT3& delta);

        void OnUpdate(float in_delta); // will be deprecated
    };
};