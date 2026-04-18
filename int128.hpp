#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

class Int128
{
public:
    Int128() noexcept;
    Int128(int64_t value) noexcept;
    explicit Int128(std::string_view text);

    explicit operator int64_t() const noexcept;
    explicit operator double() const noexcept;

    std::string str() const;

    Int128& operator+=(const Int128& other) noexcept;
    Int128& operator-=(const Int128& other) noexcept;
    Int128& operator*=(const Int128& other) noexcept;
    Int128& operator/=(const Int128& other) noexcept;

    Int128 operator-() const noexcept;

    friend Int128 operator+(Int128 lhs, const Int128& rhs) noexcept;
    friend Int128 operator-(Int128 lhs, const Int128& rhs) noexcept;
    friend Int128 operator*(Int128 lhs, const Int128& rhs) noexcept;
    friend Int128 operator/(Int128 lhs, const Int128& rhs) noexcept;

    friend bool operator==(const Int128& lhs, const Int128& rhs) noexcept;
    friend bool operator!=(const Int128& lhs, const Int128& rhs) noexcept;

    friend std::ostream& operator<<(std::ostream& out, const Int128& value);

private:
    using storage_type = __int128_t;
    using unsigned_storage_type = __uint128_t;

    storage_type value_;

    static storage_type parse(std::string_view text);
    static std::string unsigned_to_string(unsigned_storage_type value);
};