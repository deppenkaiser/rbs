
#include <iostream>

#include "functions.h"

int main()
{
    Facts facts;
    Rules rules;

    facts.push_back(Token::A);
    facts.push_back(Token::B);

    rules.push_back(Rule({Expression({Token::A, Token::C}), Expression({Token::B, Token::C}), Expression({Token::A, Token::B})}));

    while (true)
    {
        for (const Rule& rule : rules)
        {
            if (isRuleInFacts(facts, rule))
            {
                std::cout << "Hit!" << std::endl;
            }
            else
            {
                std::cout << "Miss!" << std::endl;
            }
        }
    }

    return 0;
}
