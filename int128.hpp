#ifndef EXPRESSION_INT128_HPP
#define EXPRESSION_INT128_HPP

#include <cstdint>
#include <string>

class Int128 {
    int64_t high;
    uint64_t low;

public:
    // Конструирование без аргументов
    Int128();

    // Конструирование от int64_t
    explicit Int128(int64_t num);

    // Конструирование от std::string_view
    explicit Int128(std::string_view str);

    // Явное приведение к int64_t
    explicit operator int64_t() const;

    // Явное приведение к double
    explicit operator double() const;

    // Перевод в строку: std::string str()
    std::string str() const;

    // Сложение: +, +=
    Int128 operator+(const Int128& rhs) const;
    Int128& operator+=(const Int128& rhs);

    // Вычитание: -, -=
    Int128 operator-(const Int128& rhs) const;
    Int128& operator-=(const Int128& rhs);

    // Умножение: *, *=
    Int128 operator*(const Int128& rhs) const;
    Int128& operator*=(const Int128& rhs);

    // Деление: /, /=
    Int128 operator/(const Int128& rhs) const;
    Int128& operator/=(const Int128& rhs);

    // Унарный минус: -
    Int128 operator-() const;

    // Сравнение на равенство: ==, !=
    bool operator==(const Int128& rhs) const;
    bool operator!=(const Int128& rhs) const;

    // Вывод в поток: <<
    friend std::ostream& operator<<(std::ostream& out, const Int128& board);

    // ============================== Custom ==============================

private:
    // Конструирование от int64_t(high) и uint64_t(low)
    explicit Int128(int64_t high, uint64_t low);

    // Сравнение побитовое (беззнаковое): <, <=
    bool bit_lt(const Int128& rhs) const;
    bool bit_le(const Int128& rhs) const;

public:
    // Строка из бит (с разделителем)
    std::string bit_string(bool separator = false) const;

    // Побитовое NOT (~)
    Int128 operator~() const;

    // Константы 0 и 1
    static const Int128 ZERO;
    static const Int128 ONE;

    // Битовый сдвиг влево
    Int128 operator<<(int shift) const;
    Int128& operator<<=(int shift);

    // Битовый сдвиг вправо
    Int128 operator>>(int shift) const;
    Int128& operator>>=(int shift);

    // Остаток от деления: %, %=
    Int128 operator%(const Int128& rhs) const;
    Int128& operator%=(const Int128& rhs);

    // Сравнение: <, <=
    bool operator<(const Int128& rhs) const;
    bool operator<=(const Int128& rhs) const;

    // Сравнение: >, >=
    bool operator>(const Int128& rhs) const;
    bool operator>=(const Int128& rhs) const;

    // Побитовое И: &, &=
    Int128 operator&(const Int128& rhs) const;
    Int128& operator&=(const Int128& rhs);

    // Побитовое ИЛИ: |, |=
    Int128 operator|(const Int128& rhs) const;
    Int128& operator|=(const Int128& rhs);

    // Модуль числа: abs()
    Int128 abs() const;
};

#endif  // EXPRESSION_INT128_HPP