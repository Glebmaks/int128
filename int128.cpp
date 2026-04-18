#include "int128/Int128.hpp"

#include <algorithm>
#include <bitset>
#include <cmath>
#include <iostream>

// Конструирование без аргументов
Int128::Int128() : high(0), low(0) {}

// Конструирование от int64_t
Int128::Int128(int64_t num) : high(0) {
    low  = static_cast<uint64_t>(num);
    high = static_cast<int64_t>(num < 0 ? ~0 : 0);
}

// Конструирование от int64_t(high) и uint64_t(low)
Int128::Int128(int64_t _high, uint64_t _low) : high(_high), low(_low) {}

// Конструирование от std::string_view
Int128::Int128(std::string_view str) : high(0), low(0) {
    bool is_negative = false;
    size_t start     = 0;
    if (str[0] == '-') {
        is_negative = true;
        start       = 1;
    }
    for (size_t i = start; i < str.size(); ++i) {
        *this *= Int128(10);
        *this += Int128(str[i] - '0');
    }
    if (is_negative) {
        *this = -*this;
    }
}

// Явное приведение к int64_t
Int128::operator int64_t() const {
    uint64_t sign_bit =
        (high >> 63) << 63;  // Bit 1 in 64th place if `high` is negative (has bit in 64th place), else 0
    uint64_t low_bits = low & ~(uint64_t(1) << 63);  // `low` without first bit
    return static_cast<int64_t>(sign_bit | low_bits);
}

// Явное приведение к double
Int128::operator double() const {
    return std::ldexp(double(high), 64) + double(low);
}

// Перевод в строку: std::string str()
std::string Int128::str() const {
    std::string result;
    Int128 tmp = (*this).abs();
    while (tmp != ZERO) {
        Int128 digit = tmp % Int128(10);
        result += (char)('0' + digit.low);
        tmp /= Int128(10);
    }
    if (*this < ZERO) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// Сложение: +, +=
Int128 Int128::operator+(const Int128 &_rhs) const {
    Int128 lhs = *this;
    Int128 rhs = _rhs;
    Int128 result;
    int64_t carry = 0;
    for (int i = 0; i < 128; ++i) {
        int64_t lhs_bit = static_cast<int64_t>((lhs >> i) & ONE);
        int64_t rhs_bit = static_cast<int64_t>((rhs >> i) & ONE);
        int64_t sum     = lhs_bit + rhs_bit + carry;

        carry = sum >> 1;
        result |= Int128(sum & 1) << i;
    }
    return result;
}
Int128 &Int128::operator+=(const Int128 &rhs) {
    *this = *this + rhs;
    return *this;
}

// Вычитание: -, -=
Int128 Int128::operator-(const Int128 &rhs) const {
    return *this + -rhs;
}
Int128 &Int128::operator-=(const Int128 &rhs) {
    *this = *this - rhs;
    return *this;
}

// Умножение: *, *=
Int128 Int128::operator*(const Int128 &_rhs) const {
    Int128 result;
    Int128 lhs = *this;
    Int128 rhs = _rhs;
    while (rhs != ZERO) {
        if (rhs.low & 1) {
            result += lhs;
        }
        lhs += lhs;
        rhs >>= 1;
    }
    return result;
}
Int128 &Int128::operator*=(const Int128 &rhs) {
    *this = *this * rhs;
    return *this;
}

// Деление: /, /=
Int128 Int128::operator/(const Int128 &_rhs) const {  // a / b = c
    // Перевести в unsigned и запомнить знак
    bool is_negative = false;
    Int128 lhs       = *this;  // a
    if (lhs < ZERO) {
        is_negative = !is_negative;
        lhs         = -lhs;
    }
    Int128 rhs = _rhs;  // b
    if (rhs < ZERO) {
        is_negative = !is_negative;
        rhs         = -rhs;
    }

    // Деление
    Int128 result;  // c
    while (rhs <= lhs) {
        Int128 tmp  = rhs;
        Int128 tmp2 = ONE;
        while (tmp.bit_le(lhs)) {
            tmp <<= 1;
            tmp2 <<= 1;
        }
        tmp >>= 1;
        tmp2 >>= 1;

        lhs -= tmp;
        result += tmp2;
    }

    if (is_negative) {
        result = -result;
    }
    return result;
}
Int128 &Int128::operator/=(const Int128 &rhs) {
    *this = *this / rhs;
    return *this;
}

// Унарный минус: -
Int128 Int128::operator-() const {
    Int128 result = *this;
    result.high   = ~result.high;
    result.low    = ~result.low;
    if (result.low == UINT64_MAX) {
        ++result.high;
        result.low = 0;
    } else {
        ++result.low;
    }
    return result;
}

// Сравнение на равенство: ==, !=
bool Int128::operator==(const Int128 &rhs) const {
    return high == rhs.high && low == rhs.low;
}
bool Int128::operator!=(const Int128 &rhs) const {
    return !(*this == rhs);
}

// Вывод в поток: <<
std::ostream &operator<<(std::ostream &out, const Int128 &num) {
    return out << num.str();
}

// ====================================================== Custom =======================================================

// Строка из бит (с разделителем)
std::string Int128::bit_string(bool separator) const {
    std::bitset<64> high_bits(high);
    std::bitset<64> low_bits(low);
    std::string result = high_bits.to_string();
    if (separator) {
        result += '|';
    }
    result += low_bits.to_string();
    return result;
}

// Побитовое NOT (~)
Int128 Int128::operator~() const {
    return Int128(~high, ~low);
}

// Константы 0 и 1
const Int128 Int128::ZERO = Int128(0);
const Int128 Int128::ONE  = Int128(1);

// Битовый сдвиг влево
Int128 Int128::operator<<(int shift) const {
    if (shift == 0) {
        return *this;
    }
    Int128 result;
    if (shift >= 64) {
        result.high = static_cast<int64_t>(low) << (shift - 64);
    } else {
        result.high = static_cast<int64_t>(high << shift) | static_cast<int64_t>(low >> (64 - shift));
        result.low  = low << shift;
    }
    return result;
}
Int128 &Int128::operator<<=(int shift) {
    *this = *this << shift;
    return *this;
}

// Битовый сдвиг вправо
Int128 Int128::operator>>(int shift) const {
    if (shift == 0) {
        return *this;
    }
    Int128 result;
    if (shift >= 64) {
        result.low = static_cast<uint64_t>(high) >> (shift - 64);
    } else {
        result.low  = (low >> shift) | (high << (64 - shift));
        result.high = static_cast<int64_t>(static_cast<uint64_t>(high) >> shift);
    }
    return result;
}
Int128 &Int128::operator>>=(int shift) {
    *this = *this >> shift;
    return *this;
}

// Остаток от деления: %, %=
Int128 Int128::operator%(const Int128 &rhs) const {
    return *this - (*this / rhs) * rhs;
}
Int128 &Int128::operator%=(const Int128 &rhs) {
    *this = *this % rhs;
    return *this;
}

// Сравнение: <, <=
bool Int128::operator<(const Int128 &rhs) const {
    if (high == rhs.high) {
        return low < rhs.low;
    }
    return high < rhs.high;
}
bool Int128::operator<=(const Int128 &rhs) const {
    return *this < rhs || *this == rhs;
}

// Сравнение: >, >=
bool Int128::operator>(const Int128 &rhs) const {
    return rhs < *this;
}
bool Int128::operator>=(const Int128 &rhs) const {
    return rhs <= *this;
}

// Сравнение побитовое (беззнаковое): <, <=
bool Int128::bit_lt(const Int128 &rhs) const {
    if (high == rhs.high) {
        return low < rhs.low;
    }
    return static_cast<uint64_t>(high) < static_cast<uint64_t>(rhs.high);
}
bool Int128::bit_le(const Int128 &rhs) const {
    return bit_lt(rhs) || *this == rhs;
}

// Побитовое И: &, &=
Int128 Int128::operator&(const Int128 &rhs) const {
    return Int128(high & rhs.high, low & rhs.low);
}
Int128 &Int128::operator&=(const Int128 &rhs) {
    *this = *this & rhs;
    return *this;
}

// Побитовое ИЛИ: |, |=
Int128 Int128::operator|(const Int128 &rhs) const {
    return Int128(high | rhs.high, low | rhs.low);
}
Int128 &Int128::operator|=(const Int128 &rhs) {
    *this = *this | rhs;
    return *this;
}

// Модуль числа: abs()
Int128 Int128::abs() const {
    if (high < 0) {
        return -*this;
    }
    return *this;
}