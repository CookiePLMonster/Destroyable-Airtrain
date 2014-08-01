#ifndef __MEMORYMGR
#define __MEMORYMGR

enum
{
	PATCH_CALL,
	PATCH_JUMP,
	PATCH_NOTHING,
};

// This function initially detects III/VC version then chooses the address basing on game version
template<typename T>
inline T AddressByVersion(uint32_t III_address10, uint32_t III_address11, uint32_t III_addressSteam, uint32_t VC_address10, uint32_t VC_address11, uint32_t VC_addressSteam)
{
	static int8_t		bVer = -1;

	if ( bVer == -1 )
	{
		// GTA III
		if (*(uint32_t*)0x5C1E70 == 0x53E58955) bVer = 0;
		else if (*(uint32_t*)0x5C2130 == 0x53E58955) bVer = 1;
		else if (*(uint32_t*)0x5C6FD0 == 0x53E58955) bVer = 2;

		// GTA VC
		else if (*(uint32_t*)0x667BF0 == 0x53E58955) bVer = 3;
		else if (*(uint32_t*)0x667C40 == 0x53E58955) bVer = 4;
		else if (*(uint32_t*)0x666BA0 == 0x53E58955) bVer = 5;
	}
	switch ( bVer )
	{
	case 0:
		return (T)III_address10;
	case 1:
		return (T)III_address11;
	case 2:
		return (T)III_addressSteam;
	case 3:
		return (T)VC_address10;
	case 4:
		return (T)VC_address11;
	case 5:
		return (T)VC_addressSteam;
	default:
		// Hope it never happens
		return (T)nullptr;
	}
}


namespace Memory
{
	template<typename T, typename AT>
	inline void		Patch(AT address, T value)
	{ *(T*)address = value; }

	template<typename AT>
	inline void		Nop(AT address, uint32_t nCount)
	{ memset((void*)address, 0x90, nCount); }

	template<typename AT, typename HT>
	inline void		InjectHook(AT address, HT hook, uint32_t nType=PATCH_NOTHING)
	{
		uint32_t	dwHook;
		_asm
		{
			mov		eax, hook
			mov		dwHook, eax
		}

		switch ( nType )
		{
		case PATCH_JUMP:
			*(uint8_t*)address = 0xE9;
			break;
		case PATCH_CALL:
			*(uint8_t*)address = 0xE8;
			break;
		}

		*(uint32_t*)((uint32_t)address + 1) = dwHook - (uint32_t)address - 5;
	}
};

namespace MemoryVP
{
	template<typename T, typename AT>
	inline void		Patch(AT address, T value)
	{
		DWORD		dwProtect[2];
		VirtualProtect((void*)address, sizeof(T), PAGE_EXECUTE_READWRITE, &dwProtect[0]);
		*(T*)address = value;
		VirtualProtect((void*)address, sizeof(T), dwProtect[0], &dwProtect[1]);
	}

	template<typename AT>
	inline void		Nop(AT address, uint32_t nCount)
	{
		DWORD		dwProtect[2];
		VirtualProtect((void*)address, nCount, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
		memset((void*)address, 0x90, nCount);
		VirtualProtect((void*)address, nCount, dwProtect[0], &dwProtect[1]);
	}

	template<typename AT, typename HT>
	inline void		InjectHook(AT address, HT hook, uint32_t nType=PATCH_NOTHING)
	{
		DWORD		dwProtect[2];
		switch ( nType )
		{
		case PATCH_JUMP:
			VirtualProtect((void*)address, 5, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
			*(uint8_t*)address = 0xE9;
			break;
		case PATCH_CALL:
			VirtualProtect((void*)address, 5, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
			*(uint8_t*)address = 0xE8;
			break;
		default:
			VirtualProtect((void*)((uint32_t)address + 1), 4, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
			break;
		}
		uint32_t		dwHook;
		_asm
		{
			mov		eax, hook
			mov		dwHook, eax
		}

		*(uint32_t*)((uint32_t)address + 1) = dwHook - (uint32_t)address - 5;
		if ( nType == PATCH_NOTHING )
			VirtualProtect((void*)((DWORD)address + 1), 4, dwProtect[0], &dwProtect[1]);
		else
			VirtualProtect((void*)address, 5, dwProtect[0], &dwProtect[1]);
	}
};

#endif