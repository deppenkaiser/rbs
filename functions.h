#pragma once

#include "typedefs.h"

bool isRuleInFacts(const Facts& facts, const Rule& rule);
void addToken(Facts* pFacts, const Token& requestedToken);
void removeToken(Facts* pFacts, const Token& requestedToken);
