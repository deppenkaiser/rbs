
#include <iostream>

#include "functions.h"

int main()
{
    Facts facts;
    Actions actions;

    facts.push_back(Token::B);

    actions.push_back({Rule({Expression({Token::B})}), Expression({Token::C})});
    actions.push_back({Rule({Expression({Token::C})}), Expression({Token::A, Token::NB, Token::NC})});

    while (true)
    {
        for (const Action& action : actions)
        {
            if (isRuleInFacts(facts, action.rule))
            {
                for (const Token& token : action.output)
                {
                    if (token % 2)
                    {
                        facts.push_back(token);
                    }
                    else
                    {
                        for (Facts::iterator it = facts.begin(); it != facts.end(); ++it)
                        {
                            if (*it == static_cast<Token>(token + 1))
                            {
                                facts.erase(it);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
