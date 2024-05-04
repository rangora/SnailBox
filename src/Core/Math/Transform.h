#pragma once

#include "Core/Platform.h"
#include "Vector.h"

namespace sb
{
    template <typename T>
    struct AlignTransform
    {
        T m_translation = T(0.0, 0.0, 0.0);
        T m_rotation = T(0.0, 0.0, 0.0);
        T m_scale = T(1.0, 1.0, 1.0);

        FORCEINLINE AlignTransform();
        FORCEINLINE AlignTransform(T in_translation);
        FORCEINLINE AlignTransform(T in_translation, T in_rotation);
        FORCEINLINE AlignTransform(T in_translation, T in_rotation, T in_scale);

        FORCEINLINE AlignTransform<T> operator+(const AlignTransform<T>& U) const;
        FORCEINLINE AlignTransform<T> operator-(const AlignTransform<T>& U) const;
        FORCEINLINE AlignTransform<T> operator*(const AlignTransform<T>& U) const;
        FORCEINLINE AlignTransform<T> operator/(const AlignTransform<T>& U) const;
        FORCEINLINE bool operator==(const AlignTransform<T>& U) const;

        template<typename U>
        FORCEINLINE AlignTransform<T>& operator=(const AlignTransform<U>& in_other);
    };

    template <typename T>
    FORCEINLINE AlignTransform<T>::AlignTransform() {}

    template <typename T>
    FORCEINLINE AlignTransform<T>::AlignTransform(T in_translation) : m_translation(in_translation) {}

    template <typename T>
    FORCEINLINE AlignTransform<T>::AlignTransform(T in_translation, T in_rotation) : m_translation(in_translation), m_rotation(in_rotation) {}

    template <typename T>
    FORCEINLINE AlignTransform<T>::AlignTransform(T in_translation, T in_rotation, T in_scale)
        : m_translation(in_translation), m_rotation(in_rotation), m_scale(in_scale) {}

    template <typename T>
    FORCEINLINE AlignTransform<T> AlignTransform<T>::operator+(const AlignTransform<T>& U) const
    {
        return AlignTransform<T>(m_translation + U.m_translation, m_rotation + U.m_rotation, m_scale + U.m_scale);
    }

    template <typename T>
    FORCEINLINE AlignTransform<T> AlignTransform<T>::operator-(const AlignTransform<T>& U) const
    {
        return AlignTransform<T>(m_translation - U.m_translation, m_rotation - U.m_rotation, m_scale - U.m_scale);
    }

    template <typename T>
    FORCEINLINE AlignTransform<T> AlignTransform<T>::operator*(const AlignTransform<T>& U) const
    {
        return AlignTransform<T>(m_translation * U.m_translation, m_rotation * U.m_rotation, m_scale * U.m_scale);
    }

    template <typename T>
    FORCEINLINE AlignTransform<T> AlignTransform<T>::operator/(const AlignTransform<T>& U) const
    {
        return AlignTransform<T>(m_translation / U.m_translation, m_rotation / U.m_rotation, m_scale / U.m_scale);
    }

    template <typename T>
    FORCEINLINE bool AlignTransform<T>::operator==(const AlignTransform<T>& U) const
    {
        return m_translation == U.m_translation && m_rotation == U.m_rotation && m_scale == U.m_scale;
    }

    template <typename T>
    template <typename U>
    FORCEINLINE AlignTransform<T>& AlignTransform<T>::operator=(const AlignTransform<U>& in_other)
    {
        m_translation = static_cast<T>(in_other.m_translation);
        m_rotation = static_cast<T>(in_other.m_rotation);
        m_scale = static_cast<T>(in_other.m_scale);

        return *this;
    }

    using Transform = AlignTransform<Vector3d>;
};