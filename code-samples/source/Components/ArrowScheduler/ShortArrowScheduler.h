#ifndef SHORT_ARROW_SCHEDULER_H_
#define SHORT_ARROW_SCHEDULER_H_

#include <ArrowScheduler.h>

class ShortArrowScheduler : ArrowScheduler
{
    void constructor();
    override void process(uint16 orientations);
}

#endif
