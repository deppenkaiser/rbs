#include "functions.h"

#include <stdint.h>

bool isTokenInFacts(const Facts& facts, const Token& requestedToken);
bool isTokenNotInFacts(const Facts& facts, const Token& requestedToken);

bool isTokenInFacts(const Facts& facts, const Token& requestedToken)
{
    bool bRetVal = false;

    for (const Token& fact : facts)
    {
        if (requestedToken == fact)
        {
            bRetVal = true;
            break;
        }
    }

    return bRetVal;
}

bool isTokenNotInFacts(const Facts& facts, const Token& requestedToken)
{
    return isTokenInFacts(facts, static_cast<Token>(requestedToken + 1)) == false;
}

void addToken(Facts* pFacts, const Token& requestedToken)
{
    if (isTokenInFacts(*pFacts, requestedToken) == false)
    {
        pFacts->push_back(requestedToken);
    }
}

void removeToken(Facts* pFacts, const Token& requestedToken)
{
    for (Facts::iterator it = pFacts->begin(); it != pFacts->end(); ++it)
    {
        if (*it == static_cast<Token>(requestedToken + 1))
        {
            pFacts->erase(it);
            break;
        }
    }
}

bool isRuleInFacts(const Facts& facts, const Rule& rule)
{
    bool bRetVal = false;

    uint32_t nTokenCount = 0;
    for (const Expression& expression : rule)
    {
        for (const Token& token : expression)
        {
            if (token % 2)
            {
                if (isTokenInFacts(facts, token))
                {
                    nTokenCount++;
                }
            }
            else
            {
                if (isTokenNotInFacts(facts, token))
                {
                    nTokenCount++;
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
