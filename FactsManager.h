#pragma once

#include <stdint.h>
#include <vector>
#include <map>

template<class TokenType>
class FactsManager
{
	private:
		typedef std::vector<TokenType> Facts;

		enum class ValueType
		{
			U_INTEGER,
			I_INTEGER,
			FLOAT
		};

		struct TokenValue
		{
			ValueType eType;

			union Value
			{
				uint32_t uValue;
				int32_t iValue;
				float fValue;
			} value;
		};

		typedef std::map<TokenType, TokenValue> Values;
	
	public:
		typedef std::vector<TokenType> Expression;
		typedef std::vector<Expression> Rule;

		struct Action
		{
			Rule rule;
			Expression output;
		};

		typedef std::vector<Action> Actions;

	private:
	    Facts m_facts;
		Values m_values;

	private:
		bool isTokenTrue(const TokenType& token)
		{
			return (token % 2) == 1;
		}

		bool isTokenInFacts(const TokenType& token, bool bInput)
		{
			bool bRetVal = false;

			for (const TokenType& fact : m_facts)
			{
				if (token == fact)
				{
					bRetVal = true;

					if ((m_values.find(token) != m_values.end()) && bInput)
					{
						bRetVal = calculateTokenValue(token, &m_values[token]);
					}            
					break;
				}
			}

			return bRetVal;
		}

		bool isTokenNotInFacts(const TokenType& token, bool bInput)
		{
			return isTokenInFacts(static_cast<TokenType>(token + 1), bInput) == false;
		}

		bool isRuleInFacts(const Rule& rule)
		{
			bool bRetVal = false;

			uint32_t nTokenCount = 0;
			for (const Expression& expression : rule)
			{
				for (const TokenType& token : expression)
				{
					if (isTokenTrue(token))
					{
						if (isTokenInFacts(token, true))
						{
							++nTokenCount;
						}
					}
					else
					{
						if (isTokenNotInFacts(token, true))
						{
							++nTokenCount;
						}
					}

					if (nTokenCount == expression.size())
					{
						bRetVal = true;
						break;
					}
				}

				nTokenCount = 0;
			}

			return bRetVal;
		}

		void addToken(const TokenType& token)
		{
			if (isTokenInFacts(token, false) == false)
			{
				m_facts.push_back(token);
			}
		}

		void updateFacts(const TokenType& token)
		{
			if (isTokenTrue(token))
			{
				addToken(token);
			}
			else
			{
				removeToken(static_cast<TokenType>(token + 1));
			}
		}

		void removeToken(const TokenType& token)
		{
			std::vector<TokenType> tmp;

			for (std::size_t i = 0; i < m_facts.size(); ++i)
			{
				if (m_facts[i] != token)
				{
					tmp.push_back(m_facts[i]);
				}
			}

			m_facts = tmp;
		}

		bool calculateTokenValue(const TokenType& token, TokenValue* pTokenValue)
		{
			bool bRetVal = true;

			switch (token)
			{
				case TokenType::B:
					bRetVal = pTokenValue->value.uValue > 20;
					break;

				default:
					break;
			}

			return bRetVal;
		}
	
	public:
		FactsManager()
		{
		}

		~FactsManager()
		{
		}

		bool executeProgram(const Actions& actions)
		{
			bool bLoopAgain = false;
			for (const Action& action : actions)
			{
				if (isRuleInFacts(action.rule))
				{
					for (const TokenType& token : action.output)
					{
						updateFacts(token);
					}

					bLoopAgain = true;
				}
			}

			return bLoopAgain;
		}

		void setValue(const TokenType& token, uint32_t uValue)
		{
			m_values[token].eType = ValueType::U_INTEGER;
			m_values[token].value.uValue = uValue;
		}

		void setValue(const TokenType& token, int32_t iValue)
		{
			m_values[token].eType = ValueType::I_INTEGER;
			m_values[token].value.uValue = iValue;
		}

		void setValue(const TokenType& token, float fValue)
		{
			m_values[token].eType = ValueType::FLOAT;
			m_values[token].value.uValue = fValue;
		}
};
