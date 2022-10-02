#pragma once

#include <vector>

// Tokens may be facts and are always true, if they are facts
enum Token
{
    NA, A,
    NB, B,
    NC, C
};

typedef std::vector<Token> Facts;
typedef std::vector<Token> Expression;
typedef std::vector<Expression> Rule;

struct Action
{
    Rule rule;
    Expression output;
};

typedef std::vector<Action> Actions;
