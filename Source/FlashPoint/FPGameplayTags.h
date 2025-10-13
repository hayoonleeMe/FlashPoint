
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
	}
	
	namespace Input
	{
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
	}

	namespace CharacterState
	{
		extern FNativeGameplayTag IsSprinting;
		extern FNativeGameplayTag IsFiring;
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
}
