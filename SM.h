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
	private:
	    Actions m_actions;

	public:
		SM()
		{
			Expression exp_in_00({SweenBob::N_INITIALIZE, SweenBob::N_STOP});
			Rule rule_00({exp_in_00});
			Expression exp_out_00({SweenBob::INITIALIZE, SweenBob::CHECK_STOP});
			Action act_00({rule_00, exp_out_00});

			Expression exp_in_01({SweenBob::INITIALIZE});
			Rule rule_01({exp_in_01});
			Expression exp_out_01({SweenBob::CALCULATE_COUNTER});
			Action act_01({rule_01, exp_out_01});

			Expression exp_in_02({SweenBob::CHECK_STOP});
			Rule rule_02({exp_in_02});
			Expression exp_out_02({SweenBob::STOP, SweenBob::N_INITIALIZE, SweenBob::N_CALCULATE_COUNTER, SweenBob::N_CHECK_STOP});
			Action act_02({rule_02, exp_out_02});

			m_actions.push_back(act_00);
			m_actions.push_back(act_01);
			m_actions.push_back(act_02);
		}

		virtual ~SM()
		{
		}

		bool executeProgram()
		{
			return RBS<SweenBob>::executeProgram(m_actions);
		}

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
