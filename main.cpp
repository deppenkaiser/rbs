
#include "FactsManager.h"

int main()
{
    enum SweenBob
    {
        NA, A,
        NB, B
    };

    FactsManager<SweenBob>::Actions actions;
    FactsManager<SweenBob> fm;

    fm.setValue(SweenBob::B, 100);

    actions.push_back({FactsManager<SweenBob>::Rule({FactsManager<SweenBob>::Expression({SweenBob::NA})}), FactsManager<SweenBob>::Expression({SweenBob::B})});
    actions.push_back({FactsManager<SweenBob>::Rule({FactsManager<SweenBob>::Expression({SweenBob::B})}), FactsManager<SweenBob>::Expression({SweenBob::A, SweenBob::NB})});

    while(fm.executeProgram(actions));

    return 0;
}
