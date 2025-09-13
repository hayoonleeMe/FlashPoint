
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
		}
	}
	
	namespace Input
	{
		namespace Action
		{
			extern FNativeGameplayTag Move;
			extern FNativeGameplayTag Look;
			extern FNativeGameplayTag Crouch;
			extern FNativeGameplayTag Jump;
			extern FNativeGameplayTag WeaponFire;
		}
	}

	namespace Ability
	{
		extern FNativeGameplayTag Jump;
		extern FNativeGameplayTag WeaponFire;
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
			extern FNativeGameplayTag ReserveAmmo;
		}
	}

	namespace Attributes
	{
		extern FNativeGameplayTag IncomingDamage;
	}
}
