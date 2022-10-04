
#include "SM.h"

int main()
{
    SM::Actions actions;
    SM sm;

    actions.push_back({SM::Rule({SM::Expression({SweenBob::N_INITIALIZE, SweenBob::N_STOP})}), SM::Expression({SweenBob::INITIALIZE, SweenBob::CHECK_STOP})});
    actions.push_back({SM::Rule({SM::Expression({SweenBob::INITIALIZE})}), SM::Expression({SweenBob::CALCULATE_COUNTER})});
    actions.push_back({SM::Rule({SM::Expression({SweenBob::CHECK_STOP})}), SM::Expression({SweenBob::STOP, SweenBob::N_INITIALIZE, SweenBob::N_CALCULATE_COUNTER, SweenBob::N_CHECK_STOP})});

    bool bLoop = false;
    do
    {
        bLoop = sm.executeProgram(actions);
    } while (bLoop);

    return 0;
}
