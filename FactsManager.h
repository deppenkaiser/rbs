#pragma once

#include "typedefs.h"

class FactsManager
{
	private:
	    Facts m_facts;

	private:
		bool isTokenTrue(const Token& token);
		bool isTokenInFacts(const Token& token);
		bool isTokenNotInFacts(const Token& token);
		bool isRuleInFacts(const Rule& rule);
		void addToken(const Token& token);
		void updateFacts(const Token& token);
		void removeToken(const Token& token);
	
	public:
		FactsManager();
		~FactsManager();
		bool executeProgram(const Actions& actions);
};
