#pragma once

#include "typedefs.h"

bool executeProgram(const Actions& actions, Facts* pFacts);
bool isRuleInFacts(const Facts& facts, const Rule& rule);
void addToken(Facts* pFacts, const Token& requestedToken);
void removeToken(Facts* pFacts, const Token& requestedToken);
