#include "Camera.h"
#include "Core/Input.h"
#include "Core/Math/Vector.h"
#include "spdlog/spdlog.h"

namespace sb
{
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