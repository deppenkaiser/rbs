
#include "FactsManager.h"

int main()
{
    Facts facts;
    Actions actions;
    FactsManager fm(&facts);

    actions.push_back({Rule({Expression({Token::NA})}), Expression({Token::B})});
    actions.push_back({Rule({Expression({Token::B})}), Expression({Token::A, Token::NB})});

    while(fm.executeProgram(actions));

    return 0;
}
