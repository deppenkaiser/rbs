
#include <stdint.h>
#include <vector>
#include <iostream>

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

bool inFacts(const Facts& facts, const Token& requestedToken)
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

int main()
{
    Facts facts;
    Rules rules;

    facts.push_back(Token::A);
    facts.push_back(Token::B);

    rules.push_back(Rule({Expression({Token::A, Token::C}), Expression({Token::B, Token::C}), Expression({Token::A, Token::B})}));

    while (true)
    {
        bool bRuleFulfilled = false;
        for (const Rule& rule : rules)
        {
            uint32_t nTokenCount = 0;
            for (const Expression& expression : rule)
            {
                for (const Token& token : expression)
                {
                    if (inFacts(facts, token))
                    {
                        nTokenCount++;
                    }

                    if (nTokenCount == expression.size())
                    {
                        bRuleFulfilled = true;
                        break;
                    }
                }

                nTokenCount = 0;
            }
        }

        if (bRuleFulfilled)
        {
            std::cout << "Hit!" << std::endl;
        }
        else
        {
            std::cout << "Miss!" << std::endl;
        }
    }

    return 0;
}
