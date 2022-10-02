
#include <iostream>

#include "functions.h"

int main()
{
    Facts facts;
    Actions actions;

    actions.push_back({Rule({Expression({Token::NA})}), Expression({Token::B})});
    actions.push_back({Rule({Expression({Token::B})}), Expression({Token::A, Token::NB})});

    bool bLoopAgain = false;
    do
    {
        bLoopAgain = false;
        for (const Action& action : actions)
        {
            if (isRuleInFacts(facts, action.rule))
            {
                for (const Token& token : action.output)
                {
                    if (token % 2)
                    {
                        addToken(&facts, token);
                    }
                    else
                    {
                        removeToken(&facts, token);
                    }
                }

                bLoopAgain = true;
            }
        }
    } while(bLoopAgain);

    return 0;
}
