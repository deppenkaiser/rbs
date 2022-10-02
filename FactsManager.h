#pragma once

#include "typedefs.h"

#include <mutex>

class FactsManager
{
	private:
		Facts* m_pFacts = nullptr;
		std::recursive_mutex m_factsMutex;

	private:
		bool isTokenTrue(const Token& token);
		bool isTokenInFacts(const Token& requestedToken);
		bool isTokenNotInFacts(const Token& requestedToken);
		bool isRuleInFacts(const Rule& rule);
		void addToken(const Token& requestedToken);
		void updateFacts(const Token& token);
		void removeToken(const Token& requestedToken);
	
	public:
		FactsManager(Facts* pFacts);
		~FactsManager();
		bool executeProgram(const Actions& actions);
};
