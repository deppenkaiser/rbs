#include "FactsManager.h"

#include <stdint.h>

FactsManager::FactsManager()
{
}

FactsManager::~FactsManager()
{
}

void FactsManager::setValue(const Token& token, uint32_t uValue)
{
    m_values[token].eType = ValueType::U_INTEGER;
    m_values[token].value.uValue = uValue;
}

void FactsManager::setValue(const Token& token, int32_t iValue)
{
    m_values[token].eType = ValueType::I_INTEGER;
    m_values[token].value.uValue = iValue;
}

void FactsManager::setValue(const Token& token, float fValue)
{
    m_values[token].eType = ValueType::FLOAT;
    m_values[token].value.uValue = fValue;
}

bool FactsManager::isTokenTrue(const Token& token)
{
    return (token % 2) == 1;
}

bool FactsManager::isTokenInFacts(const Token& token, bool bInput)
{
    bool bRetVal = false;

    for (const Token& fact : m_facts)
    {
        if (token == fact)
        {
            bRetVal = true;

            if ((m_values.find(token) != m_values.end()) && bInput)
            {
                bRetVal = calculateTokenValue(token, &m_values[token]);
            }            
            break;
        }
    }

    return bRetVal;
}

bool FactsManager::isTokenNotInFacts(const Token& token, bool bInput)
{
    return isTokenInFacts(static_cast<Token>(token + 1), bInput) == false;
}

void FactsManager::updateFacts(const Token& token)
{
    if (isTokenTrue(token))
    {
        addToken(token);
    }
    else
    {
        removeToken(token);
    }
}

void FactsManager::removeToken(const Token& token)
{
    for (Facts::iterator it = m_facts.begin(); it != m_facts.end(); ++it)
    {
        if (*it == static_cast<Token>(token + 1))
        {
            m_facts.erase(it);
            break;
        }
    }
}

bool FactsManager::isRuleInFacts(const Rule& rule)
{
    bool bRetVal = false;

    uint32_t nTokenCount = 0;
    for (const Expression& expression : rule)
    {
        for (const Token& token : expression)
        {
            if (isTokenTrue(token))
            {
                if (isTokenInFacts(token, true))
                {
                    ++nTokenCount;
                }
            }
            else
            {
                if (isTokenNotInFacts(token, true))
                {
                    ++nTokenCount;
                }
            }

            if (nTokenCount == expression.size())
            {
                bRetVal = true;
                break;
            }
        }

        nTokenCount = 0;
    }

    return bRetVal;
}

void FactsManager::addToken(const Token& token)
{
    if (isTokenInFacts(token, false) == false)
    {
        m_facts.push_back(token);
    }
}

bool FactsManager::executeProgram(const Actions& actions)
{
    bool bLoopAgain = false;
    for (const Action& action : actions)
    {
        if (isRuleInFacts(action.rule))
        {
            for (const Token& token : action.output)
            {
                updateFacts(token);
            }

            bLoopAgain = true;
        }
    }

    return bLoopAgain;
}

bool FactsManager::calculateTokenValue(const Token& token, TokenValue* pTokenValue)
{
    bool bRetVal = true;

    switch (token)
    {
        case Token::B:
            bRetVal = pTokenValue->value.uValue > 20;
            break;

        default:
            break;
    }

    return bRetVal;
}
