
#include "SM.h"

int main()
{
    SM::Actions actions;
    SM sm;

    sm.setValue(SweenBob::B, 100);

    actions.push_back({SM::Rule({SM::Expression({SweenBob::NA})}), SM::Expression({SweenBob::B})});
    actions.push_back({SM::Rule({SM::Expression({SweenBob::B})}), SM::Expression({SweenBob::A, SweenBob::NB})});

    bool bLoop = false;
    do
    {
        bLoop = sm.executeProgram(actions);
    } while (bLoop);

    return 0;
}
