#pragma once

#include "RBS.h"

enum SweenBob
{
	NA, A,
	NB, B
};

class SM : public RBS<SweenBob>
{
	public:
		virtual bool calculateTokenValue(const SweenBob& token, TokenValue* pTokenValue)
		{
			bool bRetVal = true;

			switch (token)
			{
				case SweenBob::A:
					break;

				case SweenBob::B:
					bRetVal = pTokenValue->value.uValue < 20;
					setValue(token, 10);
					break;
			}

			return bRetVal;
		}
};
