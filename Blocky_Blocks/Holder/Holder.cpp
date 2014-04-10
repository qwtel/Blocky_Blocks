#include "Holder.h"


Holder::Holder() : _object(0)
{
}


Holder::~Holder(void)
{
}

GLuint Holder::object(void) const
{
    return _object;
}
