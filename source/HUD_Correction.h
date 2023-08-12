#pragma once
#include <cstdint>
#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"

class HUD_Correction
{
public:
	static HUD_Correction& getInstance()
	{
		static HUD_Correction instance;
		return instance;
	}

	void SetResolution(UINT width)
	{
		Width = width;
		SetMultiplier(Multiplier);
	}

	void SetEnable(bool Enable)
	{
		if (IsEnabled != Enable)
		{
			GetAddress();

			WriteLimitValue(Enable);
			IsEnabled = Enable;
		}
	}

	void SetMultiplier(float multiplier)
	{
		this->Multiplier = multiplier;
		WriteLimitValue(!IsEnabled);
	}

private:
	HUD_Correction() = default;
	~HUD_Correction() = default;
	void GetAddress()
	{
		//Setup pointers
		if (OverrideAddress1_0 == nullptr)
		{
			auto result = hook::get_pattern<int>("81 FF 00 05 00 00 7C 05 BF 00 05 00 00 DB 44 24", 2);
			if (result != nullptr)
			{
				OverrideAddress1_0 = (int*)result;
				OverrideAddress1_1 = (int*)((char*)result + 0x07);
			}
		}

		if (OverrideAddress2_0 == nullptr)
		{
			auto result = hook::get_pattern<int>("81 FE 00 05 00 00 7D 08 8B CE 89 74 24 20 EB 09 B9 00 05 00 00 89 4C 24 20", 2);
			if (result != nullptr)
			{
				OverrideAddress2_0 = (int*)result;
				OverrideAddress2_1 = (int*)((char*)result + 0x0F);
			}
		}
	}

	void WriteLimitValue(bool original)
	{
		GetAddress();

		int newLimitValue = original ? 1280 : (int)(this->Width * Multiplier);
		if (OverrideAddress1_0 != nullptr)
		{
			DWORD dwProtect;
			VirtualProtect((void*)OverrideAddress1_0, 4, PAGE_EXECUTE_READWRITE, &dwProtect);
			*OverrideAddress1_0 = newLimitValue;
			VirtualProtect((void*)OverrideAddress1_0, 4, dwProtect, &dwProtect);

			VirtualProtect((void*)OverrideAddress1_1, 4, PAGE_EXECUTE_READWRITE, &dwProtect);
			*OverrideAddress1_1 = newLimitValue;
			VirtualProtect((void*)OverrideAddress1_1, 4, dwProtect, &dwProtect);
		}

		if (OverrideAddress2_0 != nullptr)
		{
			DWORD dwProtect;
			VirtualProtect((void*)OverrideAddress2_0, 4, PAGE_EXECUTE_READWRITE, &dwProtect);
			*OverrideAddress2_0 = newLimitValue;
			VirtualProtect((void*)OverrideAddress2_0, 4, dwProtect, &dwProtect);

			VirtualProtect((void*)OverrideAddress2_1, 4, PAGE_EXECUTE_READWRITE, &dwProtect);
			*OverrideAddress2_1 = newLimitValue;
			VirtualProtect((void*)OverrideAddress2_1, 4, dwProtect, &dwProtect);
		}
		IsEnabled = !original;
	}

	UINT Width = 1280;
	float Multiplier = 1;

	bool IsEnabled = false;
	int* OverrideAddress1_0 = NULL;
	int* OverrideAddress1_1 = NULL;
	int* OverrideAddress2_0 = NULL;
	int* OverrideAddress2_1 = NULL;
};