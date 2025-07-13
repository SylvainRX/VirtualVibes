#include "UpArrow.h"

mutation class UpArrow;

void UpArrow::displace(fixed_t distance)
{
    Vector3D locaPosition = *Container::getLocalPosition(this);
    locaPosition.y += distance;
    Base::setLocalPosition(this, &locaPosition);
}

fixed_t UpArrow::distanceFromSuccessLocation()
{
    Vector3D locaPosition = *Container::getLocalPosition(this);
    return this->successLocation.y - locaPosition.y;
}
