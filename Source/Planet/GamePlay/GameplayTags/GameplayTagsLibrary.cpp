#include "GameplayTagsLibrary.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

#include "Planet.h"

FGameplayTag UGameplayTagsLibrary::BaseFeature_Reply =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Reply")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_Dash =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Dash")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_Run =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Run")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_Jump =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Jump")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_Respawn =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Respawn")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_HasBeenRepel =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.HasBeenRepel")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_HasBeenFlyAway =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.HasBeenFlyAway")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_HasbeenTornodo =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.HasbeenTornodo")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_HasbeenTraction =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.HasbeenTraction")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_HasBeenDisplacement =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.HasBeenDisplacement")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_HasBeenPull =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.HasBeenPull")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_MoveToLocation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.MoveToLocation")));

FGameplayTag UGameplayTagsLibrary::BaseFeature_Dying =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Dying")));

#pragma region MovementStateAbl
FGameplayTag UGameplayTagsLibrary::MovementStateAble =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MovementStateAble")));

FGameplayTag UGameplayTagsLibrary::MovementStateAble_CantJump =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MovementStateAble.CantJump")));

FGameplayTag UGameplayTagsLibrary::MovementStateAble_CantPathFollowMove =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MovementStateAble.CantPathFollowMove")));
FGameplayTag UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MovementStateAble.CantPlayerInputMove")));
FGameplayTag UGameplayTagsLibrary::MovementStateAble_CantRootMotion =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MovementStateAble.CantRootMotion")));
FGameplayTag UGameplayTagsLibrary::MovementStateAble_CantRotation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MovementStateAble.CantRotation")));

FGameplayTag UGameplayTagsLibrary::MovementStateAble_Orient2Acce =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MovementStateAble.Orient2Acce")));
FGameplayTag UGameplayTagsLibrary::MovementStateAble_UseCustomRotation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MovementStateAble.UseCustomRotation")));
#pragma endregion

#pragma region State Tag
FGameplayTag UGameplayTagsLibrary::UsingConsumable =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.IsUsingConsumable")));

FGameplayTag UGameplayTagsLibrary::Ridering =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Ridering")));

FGameplayTag UGameplayTagsLibrary::State_Dying =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Dying")));

FGameplayTag UGameplayTagsLibrary::State_MoveToLocation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.MoveToLocation")));

FGameplayTag UGameplayTagsLibrary::State_Running =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Running")));

FGameplayTag UGameplayTagsLibrary::State_NoPhy =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.NoPhy")));

FGameplayTag UGameplayTagsLibrary::State_Buff_Purify =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.Purify")));

FGameplayTag UGameplayTagsLibrary::State_Buff_SuperArmor =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.SuperArmor")));

FGameplayTag UGameplayTagsLibrary::State_Buff_CantBeSlected =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.CantBeSelected")));

FGameplayTag UGameplayTagsLibrary::State_Buff_Stagnation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.Stagnation")));

FGameplayTag UGameplayTagsLibrary::Debuff =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff")));

FGameplayTag UGameplayTagsLibrary::State_Debuff_Fire =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Fire")));

FGameplayTag UGameplayTagsLibrary::State_Debuff_Stun =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Stun")));

FGameplayTag UGameplayTagsLibrary::State_Debuff_Charm =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Charm")));

FGameplayTag UGameplayTagsLibrary::State_Debuff_Fear =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Fear")));
FGameplayTag UGameplayTagsLibrary::State_Debuff_Silent =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Silent")));

FGameplayTag UGameplayTagsLibrary::State_Debuff_Slow =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Slow")));

FGameplayTag UGameplayTagsLibrary::State_Debuff_Suppress =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Suppress")));

FGameplayTag UGameplayTagsLibrary::RootMotion =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion")));

// 浮空状态（主动
FGameplayTag UGameplayTagsLibrary::RootMotion_HasBeenFlyAway =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.HasBeenFlyAway")));

FGameplayTag UGameplayTagsLibrary::State_RootMotion_FlyAway =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.FlyAway")));

FGameplayTag UGameplayTagsLibrary::KnockDown =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.KnockDown")));

FGameplayTag UGameplayTagsLibrary::State_RootMotion_Traction =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.Traction")));

FGameplayTag UGameplayTagsLibrary::TornadoTraction =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.TornadoTraction")));

FGameplayTag UGameplayTagsLibrary::MoveAlongSpline =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.MoveAlongSpline")));

FGameplayTag UGameplayTagsLibrary::State_RootMotion_MoveTo =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.MoveTo")));

FGameplayTag UGameplayTagsLibrary::Respawning =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.Respawning")));

FGameplayTag UGameplayTagsLibrary::InFightingTag =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.InFighting")));

FGameplayTag UGameplayTagsLibrary::State_ReleasingSkill =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill")));

FGameplayTag UGameplayTagsLibrary::State_ReleasingSkill_Continuous =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Continuous")));

FGameplayTag UGameplayTagsLibrary::State_ReleasingSkill_Active =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Active")));

FGameplayTag UGameplayTagsLibrary::State_ReleasingSkil_WeaponActive =
	FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.WeaponActive")));
#pragma endregion

#pragma region Socket Tags
FGameplayTag UGameplayTagsLibrary::ConsumableSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.ConsumableSocket")));

FGameplayTag UGameplayTagsLibrary::ConsumableSocket_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.ConsumableSocket.1")));

FGameplayTag UGameplayTagsLibrary::ConsumableSocket_2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.ConsumableSocket.2")));

FGameplayTag UGameplayTagsLibrary::ConsumableSocket_3 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.ConsumableSocket.3")));

FGameplayTag UGameplayTagsLibrary::ConsumableSocket_4 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.ConsumableSocket.4")));

FGameplayTag UGameplayTagsLibrary::WeaponSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.WeaponSocket")));

FGameplayTag UGameplayTagsLibrary::WeaponSocket_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.WeaponSocket.1")));

FGameplayTag UGameplayTagsLibrary::WeaponSocket_2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.WeaponSocket.2")));

FGameplayTag UGameplayTagsLibrary::ActiveSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket")));

FGameplayTag UGameplayTagsLibrary::ActiveSocket_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.1")));

FGameplayTag UGameplayTagsLibrary::ActiveSocket_2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.2")));

FGameplayTag UGameplayTagsLibrary::ActiveSocket_3 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.3")));

FGameplayTag UGameplayTagsLibrary::ActiveSocket_4 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.4")));

FGameplayTag UGameplayTagsLibrary::PassiveSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket")));

FGameplayTag UGameplayTagsLibrary::PassiveSocket_1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.1")));

FGameplayTag UGameplayTagsLibrary::PassiveSocket_2 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.2")));

FGameplayTag UGameplayTagsLibrary::PassiveSocket_3 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.3")));

FGameplayTag UGameplayTagsLibrary::PassiveSocket_4 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.4")));

FGameplayTag UGameplayTagsLibrary::PassiveSocket_5 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.5")));

FGameplayTag UGameplayTagsLibrary::TalentSocket =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.Talent")));
#pragma endregion

#pragma region ProxyTyp
FGameplayTag UGameplayTagsLibrary::Proxy =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy")));

FGameplayTag UGameplayTagsLibrary::Proxy_Weapon =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Weapon")));

FGameplayTag UGameplayTagsLibrary::Proxy_Weapon_Test =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Weapon.Test")));

FGameplayTag UGameplayTagsLibrary::Proxy_Weapon_Axe =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Weapon.Axe")));

FGameplayTag UGameplayTagsLibrary::Proxy_Tool =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Tool")));

FGameplayTag UGameplayTagsLibrary::Proxy_Tool_Axe =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Tool.Axe")));

FGameplayTag UGameplayTagsLibrary::Proxy_Consumables =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Consumables")));

FGameplayTag UGameplayTagsLibrary::Proxy_Consumables_HP =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Consumables.HP")));

FGameplayTag UGameplayTagsLibrary::Proxy_Consumables_PP =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Consumables.PP")));

FGameplayTag UGameplayTagsLibrary::Proxy_Coin =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Coin")));

FGameplayTag UGameplayTagsLibrary::Proxy_Coin_Regular =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Coin.Regular")));

FGameplayTag UGameplayTagsLibrary::Proxy_Coin_RafflePermanent =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Coin.RafflePermanent")));

FGameplayTag UGameplayTagsLibrary::Proxy_Coin_RaffleLimit =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Coin.RaffleLimit")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Weapon =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Weapon")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Weapon_Axe =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Weapon.Axe")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Weapon_HandProtection =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Weapon.HandProtection")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Weapon_RangeTest =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Weapon.RangeTest")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Weapon_Bow =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Weapon.Bow")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Weapon_FoldingFan =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Weapon.FoldingFan")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_Switch =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.Switch")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_Switch_Test =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.Switch.Test")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_Control =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.Control")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_Displacement =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.Displacement")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_GroupTherapy =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.GroupTherapy")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_ContinuousGroupTherapy =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.ContinuousGroupTherapy")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_Tornado =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.Tornado")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_FlyAway =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.FlyAway")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_Stun =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.Stun")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Active_Charm =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Active.Charm")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Passve =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Passve")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Passve_ZMJZ =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Passve.ZMJZ")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Talent =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Talent")));
FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Talent_NuQi =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Talent.NuQi")));

FGameplayTag UGameplayTagsLibrary::Proxy_Skill_Talent_YinYang =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Skill.Talent.YinYang")));

FGameplayTag UGameplayTagsLibrary::Proxy_Character =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Character")));

FGameplayTag UGameplayTagsLibrary::Proxy_Character_Player =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Character.Player")));

FGameplayTag UGameplayTagsLibrary::Proxy_Character_NPC_Assistional =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Proxy.Character.NPC.Assistional")));

#pragma endregion

// 人物身上自带的属性
FGameplayTag UGameplayTagsLibrary::DataSource_Character =
	FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Character")));

FGameplayTag UGameplayTagsLibrary::DataSource_EquipmentModify =
	FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.EquipmentModify")));

FGameplayTag UGameplayTagsLibrary::DataSource_TalentModify =
	FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.TalentModify")));

FGameplayTag UGameplayTagsLibrary::Skill_CanBeInterrupted =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Skill.CanBeInterrupted")));

FGameplayTag UGameplayTagsLibrary::Skill_CanBeInterrupted_Stagnation =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Skill.CanBeInterrupted.Stagnation")));


FGameplayTag UGameplayTagsLibrary::GEData_Info =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.Info")));

FGameplayTag UGameplayTagsLibrary::GEData_CD =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.CD")));

FGameplayTag UGameplayTagsLibrary::GEData_Damage =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.Damage")));

FGameplayTag UGameplayTagsLibrary::GEData_Damage_Callback =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.Damage.Callback")));

FGameplayTag UGameplayTagsLibrary::GEData_Damage_Callback_IsDeath =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.Damage.Callback.IsDeath")));


FGameplayTag UGameplayTagsLibrary::GEData_Duration =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.Duration")));

// 数据被修改的方式

// 直接增加
FGameplayTag UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyType.BaseValue.Addtive")));

// 覆盖
FGameplayTag UGameplayTagsLibrary::GEData_ModifyType_Immediate_Override =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyType.BaseValue.Override")));

// 增加一个临时量（比如修改移速时，先添加一个100的增速 再添加一个 20%增速，在移除这些增速是不好计算 所以用这种方式进行记录）
FGameplayTag UGameplayTagsLibrary::GEData_ModifyType_Temporary =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyType.Temporary")));

FGameplayTag UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyType.RemoveTemporary")));


FGameplayTag UGameplayTagsLibrary::GEData_ModifyItem_HP =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyItem.HP")));

FGameplayTag UGameplayTagsLibrary::GEData_ModifyItem_PP =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyItem.PP")));

FGameplayTag UGameplayTagsLibrary::GEData_ModifyItem_Mana =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyItem.Mana")));

FGameplayTag UGameplayTagsLibrary::GEData_ModifyItem_Damage_Base =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyItem.Damage.Base")));

FGameplayTag UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyItem.MoveSpeed")));

FGameplayTag UGameplayTagsLibrary::GEData_ModifyItem_PerformSpeed =
	FGameplayTag::RequestGameplayTag(FName(TEXT("GEData.ModifyItem.PerformSpeed")));
