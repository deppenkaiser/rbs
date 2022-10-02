
#include <iostream>

#include "functions.h"

int main()
{
    Facts facts;
    Actions actions;

    actions.push_back({Rule({Expression({Token::NB})}), Expression({Token::B})});
    actions.push_back({Rule({Expression({Token::B})}), Expression({Token::C})});
    actions.push_back({Rule({Expression({Token::C})}), Expression({Token::A, Token::NB, Token::NC})});
    actions.push_back({Rule({Expression({Token::A})}), Expression({Token::NA})});

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
                        removeTokenFromFacts(&facts, token);
                    }
                }
            }
        }
    }

    return 0;
}
