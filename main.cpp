
#include "FactsManager.h"

int main()
{
    Actions actions;
    FactsManager fm;

    fm.setValue(Token::B, 100);

    actions.push_back({Rule({Expression({Token::NA})}), Expression({Token::B})});
    actions.push_back({Rule({Expression({Token::B})}), Expression({Token::A, Token::NB})});

    while(fm.executeProgram(actions));

    return 0;
}
