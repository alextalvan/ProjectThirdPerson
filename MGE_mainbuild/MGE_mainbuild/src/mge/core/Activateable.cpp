#include "mge/core/Activateable.hpp"

bool Activateable::IsActive()
{
   return _isActive;
}

void Activateable::SetActive(bool state)
{
   _isActive = state;
}

Activateable::~Activateable()
{

}
