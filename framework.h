#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

// Math
#define _USE_MATH_DEFINES
#include <cmath>

// Own Defines
#include "Defines.h"

#endif // !FRAMEWORK_H