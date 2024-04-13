#pragma once

#include "Core/Platform.h"

namespace sb {
    template <typename T>
    struct Vector
    {
        struct
        {
            T X;
            T Y;
            T Z;
        };

        explicit FORCEINLINE Vector(T in_val);
        FORCEINLINE Vector(T in_x, T in_y, T in_z);

        FORCEINLINE Vector<T> operator+(const Vector<T>& U) const;
        FORCEINLINE Vector<T> operator-(const Vector<T>& U) const;
        FORCEINLINE Vector<T> operator*(const Vector<T>& U) const;
        FORCEINLINE Vector<T> operator/(const Vector<T>& U) const;
        FORCEINLINE bool operator==(const Vector<T>& U) const;
    };

    template <typename T>
    FORCEINLINE Vector<T>::Vector(T in_val) : X(in_val), Y(in_val), Z(in_val) {}

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
}