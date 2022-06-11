/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <initializer_list>

template<int N, int M, typename T = float>
class Matrix
{
public:
    Matrix() { setIdentity(); }
    Matrix(int) {} // construct without initializing identity matrix
    Matrix(const Matrix<N, M, T>& other) = default;
    template<typename U>
    Matrix(const std::initializer_list<U>& values) { *this = values; }
    template<typename U>
    Matrix(const U* values) { *this = values; }

    void setIdentity();
    bool isIdentity() const;
    void fill(T value);

    size_t hash() const { return m_hash; }

    Matrix<M, N, T> transposed() const;
    std::enable_if_t<N == M> transpose() { *this = transposed(); }

    T* data() { return m[0]; }
    const T* data() const { return m[0]; }

    T& operator()(int row, int column) { return m[row - 1][column - 1]; }
    T operator()(int row, int column) const { return m[row - 1][column - 1]; }

    Matrix<N, M, T>& operator=(const Matrix<N, M, T>& other) = default;
    template<typename U>
    Matrix<N, M, T>& operator=(const std::initializer_list<U>& values);
    template<typename U>
    Matrix<N, M, T>& operator=(const U* values);
    Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& other);
    Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& other);
    Matrix<N, M, T>& operator*=(T factor);
    Matrix<N, M, T>& operator/=(T divisor);
    bool operator==(const Matrix<N, M, T>& other) const;
    bool operator!=(const Matrix<N, M, T>& other) const;

protected:
    Matrix<N, M, T>& generateHash()
    {
        m_hash = 0;
        for (int i = -1; ++i < N;)
            for (int j = -1; ++j < M;)
                stdext::hash_combine(m_hash, m[i][j]);
        return *this;
    }

private:
    T m[N][M];
    size_t m_hash{ 0 };
};

template<int N, int M, typename T>
void Matrix<N, M, T>::setIdentity()
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (i == j)
                m[i][j] = 1.0f;
            else
                m[i][j] = 0.0f;
        }
    }
    generateHash();
}

template<int N, int M, typename T>
bool Matrix<N, M, T>::isIdentity() const
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            if ((i == j && m[i][j] != 1.0f) || (i != j && m[i][j] != 0.0f))
                return false;
    return true;
}

template<int N, int M, typename T>
void Matrix<N, M, T>::fill(T value)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            m[i][j] = value;
    generateHash();
}

template<int N, int M, typename T>
Matrix<M, N, T> Matrix<N, M, T>::transposed() const
{
    Matrix<M, N, T> result(1);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            result.m[j][i] = m[i][j];
    return result.generateHash();
}

template<int N, int M, typename T>
template<typename U>
Matrix<N, M, T>& Matrix<N, M, T>::operator=(const std::initializer_list<U>& values)
{
    auto it = values.begin();
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            m[i][j] = *(it++);
    return generateHash();
}

template<int N, int M, typename T>
template<typename U>
Matrix<N, M, T>& Matrix<N, M, T>::operator=(const U* values)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            m[i][j] = values[i * N + j];
    return generateHash();
}

template<int N, int M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator+=(const Matrix<N, M, T>& other)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            m[i][j] += other.m[i][j];
    return generateHash();
}

template<int N, int M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator-=(const Matrix<N, M, T>& other)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            m[i][j] -= other.m[i][j];
    return generateHash();
}

template<int N, int M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator*=(T factor)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            m[i][j] *= factor;
    return generateHash();
}

template<int N, int M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator/=(T divisor)
{
    m_hash = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            m[i][j] /= divisor;
    return generateHash();
}

template<int N, int M, typename T>
bool Matrix<N, M, T>::operator==(const Matrix<N, M, T>& other) const
{
    return hash() == other.hash();
}

template<int N, int M, typename T>
bool Matrix<N, M, T>::operator!=(const Matrix<N, M, T>& other) const
{
    return !(*this == other);
}

template<int N, int M, typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<N, M, T>& mat)
{
    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= M; ++j) {
            out << mat(i, j);
            if (j != M)
                out << " ";
        }
        out << "\n";
    }
    return out;
}

template<int N, int M, typename T>
std::istream& operator>>(std::istream& in, Matrix<N, M, T>& mat)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            in >> mat(i, j);
    return in;
}

template<int M, int N, int P, int Q, typename T>
Matrix<M, Q, T> operator*(const Matrix<M, N, T>& a, const Matrix<P, Q, T>& b)
{
    static_assert(N == P, "N==P");
    Matrix<M, Q, T> c(1);
    for (int i = 1; i <= M; ++i) {
        for (int j = 1; j <= Q; ++j) {
            T sum = 0;
            for (int k = 1; k <= N; ++k)
                sum += a(i, k) * b(k, j);
            c(i, j) = sum;
        }
    }
    return c;
}

template<int M, int N, typename T>
Matrix<M, N, T> operator+(const Matrix<M, N, T>& a, const Matrix<M, N, T>& b) { Matrix<M, N, T> c(a); c += b; return c.generateHash(); }

template<int M, int N, typename T>
Matrix<M, N, T> operator-(const Matrix<M, N, T>& a, const Matrix<M, N, T>& b) { Matrix<M, N, T> c(a); c -= b; return c.generateHash(); }

template<int M, int N, typename T>
Matrix<M, N, T> operator*(const Matrix<M, N, T>& a, float b) { Matrix<M, N, T> c(a); c *= b; return c.generateHash(); }

template<int M, int N, typename T>
Matrix<M, N, T> operator/(const Matrix<M, N, T>& a, float b) { Matrix<M, N, T> c = a; c /= b; return c.generateHash(); }

using Matrix4x4 = Matrix<4, 4>;
using Matrix3x3 = Matrix<3, 3>;
using Matrix2x2 = Matrix<2, 2>;

using Matrix4 = Matrix4x4;
using Matrix3 = Matrix3x3;
using Matrix2 = Matrix2x2;

const static Matrix4 DEFAULT_MATRIX_4;
const static Matrix3 DEFAULT_MATRIX_3;
const static Matrix2 DEFAULT_MATRIX_2;
