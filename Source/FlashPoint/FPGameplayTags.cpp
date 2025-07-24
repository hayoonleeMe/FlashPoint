
#include "FPGameplayTags.h"

namespace FPGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Look, "Input.Action.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Crouch, "Input.Action.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Jump, "Input.Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_WeaponFire, "Input.Action.WeaponFire");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump, "Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_WeaponFire, "Ability.WeaponFire");

	UE_DEFINE_GAMEPLAY_TAG(CharacterState_IsSprinting, "CharacterState.IsSprinting");
	UE_DEFINE_GAMEPLAY_TAG(CharacterState_IsFiring, "CharacterState.IsFiring");

	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Unarmed, "Weapon.Type.Unarmed");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Pistol, "Weapon.Type.Pistol");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Rifle, "Weapon.Type.Rifle");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Shotgun, "Weapon.Type.Shotgun");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_SMG, "Weapon.Type.SMG");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_SniperRifle, "Weapon.Type.SniperRifle");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_GrenadeLauncher, "Weapon.Type.GrenadeLauncher");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_RocketLauncher, "Weapon.Type.RocketLauncher");
	
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Data_Ammo, "Weapon.Data.Ammo");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Data_ReserveAmmo, "Weapon.Data.ReserveAmmo");
}
