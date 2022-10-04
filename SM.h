#pragma once

#include "RBS.h"

enum SweenBob
{
	N_COUNTER, COUNTER,
	N_INITIALIZE, INITIALIZE,
	N_CALCULATE_COUNTER, CALCULATE_COUNTER,
	N_CHECK_STOP, CHECK_STOP,
	N_STOP, STOP
};

class SM : public RBS<SweenBob>
{
	public:
		virtual bool calculateInputValue(const SweenBob& token)
		{
			bool bRetVal = true;

			switch (token)
			{
				case SweenBob::CHECK_STOP:
					bRetVal = getValue(SweenBob::COUNTER).value.uValue < 1;
					break;
			}

			return bRetVal;
		}

		virtual void calculateOutputValue(const SweenBob& token)
		{
			switch (token)
			{
				case SweenBob::INITIALIZE:
					setValue(SweenBob::COUNTER, 100);
					break;

				case SweenBob::CALCULATE_COUNTER:
					setValue(SweenBob::COUNTER, getValue(SweenBob::COUNTER).value.uValue - 1);
					break;
			}
		}
};
