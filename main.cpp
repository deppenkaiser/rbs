
#include <iostream>

#include "functions.h"

int main()
{
    Facts facts;
    Actions actions;

    actions.push_back({Rule({Expression({Token::NA})}), Expression({Token::B})});
    actions.push_back({Rule({Expression({Token::B})}), Expression({Token::A, Token::NB})});

    while(executeProgram(actions, &facts));

    return 0;
}
