#include "functions.h"

#include <stdint.h>

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

bool isRuleInFacts(const Facts& facts, const Rule& rule)
{
    bool bRetVal = false;

    uint32_t nTokenCount = 0;
    for (const Expression& expression : rule)
    {
        for (const Token& token : expression)
        {
            if (isTokenInFacts(facts, token))
            {
                nTokenCount++;
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
