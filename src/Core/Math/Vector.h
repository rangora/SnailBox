#pragma once

#include "Core/Platform.h"
#include "Quat.h"
#include <cstdlib>

namespace sb
{
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

        Quat ToQuat() const;
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
        return Vector<T>(X + U.X, Y + U.Y, Z + U.Z);
    }

    template <typename T>
    FORCEINLINE Vector<T> Vector<T>::operator-(const Vector<T>& U) const
    {
        return Vector<T>(X - U.X, Y - U.Y, Z - U.Z);
    }

    template <typename T>
    FORCEINLINE Vector<T> Vector<T>::operator*(const Vector<T>& U) const
    {
        return Vector<T>(X * U.X, Y * U.Y, Z * U.Z);
    }

    template <typename T>
    FORCEINLINE Vector<T> Vector<T>::operator/(const Vector<T>& U) const
    {
        return Vector<T>(X / U.X, Y / U.Y, Z / U.Z);
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

    template <typename T>
    Quat Vector<T>::ToQuat() const
    {
        Quat ReturnQuat;

        const double DEG_TO_RAD = 3.14 / (180.0);
        const double RADS_DIVIDED_BY_2 = DEG_TO_RAD / 2.0;
        double SP, SY, SR;
        double CP, CY, CR;

        const double RollNoWinding = std::fmod(X, 360.0);
        const double PitchNoWinding = std::fmod(Y, 360.0);
        const double YawNoWinding = std::fmod(Z, 360.0);

        SP = std::sin(PitchNoWinding * RADS_DIVIDED_BY_2);
        CP = std::cos(PitchNoWinding * RADS_DIVIDED_BY_2);
        SY = std::sin(YawNoWinding * RADS_DIVIDED_BY_2);
        CY = std::cos(YawNoWinding * RADS_DIVIDED_BY_2);
        SR = std::sin(RollNoWinding * RADS_DIVIDED_BY_2);
        CR = std::cos(RollNoWinding * RADS_DIVIDED_BY_2);

        ReturnQuat.X = CR * SP * SY - SR * CP * CY;
        ReturnQuat.Y = -CR * SP * CY - SR * CP * SY;
        ReturnQuat.Z = CR * CP * SY - SR * SP * CY;
        ReturnQuat.W = CR * CP * CY + SR * SP * SY;

        return ReturnQuat;
    }

    template<> const Vector<float> Vector<float>::zeroVector(0.f, 0.f, 0.f);
    template<> const Vector<float> Vector<float>::oneVector(1.f, 1.f, 1.f);
    template<> const Vector<double> Vector<double>::zeroVector(0.0, 0.0, 0.0);
    template<> const Vector<double> Vector<double>::oneVector(1.0, 1.0, 1.0);

    using Vector3f = Vector<float>;
    using Vector3d = Vector<double>;

    template <typename T>
    struct Vector2
    {
        struct
        {
            T X;
            T Y;
        };

        static const Vector2<T> zeroVector;
        static const Vector2<T> oneVector;

        FORCEINLINE Vector2();
        explicit FORCEINLINE Vector2(T in_val);
        FORCEINLINE Vector2(T in_x, T in_y);

        FORCEINLINE Vector2<T> operator+(const Vector2<T>& U) const;
        FORCEINLINE Vector2<T> operator-(const Vector2<T>& U) const;
        FORCEINLINE Vector2<T> operator*(const Vector2<T>& U) const;
        FORCEINLINE Vector2<T> operator/(const Vector2<T>& U) const;
        FORCEINLINE bool operator==(const Vector2<T>& U) const;

        template <typename U> FORCEINLINE Vector2<T>& operator=(const Vector2<U>& in_other);
    };

    template <typename T> FORCEINLINE Vector2<T>::Vector2()
    {
    }

    template <typename T> FORCEINLINE Vector2<T>::Vector2(T in_val) : X(in_val), Y(in_val)
    {
    }

    template <typename T> FORCEINLINE Vector2<T>::Vector2(T in_x, T in_y) : X(in_x), Y(in_y)
    {
    }

    template <typename T> FORCEINLINE Vector2<T> Vector2<T>::operator+(const Vector2<T>& U) const
    {
        return Vector<T>(X + U.X, Y + U.Y);
    }

    template <typename T> FORCEINLINE Vector2<T> Vector2<T>::operator-(const Vector2<T>& U) const
    {
        return Vector<T>(X - U.X, Y - U.Y);
    }

    template <typename T> FORCEINLINE Vector2<T> Vector2<T>::operator*(const Vector2<T>& U) const
    {
        return Vector<T>(X * U.X, Y * U.Y);
    }

    template <typename T> FORCEINLINE Vector2<T> Vector2<T>::operator/(const Vector2<T>& U) const
    {
        return Vector<T>(X / U.X, Y / U.Y);
    }

    template <typename T> FORCEINLINE bool Vector2<T>::operator==(const Vector2<T>& U) const
    {
        return X == U.X && Y == U.Y;
    }

    template <typename T> template <typename U> FORCEINLINE Vector2<T>& Vector2<T>::operator=(const Vector2<U>& in_other)
    {
        X = static_cast<T>(in_other.X);
        Y = static_cast<T>(in_other.Y);

        return *this;
    }

    template <> const Vector2<float> Vector2<float>::zeroVector(0.f, 0.f);
    template <> const Vector2<float> Vector2<float>::oneVector(1.f, 1.f);
    template <> const Vector2<double> Vector2<double>::zeroVector(0.0, 0.0);
    template <> const Vector2<double> Vector2<double>::oneVector(1.0, 1.0);

    using Vector2f = Vector2<float>;
    using Vector2d = Vector2<double>;
}
