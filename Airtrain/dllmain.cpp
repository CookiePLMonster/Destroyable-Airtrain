#define WIN32_LEAN_AND_MEAN

// Target Windows 2000
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <cstdint>

#include "MemoryMgr.h"

// This time, we don't use AddressByVersion to initialise variables - this ASI has no CRT, as we want to keep it lightweight
// They'll be initialised inside DllMain instead
void*	AirtrainRocketTest_ReturnTrue;
void*	AirtrainRocketTest_ReturnFalse;

// Same test as for deaddodo, but with a bigger radius (15.0 compared to Dodo's 5.0)
void __declspec(naked) AirtrainRocketTest_III()
{
	static const float		fRadiusSqr = 15.0f * 15.0f;
	_asm
	{
		cmp		byte ptr [ebp+298h], 0
		jnz		AirtrainRocketTestIII_CheckFailed
		lea     ecx, [ebp+34h]
		fld     dword ptr [ebx+8]
		mov     eax, ecx
		fsub    dword ptr [eax+8]
		fstp    st(1)
		fld     dword ptr [ebx+4]
		fsub    dword ptr [eax+4]
		fstp    st(2)
		fld     dword ptr [ebx]
		fsub    dword ptr [eax]
		fstp    st(3)
		fld     st(1)
		fmul    st, st
		fld     st(3)
		fmul    st, st
		faddp   st(1), st
		fld     st(1)
		fmul    st, st
		faddp   st(1), st
		fcomp   [fRadiusSqr]
		fnstsw  ax
		and     ah, 5
		cmp     ah, 1
		jnz		AirtrainRocketTestIII_CheckFailed
		jmp		AirtrainRocketTest_ReturnTrue

AirtrainRocketTestIII_CheckFailed:
		jmp		AirtrainRocketTest_ReturnFalse
	}
}

void __declspec(naked) AirtrainRocketTest_VC()
{
	static const float		fRadiusSqr = 15.0f * 15.0f;
	_asm
	{
		cmp		byte ptr [ebp+2B0h], 0
		jnz		AirtrainRocketTestVC_CheckFailed
		lea     ecx, [ebp+34h]
		fld     dword ptr [ebx+8]
		mov     eax, ecx
		fsub    dword ptr [eax+8]
		fstp    st(1)
		fld     dword ptr [ebx+4]
		fsub    dword ptr [eax+4]
		fstp    st(2)
		fld     dword ptr [ebx]
		fsub    dword ptr [eax]
		fstp    st(3)
		fld     st(1)
		fmul    st, st
		fld     st(3)
		fmul    st, st
		faddp   st(1), st
		fld     st(1)
		fmul    st, st
		faddp   st(1), st
		fcomp   [fRadiusSqr]
		fnstsw  ax
		and     ah, 5
		cmp     ah, 1
		jnz		AirtrainRocketTestVC_CheckFailed
		jmp		AirtrainRocketTest_ReturnTrue

AirtrainRocketTestVC_CheckFailed:
		jmp		AirtrainRocketTest_ReturnFalse
	}
}

// extern "C" is not really needed there - it's only added for MinGW's sake
// When using Visual Studio, no need to use it
extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpReserved);

	if( reason == DLL_PROCESS_ATTACH )
	{
		using namespace MemoryVP;

		if (*(uint32_t*)0x5C1E70 == 0x53E58955)
		{
			// III 1.0
			AirtrainRocketTest_ReturnTrue = (void*)0x54DF40;
			AirtrainRocketTest_ReturnFalse = (void*)0x54DF95;

			InjectHook(0x54DEE8, AirtrainRocketTest_III);
		}
		else if (*(uint32_t*)0x5C2130 == 0x53E58955)
		{
			// III 1.1
			AirtrainRocketTest_ReturnTrue = (void*)0x54E0C0;
			AirtrainRocketTest_ReturnFalse = (void*)0x54E115;

			InjectHook(0x54E068, AirtrainRocketTest_III);
		}
		else if (*(uint32_t*)0x5C6FD0 == 0x53E58955)
		{
			// III Steam
			AirtrainRocketTest_ReturnTrue = (void*)0x54E070;
			AirtrainRocketTest_ReturnFalse = (void*)0x54E0C5;

			InjectHook(0x54E018, AirtrainRocketTest_III);
		}
		else if (*(uint32_t*)0x667BF0 == 0x53E58955)
		{
			// VC 1.0
			AirtrainRocketTest_ReturnTrue = (void*)0x5AFD40;
			AirtrainRocketTest_ReturnFalse = (void*)0x5AFD95;

			InjectHook(0x5AFCE8, AirtrainRocketTest_VC);
		}
		else if (*(uint32_t*)0x667C40 == 0x53E58955)
		{
			// VC 1.1
			AirtrainRocketTest_ReturnTrue = (void*)0x5AFD60;
			AirtrainRocketTest_ReturnFalse = (void*)0x5AFDB5;

			InjectHook(0x5AFD08, AirtrainRocketTest_VC);
		}
		else if (*(uint32_t*)0x666BA0 == 0x53E58955)
		{
			// VC Steam
			AirtrainRocketTest_ReturnTrue = (void*)0x5AFB70;
			AirtrainRocketTest_ReturnFalse = (void*)0x5AFBC5;

			InjectHook(0x5AFB18, AirtrainRocketTest_VC);
		}
		else return FALSE;	// If game version couldn't be detected

	}

	return TRUE;
}