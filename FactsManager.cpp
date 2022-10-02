#include "FactsManager.h"

FactsManager::FactsManager(Facts* pFacts)
{
	m_pFacts = pFacts;
}

FactsManager::~FactsManager()
{
}

bool FactsManager::isTokenTrue(const Token& token)
{
    return (token % 2) == 1;
}

bool FactsManager::isTokenInFacts(const Token& requestedToken)
{
    bool bRetVal = false;

    for (const Token& fact : *m_pFacts)
    {
        if (requestedToken == fact)
        {
            bRetVal = true;
            break;
        }
    }

    return bRetVal;
}

bool FactsManager::isTokenNotInFacts(const Token& requestedToken)
{
    return isTokenInFacts(static_cast<Token>(requestedToken + 1)) == false;
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

void FactsManager::removeToken(const Token& requestedToken)
{
    for (Facts::iterator it = m_pFacts->begin(); it != m_pFacts->end(); ++it)
    {
        if (*it == static_cast<Token>(requestedToken + 1))
        {
            m_pFacts->erase(it);
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
                if (isTokenInFacts(token))
                {
                    ++nTokenCount;
                }
            }
            else
            {
                if (isTokenNotInFacts(token))
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

void FactsManager::addToken(const Token& requestedToken)
{
    if (isTokenInFacts(requestedToken) == false)
    {
        m_pFacts->push_back(requestedToken);
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
