#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

class CVector
{
    public:

        // Конструкторы
        CVector() : x(0.0f), y(0.0f), z(0.0f) {}
        CVector(float xxx, float yyy, float zzz) : x(xxx), y(yyy), z(zzz) {}

        // Копирующий конструктор
        CVector(const CVector &vec) : x(vec.x), y(vec.y), z(vec.z) {}

        // Перегружаем операторы
        CVector& operator =(const CVector &vec);
        CVector operator +(const CVector &vec) const;
        CVector operator -(const CVector &vec) const;
        CVector operator -() const;
        void operator +=(const CVector &vec);
        void operator -=(const CVector &vec);
        bool operator ==(const CVector &vec) const;
        bool operator !=(const CVector &vec) const;

        float operator *(const CVector &vec) const;

        // Устанавливает вектор в переданные переменные
        void set(float xxx, float yyy, float zzz);
        void set(const CVector &vec);

        void negate(); // Вычитает вектор
        void normalize(); // Нормализует вектор
        void scale(float amt); // Масштабирует вектор на переданные значения

        float magnitude() const; // Возвращает величину вектора

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