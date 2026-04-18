#include "int128.hpp"

#include <algorithm>
#include <ostream>

Int128::Int128() noexcept
    : value_(0)
{
}

Int128::Int128(int64_t value) noexcept
    : value_(static_cast<storage_type>(value))
{
}

Int128::Int128(std::string_view text)
    : value_(parse(text))
{
}

Int128::operator int64_t() const noexcept
{
    return static_cast<int64_t>(value_);
}

Int128::operator double() const noexcept
{
    return static_cast<double>(value_);
}

std::string Int128::unsigned_to_string(unsigned_storage_type value)
{
    if (value == 0)
    {
        return "0";
    }

    std::string result;
    while (value > 0)
    {
        const unsigned digit = static_cast<unsigned>(value % 10);
        result.push_back(static_cast<char>('0' + digit));
        value /= 10;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

std::string Int128::str() const
{
    if (value_ == 0)
    {
        return "0";
    }

    if (value_ < 0)
    {
        const unsigned_storage_type magnitude =
            static_cast<unsigned_storage_type>(-(value_ + 1)) + 1;
        return "-" + unsigned_to_string(magnitude);
    }

    return unsigned_to_string(static_cast<unsigned_storage_type>(value_));
}

Int128& Int128::operator+=(const Int128& other) noexcept
{
    value_ += other.value_;
    return *this;
}

Int128& Int128::operator-=(const Int128& other) noexcept
{
    value_ -= other.value_;
    return *this;
}

Int128& Int128::operator*=(const Int128& other) noexcept
{
    value_ *= other.value_;
    return *this;
}

Int128& Int128::operator/=(const Int128& other) noexcept
{
    if (other.value_ == 0)
    {
        value_ = 0;
        return *this;
    }

    value_ /= other.value_;
    return *this;
}

Int128 Int128::operator-() const noexcept
{
    Int128 result;
    result.value_ = -value_;
    return result;
}

Int128 operator+(Int128 lhs, const Int128& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

Int128 operator-(Int128 lhs, const Int128& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

Int128 operator*(Int128 lhs, const Int128& rhs) noexcept
{
    lhs *= rhs;
    return lhs;
}

Int128 operator/(Int128 lhs, const Int128& rhs) noexcept
{
    lhs /= rhs;
    return lhs;
}

bool operator==(const Int128& lhs, const Int128& rhs) noexcept
{
    return lhs.value_ == rhs.value_;
}

bool operator!=(const Int128& lhs, const Int128& rhs) noexcept
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const Int128& value)
{
    out << value.str();
    return out;
}

Int128::storage_type Int128::parse(std::string_view text)
{
    if (text.empty())
    {
        return 0;
    }

    std::size_t pos = 0;
    bool negative = false;

    if (text[pos] == '+' || text[pos] == '-')
    {
        negative = (text[pos] == '-');
        ++pos;
    }

    unsigned_storage_type magnitude = 0;
    for (; pos < text.size(); ++pos)
    {
        const char c = text[pos];
        if (c < '0' || c > '9')
        {
            break;
        }
        magnitude = magnitude * 10 + static_cast<unsigned>(c - '0');
    }

    if (negative)
    {
        const storage_type result = -static_cast<storage_type>(magnitude);
        return result;
    }

    return static_cast<storage_type>(magnitude);
}