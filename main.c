
#include "SM.h"

int main()
{
    SM sm;

    bool bLoop = false;
    do
    {
        bLoop = sm.executeProgram();
    } while (bLoop);

    return 0;
}
