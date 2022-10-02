#pragma once

#include "typedefs.h"

bool isRuleInFacts(const Facts& facts, const Rule& rule);
void removeTokenFromFacts(Facts* pFacts, const Token& requestedToken);
