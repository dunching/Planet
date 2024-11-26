#include "GameplayTagsSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

#include "Planet.h"

FGameplayTag UGameplayTagsSubSystem::BaseFeature_Send =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Send")));

FGameplayTag UGameplayTagsSubSystem::BaseFeature_Received =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Received")));

FGameplayTag UGameplayTagsSubSystem::BaseFeature_Respawn =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Respawn")));

#pragma region Locomotio
FGameplayTag UGameplayTagsSubSystem::Jump =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Jump")));

FGameplayTag UGameplayTagsSubSystem::State_Locomotion_Run =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Run")));

FGameplayTag UGameplayTagsSubSystem::Dash =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Dash")));

FGameplayTag UGameplayTagsSubSystem::Mount =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Mount")));

FGameplayTag UGameplayTagsSubSystem::DisMount =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.DisMount")));

FGameplayTag UGameplayTagsSubSystem::Affected =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Affected")));

FGameplayTag UGameplayTagsSubSystem::State_MoveToAttaclArea =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MoveToAttaclArea")));
#pragma endregion

#pragma region MovementStateAbl
FGameplayTag UGameplayTagsSubSystem::MovementStateAble =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble")));

FGameplayTag UGameplayTagsSubSystem::MovementStateAble_CantJump =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantJump")));

FGameplayTag UGameplayTagsSubSystem::MovementStateAble_CantPathFollowMove =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantPathFollowMove")));
FGameplayTag UGameplayTagsSubSystem::MovementStateAble_CantPlayerInputMove =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantPlayerInputMove")));
FGameplayTag UGameplayTagsSubSystem::MovementStateAble_CantRootMotion =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantRootMotion")));
FGameplayTag UGameplayTagsSubSystem::MovementStateAble_CantRotation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantRotation")));

// 浮空状态（被动
FGameplayTag UGameplayTagsSubSystem::MovementStateAble_IntoFly =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.IntoFly")));
FGameplayTag UGameplayTagsSubSystem::MovementStateAble_Orient2Acce =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.Orient2Acce")));
FGameplayTag UGameplayTagsSubSystem::MovementStateAble_UseCustomRotation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.UseCustomRotation")));
#pragma endregion

#pragma region State Tag
FGameplayTag UGameplayTagsSubSystem::UsingConsumable =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.IsUsingConsumable")));
FGameplayTag UGameplayTagsSubSystem::Ridering =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Ridering")));
FGameplayTag UGameplayTagsSubSystem::State_NoPhy =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.NoPhy")));
FGameplayTag UGameplayTagsSubSystem::State_Buff_Purify =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.Purify")));

FGameplayTag UGameplayTagsSubSystem::State_Buff_SuperArmor =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.SuperArmor")));

FGameplayTag UGameplayTagsSubSystem::State_Buff_CantBeSlected =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.CantBeSelected")));

FGameplayTag UGameplayTagsSubSystem::State_Buff_Stagnation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.Stagnation")));

FGameplayTag UGameplayTagsSubSystem::Debuff =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff")));

FGameplayTag UGameplayTagsSubSystem::State_Debuff_Fire =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Fire")));

FGameplayTag UGameplayTagsSubSystem::State_Debuff_Stun =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Stun")));

FGameplayTag UGameplayTagsSubSystem::State_Debuff_Charm =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Charm")));

FGameplayTag UGameplayTagsSubSystem::State_Debuff_Fear =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Fear")));
FGameplayTag UGameplayTagsSubSystem::State_Debuff_Silent =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Silent")));

FGameplayTag UGameplayTagsSubSystem::State_Debuff_Slow =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Slow")));

FGameplayTag UGameplayTagsSubSystem::State_Debuff_Suppress =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Suppress")));
FGameplayTag UGameplayTagsSubSystem::RootMotion =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion")));

// 浮空状态（主动
FGameplayTag UGameplayTagsSubSystem::FlyAway =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.FlyAway")));

FGameplayTag UGameplayTagsSubSystem::KnockDown =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.KnockDown")));

FGameplayTag UGameplayTagsSubSystem::State_RootMotion_Traction =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.Traction")));

FGameplayTag UGameplayTagsSubSystem::TornadoTraction =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.TornadoTraction")));

FGameplayTag UGameplayTagsSubSystem::MoveAlongSpline =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.MoveAlongSpline")));

FGameplayTag UGameplayTagsSubSystem::State_RootMotion_MoveTo =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.MoveTo")));

FGameplayTag UGameplayTagsSubSystem::DeathingTag =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Deathing")));

FGameplayTag UGameplayTagsSubSystem::Respawning =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Respawning")));

FGameplayTag UGameplayTagsSubSystem::InFightingTag =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.InFighting")));

FGameplayTag UGameplayTagsSubSystem::State_ReleasingSkill =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill")));

FGameplayTag UGameplayTagsSubSystem::State_ReleasingSkill_Continuous =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Continuous")));

FGameplayTag UGameplayTagsSubSystem::State_ReleasingSkill_Active =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Active")));

FGameplayTag UGameplayTagsSubSystem::State_ReleasingSkil_WeaponActive =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.WeaponActive")));
#pragma endregion

#pragma region Socket Tags
FGameplayTag UGameplayTagsSubSystem::ConsumableSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket")));

FGameplayTag UGameplayTagsSubSystem::ConsumableSocket1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket.1")));

FGameplayTag UGameplayTagsSubSystem::ConsumableSocket2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket.2")));

FGameplayTag UGameplayTagsSubSystem::ConsumableSocket3 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket.3")));

FGameplayTag UGameplayTagsSubSystem::ConsumableSocket4 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket.4")));

FGameplayTag UGameplayTagsSubSystem::WeaponSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponSocket")));

FGameplayTag UGameplayTagsSubSystem::WeaponSocket_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponSocket.1")));

FGameplayTag UGameplayTagsSubSystem::WeaponSocket_2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponSocket.2")));

FGameplayTag UGameplayTagsSubSystem::WeaponActiveSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponActiveSocket")));

FGameplayTag UGameplayTagsSubSystem::WeaponActiveSocket_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponActiveSocket.1")));

FGameplayTag UGameplayTagsSubSystem::WeaponActiveSocket_2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponActiveSocket.2")));

FGameplayTag UGameplayTagsSubSystem::ActiveSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket")));

FGameplayTag UGameplayTagsSubSystem::ActiveSocket_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.1")));

FGameplayTag UGameplayTagsSubSystem::ActiveSocket_2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.2")));

FGameplayTag UGameplayTagsSubSystem::ActiveSocket_3 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.3")));

FGameplayTag UGameplayTagsSubSystem::ActiveSocket_4 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.4")));

FGameplayTag UGameplayTagsSubSystem::PassiveSocket_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.1")));

FGameplayTag UGameplayTagsSubSystem::PassiveSocket_2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.2")));

FGameplayTag UGameplayTagsSubSystem::PassiveSocket_3 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.3")));

FGameplayTag UGameplayTagsSubSystem::PassiveSocket_4 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.4")));

FGameplayTag UGameplayTagsSubSystem::PassiveSocket_5 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.5")));

FGameplayTag UGameplayTagsSubSystem::TalentSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.Talent")));
#pragma endregion

#pragma region UnitTyp
FGameplayTag UGameplayTagsSubSystem::Unit_Weapon =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Weapon")));

FGameplayTag UGameplayTagsSubSystem::Unit_Weapon_Axe =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Weapon.Axe")));
FGameplayTag UGameplayTagsSubSystem::Unit_Tool =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Tool")));

FGameplayTag UGameplayTagsSubSystem::Unit_Tool_Axe =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Tool.Axe")));

FGameplayTag UGameplayTagsSubSystem::Unit_Consumables =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Consumables")));

FGameplayTag UGameplayTagsSubSystem::Unit_Consumables_HP =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Consumables.HP")));

FGameplayTag UGameplayTagsSubSystem::Unit_Consumables_PP =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Consumables.PP")));

FGameplayTag UGameplayTagsSubSystem::Unit_Coin =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Coin")));

FGameplayTag UGameplayTagsSubSystem::Unit_Coin_Regular =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Coin.Regular")));

FGameplayTag UGameplayTagsSubSystem::Unit_Coin_RafflePermanent =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Coin.RafflePermanent")));

FGameplayTag UGameplayTagsSubSystem::Unit_Coin_RaffleLimit =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Coin.RaffleLimit")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Weapon =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Weapon_Axe =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.Axe")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Weapon_HandProtection =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.HandProtection")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Weapon_RangeTest =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.RangeTest")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Weapon_Bow =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.Bow")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Weapon_FoldingFan =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.FoldingFan")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_Switch =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Switch")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_Switch_Test =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Switch.Test")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_Control =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Control")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_Displacement =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Displacement")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_GroupTherapy =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.GroupTherapy")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_ContinuousGroupTherapy =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.ContinuousGroupTherapy")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_Tornado =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Tornado")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_FlyAway =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.FlyAway")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_Stun =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Stun")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Active_Charm =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Charm")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Passve =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Passve")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Passve_ZMJZ =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Passve.ZMJZ")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Talent =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Talent")));
FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Talent_NuQi =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Talent.NuQi")));

FGameplayTag UGameplayTagsSubSystem::Unit_Skill_Talent_YinYang =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Talent.YinYang")));

FGameplayTag UGameplayTagsSubSystem::Unit_Character =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Character")));

FGameplayTag UGameplayTagsSubSystem::Unit_Character_Player =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Character.Player")));

FGameplayTag UGameplayTagsSubSystem::Unit_Character_TestNPC_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Character.TestNPC.1")));

#pragma endregion

FGameplayTag UGameplayTagsSubSystem::DataSource_Regular =
	FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Regular")));

// 人物身上自带的属性
FGameplayTag UGameplayTagsSubSystem::DataSource_Character =
	FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Character")));

FGameplayTag UGameplayTagsSubSystem::DataSource_EquipmentModify =
	FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.EquipmentModify")));

FGameplayTag UGameplayTagsSubSystem::DataSource_TalentModify =
	FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.TalentModify")));

FGameplayTag UGameplayTagsSubSystem::Skill_CanBeInterrupted =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Skill.CanBeInterrupted")));

FGameplayTag UGameplayTagsSubSystem::Skill_CanBeInterrupted_Stagnation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Skill.CanBeInterrupted.Stagnation")));
