#include "GlobalFuncs.h"

bool AlmostEqual(float a, float b, float epsilon)
{
	return std::fabs(a - b) <= epsilon;
}

ENGINE_DLL float G_CalNDCX(float fScreenPos, float fScreenMax)
{
	return (fScreenPos / fScreenMax) * 2.f - 1.f;
}

ENGINE_DLL float G_CalNDCY(float fScreenPos, float fScreenMax)
{
	return 1.f - (fScreenPos / fScreenMax) * 2.f;
}
