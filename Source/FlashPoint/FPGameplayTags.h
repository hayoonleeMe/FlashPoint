
#pragma once

#include "NativeGameplayTags.h"

namespace FPGameplayTags
{
	namespace Asset
	{
		namespace Level
		{
			extern FNativeGameplayTag SignScreen;
			extern FNativeGameplayTag MainMenu;
			extern FNativeGameplayTag Lobby;
			extern FNativeGameplayTag Warehouse_TDM;
			extern FNativeGameplayTag Warehouse_FFA;
		}

		namespace RecoilData
		{
			extern FNativeGameplayTag Pistol;
			extern FNativeGameplayTag Rifle;
			extern FNativeGameplayTag Shotgun;
			extern FNativeGameplayTag SMG;
			extern FNativeGameplayTag SniperRifle;
		}

		namespace AbilitySystemData
		{
			extern FNativeGameplayTag Default;
			extern FNativeGameplayTag Pistol;
			extern FNativeGameplayTag Rifle;
			extern FNativeGameplayTag Shotgun;
			extern FNativeGameplayTag SMG;
			extern FNativeGameplayTag SniperRifle;
		}
		
		extern FNativeGameplayTag InputData;		
		extern FNativeGameplayTag CosmeticData;		
	}
	
	namespace Input
	{
		extern FNativeGameplayTag NoInput;
		
		namespace Gameplay
		{
			extern FNativeGameplayTag PauseMenu;
			extern FNativeGameplayTag Move;
			extern FNativeGameplayTag Look;
			extern FNativeGameplayTag Crouch;
			extern FNativeGameplayTag Jump;
			extern FNativeGameplayTag WeaponFire;
			extern FNativeGameplayTag Scoreboard;
			extern FNativeGameplayTag Sprint;
			extern FNativeGameplayTag Reload;

			namespace QuickSlot
			{
				extern FNativeGameplayTag Slot1;
				extern FNativeGameplayTag Slot2;
				extern FNativeGameplayTag Slot3;
			}
		}

		namespace UI
		{
			extern FNativeGameplayTag Back;
			extern FNativeGameplayTag Confirm;
		}
	}

	namespace Ability
	{
		extern FNativeGameplayTag Jump;
		extern FNativeGameplayTag WeaponFire;
		extern FNativeGameplayTag Sprint;
		extern FNativeGameplayTag Crouch;
		extern FNativeGameplayTag Reload;
		extern FNativeGameplayTag DryFire;

		namespace Fail
		{
			extern FNativeGameplayTag NoAmmo;
		}
	}

	namespace CharacterState
	{
		extern FNativeGameplayTag IsSprinting;
		extern FNativeGameplayTag IsFiring;
		extern FNativeGameplayTag IsMovingForwardFromInput;
	}

	namespace Weapon
	{
		extern FNativeGameplayTag NoFire;
		
		namespace Type
		{
			extern FNativeGameplayTag Unarmed;
			extern FNativeGameplayTag Pistol;
			extern FNativeGameplayTag Rifle;
			extern FNativeGameplayTag Shotgun;
			extern FNativeGameplayTag SMG;
			extern FNativeGameplayTag SniperRifle;
			extern FNativeGameplayTag GrenadeLauncher;
			extern FNativeGameplayTag RocketLauncher;
		}

		namespace Data
		{
			extern FNativeGameplayTag Ammo;
		}
	}

	namespace Attributes
	{
		extern FNativeGameplayTag IncomingDamage;
	}

	namespace GameplayEvent
	{
		extern FNativeGameplayTag ReloadFinished;
	}
}
