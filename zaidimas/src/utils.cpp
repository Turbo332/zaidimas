#include "Utils.h"
#include "Objects.h"

bool utils::IsBetween(float val, float rangeB, float rangeE)
{
	return val > rangeB && val < rangeE;
}
