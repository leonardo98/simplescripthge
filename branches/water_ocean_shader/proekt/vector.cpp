#include <stdlib.h>
#include <assert.h>
#include "vector.h"

inline bool TEqual(float a, float b, float t)
{
    return ((a > b - t) && (a < b + t));
}

// �������� ������������
CVector& CVector::operator =(const CVector &vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
        return *this;
}

// �������� ��������
CVector CVector::operator +(const CVector &vec) const
{
    return CVector(x + vec.x, y + vec.y, z + vec.z);
}

// �������� ���������
CVector CVector::operator -(const CVector &vec) const
{
    return CVector(x - vec.x, y - vec.y, z - vec.z);
}

// ��������������� ��������
CVector CVector::operator -() const
{
    return CVector(-x, -y, -z);
}

// +=��������
void CVector::operator +=(const CVector &vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
}

// -= ��������
void CVector::operator -=(const CVector &vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
}

// �������� ���������
bool CVector::operator ==(const CVector &vec) const
{
    return (TEqual(x, vec.x, .001f) &&
            TEqual(y, vec.y, .001f) &&
            TEqual(z, vec.z, .001f));
}

// �������� !=
bool CVector::operator !=(const CVector &vec) const
{
    return !(*this == vec);
}

// ���������
float CVector::operator *(const CVector &vec) const
{
    return (x * vec.x + y * vec.y + z * vec.z);
}

// ���������� ������ � ���������� ������� x,y,z
void CVector::set(float xxx, float yyy, float zzz)
{
    x = xxx;
    y = yyy;
    z = zzz;
}

// ������������� ������ � ������� ����������� CVector
void CVector::set(const CVector &vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
}

// ������ ������ �������������
void CVector::negate()
{
    x = -x;
    y = -y;
    z = -z;
}

// ����������� ������
void CVector::normalize()
{
    assert(!TEqual(magnitude(), 0.0f, .001f)); // ��������, ��� ������ �� 0

    float oneOverLen = 1.0f / magnitude();

    x *= oneOverLen;
    y *= oneOverLen;
    z *= oneOverLen;
}

// ������������ ������ �� ���������� ��������
void CVector::scale(float amt)
{
    x *= amt;
    y *= amt;
    z *= amt;
}

// ���������� ��������
float CVector::magnitude() const
{
    return sqrtf((x * x) + (y * y) + (z * z));
}

// �������  cross
void CVector::crossProduct(const CVector &vec, CVector &result) const
{
    result.x = (y * vec.z) - (vec.y * z);
    result.y = (z * vec.x) - (vec.z * x);
    result.z = (x * vec.y) - (vec.x * y);
}

// ���� ������� cross
CVector CVector::crossProduct(const CVector &vec) const
{
    return CVector((y * vec.z) - (vec.y * z),
                   (z * vec.x) - (vec.z * x),
                   (x * vec.y) - (vec.x * y));
}