#pragma once

#include "Core/Platform.h"

namespace sb
{
    // Vector2D 따로 있는게 좋을 듯.
    // 우선 이걸로 다 쓰고 나중에 바꾸기
    template <typename T>
    struct Vector
    {
        struct
        {
            T X;
            T Y;
            T Z;
        };

        static const Vector<T> zeroVector;
        static const Vector<T> oneVector;

        FORCEINLINE Vector();
        explicit FORCEINLINE Vector(T in_val);
        FORCEINLINE Vector(T in_x, T in_y, T in_z);

        FORCEINLINE Vector<T> operator+(const Vector<T>& U) const;
        FORCEINLINE Vector<T> operator-(const Vector<T>& U) const;
        FORCEINLINE Vector<T> operator*(const Vector<T>& U) const;
        FORCEINLINE Vector<T> operator/(const Vector<T>& U) const;
        FORCEINLINE bool operator==(const Vector<T>& U) const;

        template<typename U>
        FORCEINLINE Vector<T>& operator=(const Vector<U>& in_other);
    };

    template <typename T>
    FORCEINLINE Vector<T>::Vector() {}

    template <typename T>
    FORCEINLINE Vector<T>::Vector(T in_val) : X(in_val), Y(in_val), Z(in_val) {}

    template <typename T>
    FORCEINLINE Vector<T>::Vector(T in_x, T in_y, T in_z) : X(in_x), Y(in_y), Z(in_z) {}

    template <typename T>
    FORCEINLINE Vector<T> Vector<T>::operator+(const Vector<T>& U) const
    {
        return TVector<T>(X + U.X, Y + U.Y, Z + U.Z);
    }

    template <typename T>
    FORCEINLINE Vector<T> Vector<T>::operator-(const Vector<T>& U) const
    {
        return TVector<T>(X - U.X, Y - U.Y, Z - U.Z);
    }

    template <typename T>
    FORCEINLINE Vector<T> Vector<T>::operator*(const Vector<T>& U) const
    {
        return TVector<T>(X * U.X, Y * U.Y, Z * U.Z);
    }

    template <typename T>
    FORCEINLINE Vector<T> Vector<T>::operator/(const Vector<T>& U) const
    {
        return TVector<T>(X / U.X, Y / U.Y, Z / U.Z);
    }

    template <typename T>
    FORCEINLINE bool Vector<T>::operator==(const Vector<T>& U) const
    {
        return X == U.X && Y == U.Y && Z == U.Z;
    }

    template <typename T>
    template <typename U>
    FORCEINLINE Vector<T>& Vector<T>::operator=(const Vector<U>& in_other)
    {
        X = static_cast<T>(in_other.X);
        Y = static_cast<T>(in_other.Y);
        Z = static_cast<T>(in_other.Z);

        return *this;
    }

    template<> const Vector<float> Vector<float>::zeroVector(0.f, 0.f, 0.f);
    template<> const Vector<float> Vector<float>::oneVector(1.f, 1.f, 1.f);
    template<> const Vector<double> Vector<double>::zeroVector(0.0, 0.0, 0.0);
    template<> const Vector<double> Vector<double>::oneVector(1.0, 1.0, 1.0);

    using Vector3f = Vector<float>;
    using Vector3d = Vector<double>;
}
