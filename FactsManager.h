#pragma once

#include "typedefs.h"

class FactsManager
{
	private:
	    Facts m_facts;
		Values m_values;

	private:
		bool isTokenTrue(const Token& token);
		bool isTokenInFacts(const Token& token, bool bInput);
		bool isTokenNotInFacts(const Token& token, bool bInput);
		bool isRuleInFacts(const Rule& rule);
		void addToken(const Token& token);
		void updateFacts(const Token& token);
		void removeToken(const Token& token);
		bool calculateTokenValue(const Token& token, TokenValue* pTokenValue);
	
	public:
		FactsManager();
		~FactsManager();
		bool executeProgram(const Actions& actions);
		void setValue(const Token& token, uint32_t uValue);
		void setValue(const Token& token, int32_t iValue);
		void setValue(const Token& token, float fValue);
};
