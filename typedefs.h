#pragma once

#include <vector>

enum class Token
{
    A,
    B,
    C
};

enum class Operators
{
    OR,
    NOT
};

typedef std::vector<Token> Facts;
typedef std::vector<Token> Expression;
typedef std::vector<Expression> Rule;
typedef std::vector<Rule> Rules;
