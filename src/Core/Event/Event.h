#pragma once

#include "Core/Platform.h"
#include "Core/Math/Vector.h"
#include "Core/Input.h"
#include <functional>

namespace sb
{
    enum class EventType
    {
        MouseMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseScrolled,

        ClearColorChanged,
    };

    #define EVENT_TYPE(type)  static EventType GetStaticType() { return EventType::type; } \
    EventType GetEventType() const final { return GetStaticType(); }

    struct Event
    {
        bool m_IsHandled = false;

        virtual ~Event() {}
        virtual EventType GetEventType() const = 0;
    };

    struct MouseMoveEvent : public Event
    {
        MouseMoveEvent(float in_x, float in_y) : m_mouseX(in_x), m_mouseY(in_y) {}

        EventType GetEventType() const final { return EventType::MouseMoved; }

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

        EventType GetEventType() const override { return EventType::MouseButtonPressed; }

    protected:
        MouseButton m_MouseButton;
    };

    struct MouseButtonPressedEvent : public MouseButtonEvent
    {
        MouseButtonPressedEvent(MouseButton in_mouseButton) : MouseButtonEvent(in_mouseButton) {}

        EventType GetEventType() const final { return EventType::MouseButtonPressed; }
    };

    struct MouseButtonReleasedEvent : public MouseButtonEvent
    {
        MouseButtonReleasedEvent(MouseButton in_mouseButton) : MouseButtonEvent(in_mouseButton) {}

        EventType GetEventType() const final { return EventType::MouseButtonReleased; }
    };

    struct ClearColorChangedEvent : public Event
    {
        ClearColorChangedEvent(Vector4f nVector4f) : _value(nVector4f) {}
        Vector4f GetValue() { return _value; }

        EVENT_TYPE(ClearColorChanged)

    private:
        Vector4f _value = Vector4f::zeroVector;
    };

    class EventDisaptcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    public:
        EventDisaptcher(Event& e) : _event(e) {}

        template<typename T> bool Dispatch(EventFn<T> nFn)
        {
            if (_event.GetEventType() == T::GetStaticType() && !_event.m_IsHandled)
            {
                _event.m_IsHandled = nFn(static_cast<T&>(_event));
                return true;
            }

            return false;
        }

    private:
        Event& _event;
    };
};