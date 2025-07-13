#ifndef UP_ARROW_H_
#define UP_ARROW_H_

#include "Arrow.h"

mutation class UpArrow : Arrow
{
    override void displace(fixed_t distance);
    override fixed_t distanceFromSuccessLocation();
}

#endif
