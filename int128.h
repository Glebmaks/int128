#pragma once

#include <algorithm>
#include <cstdint>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

class Int128 {
public:
    Int128() noexcept : value_(0) {}
    Int128(int64_t value) noexcept : value_(static_cast<storage_type>(value)) {}

    explicit Int128(std::string_view text) : value_(parse(text)) {}

    explicit operator int64_t() const noexcept {
        return static_cast<int64_t>(value_);
    }

    explicit operator double() const noexcept {
        return static_cast<double>(value_);
    }

    std::string str() const {
        if (value_ == 0) {
            return "0";
        }

        if (value_ < 0) {
            storage_type tmp = value_;
            ++tmp;
            tmp = -tmp;
            ++tmp;
            return "-" + to_decimal_string(static_cast<unsigned_storage_type>(tmp));
        }

        return to_decimal_string(static_cast<unsigned_storage_type>(value_));
    }

    Int128& operator+=(const Int128& other) noexcept {
        value_ += other.value_;
        return *this;
    }

    Int128& operator-=(const Int128& other) noexcept {
        value_ -= other.value_;
        return *this;
    }

    Int128& operator*=(const Int128& other) noexcept {
        value_ *= other.value_;
        return *this;
    }

    Int128& operator/=(const Int128& other) noexcept {
        value_ /= other.value_;
        return *this;
    }

    Int128 operator-() const noexcept {
        Int128 result;
        result.value_ = -value_;
        return result;
    }

    friend Int128 operator+(Int128 lhs, const Int128& rhs) noexcept {
        lhs += rhs;
        return lhs;
    }

    friend Int128 operator-(Int128 lhs, const Int128& rhs) noexcept {
        lhs -= rhs;
        return lhs;
    }

    friend Int128 operator*(Int128 lhs, const Int128& rhs) noexcept {
        lhs *= rhs;
        return lhs;
    }

    friend Int128 operator/(Int128 lhs, const Int128& rhs) noexcept {
        lhs /= rhs;
        return lhs;
    }

    friend bool operator==(const Int128& lhs, const Int128& rhs) noexcept {
        return lhs.value_ == rhs.value_;
    }

    friend bool operator!=(const Int128& lhs, const Int128& rhs) noexcept {
        return !(lhs == rhs);
    }

    friend bool operator<(const Int128& lhs, const Int128& rhs) noexcept {
        return lhs.value_ < rhs.value_;
    }

    friend bool operator>(const Int128& lhs, const Int128& rhs) noexcept {
        return rhs < lhs;
    }

    friend bool operator<=(const Int128& lhs, const Int128& rhs) noexcept {
        return !(rhs < lhs);
    }

    friend bool operator>=(const Int128& lhs, const Int128& rhs) noexcept {
        return !(lhs < rhs);
    }

    friend std::ostream& operator<<(std::ostream& out, const Int128& value);

private:
    using storage_type = __int128_t;
    using unsigned_storage_type = __uint128_t;

    storage_type value_;

    static std::string to_decimal_string(unsigned_storage_type value) {
        std::string result;
        while (value > 0) {
            const unsigned digit = static_cast<unsigned>(value % 10);
            result.push_back(static_cast<char>('0' + digit));
            value /= 10;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    static storage_type parse(std::string_view text) {
        if (text.empty()) {
            return 0;
        }

        std::size_t pos = 0;
        bool negative = false;

        if (text[pos] == '+' || text[pos] == '-') {
            negative = (text[pos] == '-');
            ++pos;
        }

        unsigned_storage_type magnitude = 0;
        for (; pos < text.size(); ++pos) {
            const char c = text[pos];
            if (c < '0' || c > '9') {
                break;
            }
            magnitude = magnitude * 10 + static_cast<unsigned>(c - '0');
        }

        if (negative) {
            storage_type result = -static_cast<storage_type>(magnitude);
            return result;
        }

        return static_cast<storage_type>(magnitude);
    }
};

inline std::ostream& operator<<(std::ostream& out, const Int128& value) {
    out << value.str();
    return out;
}

class Expression {
public:
    using Environment = std::map<std::string, Int128>;

    virtual ~Expression() = default;

    virtual Int128 eval(const Environment& variables) const = 0;
    virtual Expression* clone() const = 0;
    virtual void print(std::ostream& out) const = 0;
};

inline std::ostream& operator<<(std::ostream& out, const Expression& expr) {
    expr.print(out);
    return out;
}

class Const final : public Expression {
public:
    Const() : value_(0) {}
    Const(int64_t value) : value_(value) {}
    Const(const Int128& value) : value_(value) {}
    explicit Const(std::string_view value) : value_(value) {}

    Int128 eval(const Environment&) const override {
        return value_;
    }

    Expression* clone() const override {
        return new Const(*this);
    }

    void print(std::ostream& out) const override {
        out << value_;
    }

private:
    Int128 value_;
};

class Variable final : public Expression {
public:
    explicit Variable(std::string_view name) : name_(name) {}

    Int128 eval(const Environment& variables) const override {
        auto it = variables.find(name_);
        if (it == variables.end()) {
            return Int128();
        }
        return it->second;
    }

    Expression* clone() const override {
        return new Variable(*this);
    }

    void print(std::ostream& out) const override {
        out << name_;
    }

private:
    std::string name_;
};

class UnaryExpression : public Expression {
public:
    explicit UnaryExpression(const Expression& arg)
        : arg_(arg.clone()) {}

    UnaryExpression(const UnaryExpression& other)
        : arg_(other.arg_->clone()) {}

    UnaryExpression& operator=(const UnaryExpression& other) {
        if (this != &other) {
            arg_.reset(other.arg_->clone());
        }
        return *this;
    }

protected:
    const Expression& arg() const {
        return *arg_;
    }

private:
    std::unique_ptr<Expression> arg_;
};

class BinaryExpression : public Expression {
public:
    BinaryExpression(const Expression& lhs, const Expression& rhs)
        : lhs_(lhs.clone()), rhs_(rhs.clone()) {}

    BinaryExpression(const BinaryExpression& other)
        : lhs_(other.lhs_->clone()), rhs_(other.rhs_->clone()) {}

    BinaryExpression& operator=(const BinaryExpression& other) {
        if (this != &other) {
            lhs_.reset(other.lhs_->clone());
            rhs_.reset(other.rhs_->clone());
        }
        return *this;
    }

protected:
    const Expression& lhs() const {
        return *lhs_;
    }

    const Expression& rhs() const {
        return *rhs_;
    }

private:
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
};

class Negate final : public UnaryExpression {
public:
    explicit Negate(const Expression& arg)
        : UnaryExpression(arg) {}

    Int128 eval(const Environment& variables) const override {
        return -arg().eval(variables);
    }

    Expression* clone() const override {
        return new Negate(*this);
    }

    void print(std::ostream& out) const override {
        out << "(-" << arg() << ")";
    }
};

class Add final : public BinaryExpression {
public:
    Add(const Expression& lhs, const Expression& rhs)
        : BinaryExpression(lhs, rhs) {}

    Int128 eval(const Environment& variables) const override {
        return lhs().eval(variables) + rhs().eval(variables);
    }

    Expression* clone() const override {
        return new Add(*this);
    }

    void print(std::ostream& out) const override {
        out << '(' << lhs() << " + " << rhs() << ')';
    }
};

class Subtract final : public BinaryExpression {
public:
    Subtract(const Expression& lhs, const Expression& rhs)
        : BinaryExpression(lhs, rhs) {}

    Int128 eval(const Environment& variables) const override {
        return lhs().eval(variables) - rhs().eval(variables);
    }

    Expression* clone() const override {
        return new Subtract(*this);
    }

    void print(std::ostream& out) const override {
        out << '(' << lhs() << " - " << rhs() << ')';
    }
};

class Multiply final : public BinaryExpression {
public:
    Multiply(const Expression& lhs, const Expression& rhs)
        : BinaryExpression(lhs, rhs) {}

    Int128 eval(const Environment& variables) const override {
        return lhs().eval(variables) * rhs().eval(variables);
    }

    Expression* clone() const override {
        return new Multiply(*this);
    }

    void print(std::ostream& out) const override {
        out << '(' << lhs() << " * " << rhs() << ')';
    }
};

class Divide final : public BinaryExpression {
public:
    Divide(const Expression& lhs, const Expression& rhs)
        : BinaryExpression(lhs, rhs) {}

    Int128 eval(const Environment& variables) const override {
        return lhs().eval(variables) / rhs().eval(variables);
    }

    Expression* clone() const override {
        return new Divide(*this);
    }

    void print(std::ostream& out) const override {
        out << '(' << lhs() << " / " << rhs() << ')';
    }
};

inline Negate operator-(const Expression& arg) {
    return Negate(arg);
}

inline Add operator+(const Expression& lhs, const Expression& rhs) {
    return Add(lhs, rhs);
}

inline Subtract operator-(const Expression& lhs, const Expression& rhs) {
    return Subtract(lhs, rhs);
}

inline Multiply operator*(const Expression& lhs, const Expression& rhs) {
    return Multiply(lhs, rhs);
}

inline Divide operator/(const Expression& lhs, const Expression& rhs) {
    return Divide(lhs, rhs);
}
