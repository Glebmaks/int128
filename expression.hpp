#pragma once

#include "int128.hpp"
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

using Arguments = std::map<std::string, Int128>;

struct Expression
{
    virtual Int128 eval(Arguments const& values = {}) const = 0;
    virtual Expression* clone() const = 0;
    virtual std::string str() const = 0;

    friend std::ostream& operator<<(std::ostream& out, const Expression& expression)
    {
        out << expression.str();
        return out;
    }

    virtual ~Expression() = default;
};

struct Const : Expression
{
    const Int128 cnst;

    Const(Int128 value)
        : cnst(std::move(value))
    {
    }

    Const(int64_t value)
        : cnst(value)
    {
    }

    explicit Const(std::string_view value)
        : cnst(value)
    {
    }

    Int128 eval(Arguments const& = {}) const override
    {
        return cnst;
    }

    Expression* clone() const override
    {
        return new Const(cnst);
    }

    std::string str() const override
    {
        return cnst.str();
    }
};

struct Variable : Expression
{
    const std::string name;

    Variable(std::string value)
        : name(std::move(value))
    {
    }

    Variable(char const* value)
        : name(value)
    {
    }

    Int128 eval(Arguments const& values = {}) const override
    {
        return values.at(name);
    }

    Expression* clone() const override
    {
        return new Variable(name);
    }

    std::string str() const override
    {
        return name;
    }
};

struct BinaryOperation : Expression
{
    const std::shared_ptr<Expression> a;
    const std::shared_ptr<Expression> b;
    const char operation;

    BinaryOperation(Expression const& x, Expression const& y, char op)
        : a(x.clone())
        , b(y.clone())
        , operation(op)
    {
    }

    BinaryOperation(std::shared_ptr<Expression> const& x,
                    std::shared_ptr<Expression> const& y,
                    char op)
        : a(x)
        , b(y)
        , operation(op)
    {
    }

    std::string str() const override
    {
        return "(" + a->str() + " " + operation + " " + b->str() + ")";
    }
};

struct Negate : Expression
{
    const std::shared_ptr<Expression> a;

    Negate(Expression const& x)
        : a(x.clone())
    {
    }

    Negate(std::shared_ptr<Expression> const& x)
        : a(x)
    {
    }

    Int128 eval(Arguments const& values = {}) const override
    {
        return -a->eval(values);
    }

    Expression* clone() const override
    {
        return new Negate(a);
    }

    std::string str() const override
    {
        return "(-" + a->str() + ")";
    }
};

struct Add : BinaryOperation
{
    Add(Expression const& x, Expression const& y)
        : BinaryOperation(x, y, '+')
    {
    }

    Add(std::shared_ptr<Expression> const& x, std::shared_ptr<Expression> const& y)
        : BinaryOperation(x, y, '+')
    {
    }

    Int128 eval(Arguments const& values = {}) const override
    {
        return a->eval(values) + b->eval(values);
    }

    Expression* clone() const override
    {
        return new Add(a, b);
    }
};

struct Subtract : BinaryOperation
{
    Subtract(Expression const& x, Expression const& y)
        : BinaryOperation(x, y, '-')
    {
    }

    Subtract(std::shared_ptr<Expression> const& x, std::shared_ptr<Expression> const& y)
        : BinaryOperation(x, y, '-')
    {
    }

    Int128 eval(Arguments const& values = {}) const override
    {
        return a->eval(values) - b->eval(values);
    }

    Expression* clone() const override
    {
        return new Subtract(a, b);
    }
};

struct Multiply : BinaryOperation
{
    Multiply(Expression const& x, Expression const& y)
        : BinaryOperation(x, y, '*')
    {
    }

    Multiply(std::shared_ptr<Expression> const& x, std::shared_ptr<Expression> const& y)
        : BinaryOperation(x, y, '*')
    {
    }

    Int128 eval(Arguments const& values = {}) const override
    {
        return a->eval(values) * b->eval(values);
    }

    Expression* clone() const override
    {
        return new Multiply(a, b);
    }
};

struct Divide : BinaryOperation
{
    Divide(Expression const& x, Expression const& y)
        : BinaryOperation(x, y, '/')
    {
    }

    Divide(std::shared_ptr<Expression> const& x, std::shared_ptr<Expression> const& y)
        : BinaryOperation(x, y, '/')
    {
    }

    Int128 eval(Arguments const& values = {}) const override
    {
        return a->eval(values) / b->eval(values);
    }

    Expression* clone() const override
    {
        return new Divide(a, b);
    }
};

inline Add operator+(Expression const& a, Expression const& b)
{
    return Add(a, b);
}

inline Subtract operator-(Expression const& a, Expression const& b)
{
    return Subtract(a, b);
}

inline Multiply operator*(Expression const& a, Expression const& b)
{
    return Multiply(a, b);
}

inline Divide operator/(Expression const& a, Expression const& b)
{
    return Divide(a, b);
}

inline Negate operator-(Expression const& a)
{
    return Negate(a);
}