
#include "FPGameplayTags.h"

namespace FPGameplayTags
{
	namespace Asset
	{
		namespace Level
		{
			UE_DEFINE_GAMEPLAY_TAG(SignScreen, "Asset.Level.SignScreen");
			UE_DEFINE_GAMEPLAY_TAG(MainMenu, "Asset.Level.MainMenu");
			UE_DEFINE_GAMEPLAY_TAG(Lobby, "Asset.Level.Lobby");
			UE_DEFINE_GAMEPLAY_TAG(Warehouse_TDM, "Asset.Level.Warehouse_TDM");
			UE_DEFINE_GAMEPLAY_TAG(Warehouse_FFA, "Asset.Level.Warehouse_FFA");
		}

		namespace RecoilData
		{
			UE_DEFINE_GAMEPLAY_TAG(Pistol, "Asset.RecoilData.Pistol");
			UE_DEFINE_GAMEPLAY_TAG(Rifle, "Asset.RecoilData.Rifle");
			UE_DEFINE_GAMEPLAY_TAG(Shotgun, "Asset.RecoilData.Shotgun");
			UE_DEFINE_GAMEPLAY_TAG(SMG, "Asset.RecoilData.SMG");
			UE_DEFINE_GAMEPLAY_TAG(SniperRifle, "Asset.RecoilData.SniperRifle");
		}
	}
	
	namespace Input
	{
		UE_DEFINE_GAMEPLAY_TAG(NoInput, "Input.NoInput");
		
		namespace Gameplay
		{
			UE_DEFINE_GAMEPLAY_TAG(PauseMenu, "Input.Gameplay.PauseMenu");
			UE_DEFINE_GAMEPLAY_TAG(Move, "Input.Gameplay.Move");
			UE_DEFINE_GAMEPLAY_TAG(Look, "Input.Gameplay.Look");
			UE_DEFINE_GAMEPLAY_TAG(Crouch, "Input.Gameplay.Crouch");
			UE_DEFINE_GAMEPLAY_TAG(Jump, "Input.Gameplay.Jump");
			UE_DEFINE_GAMEPLAY_TAG(WeaponFire, "Input.Gameplay.WeaponFire");
			UE_DEFINE_GAMEPLAY_TAG(Scoreboard, "Input.Gameplay.Scoreboard");
			UE_DEFINE_GAMEPLAY_TAG(Sprint, "Input.Gameplay.Sprint");
			UE_DEFINE_GAMEPLAY_TAG(Reload, "Input.Gameplay.Reload");

			namespace QuickSlot
			{
				UE_DEFINE_GAMEPLAY_TAG(Slot1, "Input.Gameplay.QuickSlot.Slot1");
				UE_DEFINE_GAMEPLAY_TAG(Slot2, "Input.Gameplay.QuickSlot.Slot2");
				UE_DEFINE_GAMEPLAY_TAG(Slot3, "Input.Gameplay.QuickSlot.Slot3");
			}
		}

		namespace UI
		{
			UE_DEFINE_GAMEPLAY_TAG(Back, "Input.UI.Back");
			UE_DEFINE_GAMEPLAY_TAG(Confirm, "Input.UI.Confirm");
		}
	}

	namespace Ability
	{
		UE_DEFINE_GAMEPLAY_TAG(Jump, "Ability.Jump");
		UE_DEFINE_GAMEPLAY_TAG(WeaponFire, "Ability.WeaponFire");
		UE_DEFINE_GAMEPLAY_TAG(Sprint, "Ability.Sprint");
		UE_DEFINE_GAMEPLAY_TAG(Crouch, "Ability.Crouch");
		UE_DEFINE_GAMEPLAY_TAG(Reload, "Ability.Reload");
		UE_DEFINE_GAMEPLAY_TAG(DryFire, "Ability.DryFire");

		namespace Fail
		{
			UE_DEFINE_GAMEPLAY_TAG(NoAmmo, "Ability.Fail.NoAmmo");
		}
	}

	namespace CharacterState
	{
		UE_DEFINE_GAMEPLAY_TAG(IsSprinting, "CharacterState.IsSprinting");
		UE_DEFINE_GAMEPLAY_TAG(IsFiring, "CharacterState.IsFiring");
		UE_DEFINE_GAMEPLAY_TAG(IsMoving, "CharacterState.IsMoving");
	}

	namespace Weapon
	{
		UE_DEFINE_GAMEPLAY_TAG(NoFire, "Weapon.NoFire");
		
		namespace Type
		{
			UE_DEFINE_GAMEPLAY_TAG(Unarmed, "Weapon.Type.Unarmed");
			UE_DEFINE_GAMEPLAY_TAG(Pistol, "Weapon.Type.Pistol");
			UE_DEFINE_GAMEPLAY_TAG(Rifle, "Weapon.Type.Rifle");
			UE_DEFINE_GAMEPLAY_TAG(Shotgun, "Weapon.Type.Shotgun");
			UE_DEFINE_GAMEPLAY_TAG(SMG, "Weapon.Type.SMG");
			UE_DEFINE_GAMEPLAY_TAG(SniperRifle, "Weapon.Type.SniperRifle");
			UE_DEFINE_GAMEPLAY_TAG(GrenadeLauncher, "Weapon.Type.GrenadeLauncher");
			UE_DEFINE_GAMEPLAY_TAG(RocketLauncher, "Weapon.Type.RocketLauncher");
		}

		namespace Data
		{
			UE_DEFINE_GAMEPLAY_TAG(Ammo, "Weapon.Data.Ammo");
		}
	}

	namespace Attributes
	{
		UE_DEFINE_GAMEPLAY_TAG(IncomingDamage, "Attributes.IncomingDamage");
	}

	namespace GameplayEvent
	{
		UE_DEFINE_GAMEPLAY_TAG(ReloadFinished, "GameplayEvent.ReloadFinished");
	}
}
