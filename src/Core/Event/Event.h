#pragma once

#include "Core/Platform.h"
#include "Core/Inputs.h"

namespace sb
{
    enum class EventType
    {
        MouseMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseScrolled,
    };

    struct Event
    {
        bool m_IsHandled = false;

        virtual ~Event() {}
        virtual EventType GetEventType() const = 0;
    };

    struct MouseMoveEvent : public Event
    {
        MouseMoveEvent(float in_x, float in_y) : m_mouseX(in_x), m_mouseY(in_y) {}

        EventType GetEventType() { return EventType::MouseMoved; }

        FORCEINLINE float GetX() const { return m_mouseX; }
        FORCEINLINE float GetY() const { return m_mouseY; }

    private:
        float m_mouseX;
        float m_mouseY;
    };

    struct MouseButtonEvent : public Event
    {
        MouseButtonEvent(MouseButton in_mouseButton) : m_MouseButton(in_mouseButton) {}

        FORCEINLINE MouseButton GetMouseButton() const { return m_MouseButton; }

    protected:
        MouseButton m_MouseButton;
    };

    struct MouseButtonPressedEvent : public MouseButtonEvent
    {
        MouseButtonPressedEvent(MouseButton in_mouseButton) : MouseButtonEvent(in_mouseButton) {}

        EventType GetEventType() { return EventType::MouseButtonPressed; }
    };

    struct MouseButtonReleasedEvent : public MouseButtonEvent
    {
        MouseButtonReleasedEvent(MouseButton in_mouseButton) : MouseButtonEvent(in_mouseButton) {}

        EventType GetEventType() { return EventType::MouseButtonReleased; }
    };
};