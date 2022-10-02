#pragma once

#include <stdint.h>
#include <vector>
#include <map>
#include <string>

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

enum class ValueType
{
    U_INTEGER,
    I_INTEGER,
    FLOAT
};

struct TokenValue
{
    ValueType eType;

    union Value
    {
        uint32_t uValue;
        int32_t iValue;
        float fValue;
    } value;
};

typedef std::map<Token, TokenValue> Values;
typedef bool (*CalculationCallback)(void* pData, const Token& token, TokenValue* pTokenValue);
