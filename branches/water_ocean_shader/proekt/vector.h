#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

class CVector
{
    public:

        // ������������
        CVector() : x(0.0f), y(0.0f), z(0.0f) {}
        CVector(float xxx, float yyy, float zzz) : x(xxx), y(yyy), z(zzz) {}

        // ���������� �����������
        CVector(const CVector &vec) : x(vec.x), y(vec.y), z(vec.z) {}

        // ����������� ���������
        CVector& operator =(const CVector &vec);
        CVector operator +(const CVector &vec) const;
        CVector operator -(const CVector &vec) const;
        CVector operator -() const;
        void operator +=(const CVector &vec);
        void operator -=(const CVector &vec);
        bool operator ==(const CVector &vec) const;
        bool operator !=(const CVector &vec) const;

        float operator *(const CVector &vec) const;

        // ������������� ������ � ���������� ����������
        void set(float xxx, float yyy, float zzz);
        void set(const CVector &vec);

        void negate(); // �������� ������
        void normalize(); // ����������� ������
        void scale(float amt); // ������������ ������ �� ���������� ��������

        float magnitude() const; // ���������� �������� �������

        // Cross
        void crossProduct(const CVector &vec, CVector &result) const;
        CVector crossProduct(const CVector &vec) const;

        // Public data
        float x;
        float y;
        float z;
};

// Typedef
typedef CVector CPos;

#endif