#pragma once

#include "Core/Platform.h"
#include "Vector.h"

namespace sb
{
    template <typename T>
    struct alignas(16) Matrix
    {
        alignas(16) T M[4][4];

        void SetIdentity();

        FORCEINLINE Matrix();
        FORCEINLINE Matrix(const Vector3d& inX, const Vector3d& inY, const Vector3d& inZ, const Vector3d& inW);

        FORCEINLINE Matrix<T> operator+(const Matrix<T>& U) const;
        FORCEINLINE Matrix<T> operator-(const Matrix<T>& U) const;
        FORCEINLINE Matrix<T> operator*(const Matrix<T>& U) const;
        FORCEINLINE Matrix<T> operator/(const Matrix<T>& U) const;
        FORCEINLINE bool operator==(const Matrix<T>& U) const;

        template<typename U>
        FORCEINLINE Matrix<T>& operator=(const Matrix<U>& in_other);
    };

    template <typename T>
    FORCEINLINE Matrix<T>::Matrix() {}

    template <typename T>
    FORCEINLINE Matrix<T>::Matrix(const Vector3d& inX, const Vector3d& inY, const Vector3d& inZ, const Vector3d& inW)
    {
        M[0][0] = inX.X; M[0][1] = inX.Y;  M[0][2] = inX.Z;  M[0][3] = 0.0;
        M[1][0] = inY.X; M[1][1] = inY.Y;  M[1][2] = inY.Z;  M[1][3] = 0.0;
        M[2][0] = inZ.X; M[2][1] = inZ.Y;  M[2][2] = inZ.Z;  M[2][3] = 0.0;
        M[3][0] = inW.X; M[3][1] = inW.Y;  M[3][2] = inW.Z;  M[3][3] = 1.0;
    }

    template <typename T>
    FORCEINLINE Matrix<T> Matrix<T>::operator+(const Matrix<T>& U) const
    {
        M[0][0] += U.M[0][0]; M[0][1] += U.M[0][1];  M[0][2] += U.M[0][2];
        M[1][0] += U.M[1][0]; M[1][1] += U.M[1][1];  M[1][2] += U.M[1][2];
        M[2][0] += U.M[2][0]; M[2][1] += U.M[2][1];  M[2][2] += U.M[2][2];
        M[3][0] += U.M[3][0]; M[3][1] += U.M[3][1];  M[3][2] += U.M[3][2];
    }

    template <typename T>
    FORCEINLINE Matrix<T> Matrix<T>::operator-(const Matrix<T>& U) const
    {
        M[0][0] -= U.M[0][0]; M[0][1] -= U.M[0][1];  M[0][2] -= U.M[0][2];
        M[1][0] -= U.M[1][0]; M[1][1] -= U.M[1][1];  M[1][2] -= U.M[1][2];
        M[2][0] -= U.M[2][0]; M[2][1] -= U.M[2][1];  M[2][2] -= U.M[2][2];
        M[3][0] -= U.M[3][0]; M[3][1] -= U.M[3][1];  M[3][2] -= U.M[3][2];
    }

    template <typename T>
    FORCEINLINE Matrix<T> Matrix<T>::operator*(const Matrix<T>& U) const
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                M[i][j] = 0;
                for (int k = 0; k < 4; k++)
                {
                    M[i][j] += M[i][k] * U[k][j];
                }
            }
        }

        return this;
    }

    template <typename T>
    FORCEINLINE Matrix<T> Matrix<T>::operator/(const Matrix<T>& U) const
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                M[i][j] = 0;
                for (int k = 0; k < 4; k++)
                {
                    M[i][j] += M[i][k] / U[k][j];
                }
            }
        }

        return this;
    }

    template <typename T>
    FORCEINLINE bool Matrix<T>::operator==(const Matrix<T>& U) const
    {
        (M[0][0] == U.M[0][0] && M[0][1] == U.M[0][1] && M[0][2] == U.M[0][2] && M[1][0] == U.M[1][0] &&
         M[1][1] == U.M[1][1] && M[1][2] == U.M[1][2] && M[2][0] == U.M[2][0] && M[2][1] == U.M[2][1] &&
         M[2][2] == U.M[2][2] && M[3][0] == U.M[3][0] && M[3][1] == U.M[3][1] && M[3][2] == U.M[3][2])
            ? true
            : false;
    }

    template <typename T>
    template <typename U>
    FORCEINLINE Matrix<T>& Matrix<T>::operator=(const Matrix<U>& in_other)
    {
        M[0][0] = U.M[0][0]; M[0][1] = U.M[0][1];  M[0][2] = U.M[0][2]; M[0][3] = U.M[0][3];
        M[1][0] = U.M[1][0]; M[1][1] = U.M[1][1];  M[1][2] = U.M[1][2]; M[1][3] = U.M[1][3];
        M[2][0] = U.M[2][0]; M[2][1] = U.M[2][1];  M[2][2] = U.M[2][2]; M[2][3] = U.M[2][3];
        M[3][0] = U.M[3][0]; M[3][1] = U.M[3][1];  M[3][2] = U.M[3][2]; M[3][3] = U.M[3][3];

        return *this;
    }

    template <typename T>
    void Matrix<T>::SetIdentity()
    {
        M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0.0;
        M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0.0;
        M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0.0;
        M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1.0;
    }
};