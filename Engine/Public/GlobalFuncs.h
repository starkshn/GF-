#pragma once
#include "Engine_Macro.h"
#include <cmath>

ENGINE_DLL bool AlmostEqual(float a, float b, float epsilon = 0.01f);
ENGINE_DLL float G_CalNDCX(float fScreenPos, float fScreenMax); 
ENGINE_DLL float G_CalNDCY(float fScreenPos, float fScreenMax);