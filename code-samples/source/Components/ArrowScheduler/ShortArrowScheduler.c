#include <GameEvents.h>
#include <Macros.h>

#include "ShortArrowScheduler.h"

void ShortArrowScheduler::constructor()
{
    Base::constructor();
}

void ShortArrowScheduler::destructor()
{
    Base::destructor();
}

void ShortArrowScheduler::process(uint16 orientations)
{
    if(O_LEFT & orientations)
        ShortArrowScheduler::fireEvent(this, kEventShortArrowSchedulerShootArrowLeft);
    if(O_RIGHT & orientations)
        ShortArrowScheduler::fireEvent(this, kEventShortArrowSchedulerShootArrowRight);
    if(O_UP & orientations)
        ShortArrowScheduler::fireEvent(this, kEventShortArrowSchedulerShootArrowUp);
    if(O_DOWN & orientations)
        ShortArrowScheduler::fireEvent(this, kEventShortArrowSchedulerShootArrowDown);
}
