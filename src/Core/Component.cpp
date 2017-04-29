#include "Core/Component.h"

int Component::_id = 0;

Component::~Component() = default;

int Component::id() const
{
	return _id++;
}
