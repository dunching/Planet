
#include "Skill_Active_Switch_Test.h"

#include "Net/UnrealNetwork.h"

#include "ProxyProcessComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsSubSystem.h"

void USkill_Active_Switch_Test::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, bIsUsingActives, COND_AutonomousOnly);
}

void USkill_Active_Switch_Test::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bIsUsingActives = !bIsUsingActives;
	struct FMyStruct
	{
		FGameplayTag ActiveSocketTag;
		FGameplayTag PassiveSocketTag;
	};
	TArray<FMyStruct> Ary
	{
		{UGameplayTagsSubSystem::ActiveSocket_1,UGameplayTagsSubSystem::PassiveSocket_1 },
		{UGameplayTagsSubSystem::ActiveSocket_2,UGameplayTagsSubSystem::PassiveSocket_2 },
		{UGameplayTagsSubSystem::ActiveSocket_3,UGameplayTagsSubSystem::PassiveSocket_3 },
		{UGameplayTagsSubSystem::ActiveSocket_4,UGameplayTagsSubSystem::PassiveSocket_4 },
	};

	TMap<FGameplayTag, TSharedPtr<FSocket_FASI>>CanActiveSocketMap;
	auto ProxyProcessComponentPtr = CharacterPtr->GetProxyProcessComponent();
	for (const auto& Iter : Ary)
	{
		auto SocketSPtr = ProxyProcessComponentPtr->FindSocket(Iter.ActiveSocketTag);
		TSharedPtr<FSocket_FASI> TempSocketSPtr = SocketSPtr;
		if (
			SocketSPtr->ProxySPtr &&
			(SocketSPtr->ProxySPtr == SkillUnitPtr)
			)
		{
		}
		else 
		{
			if (bIsUsingActives)
			{
			}
			else
			{
				auto PassiveSocketSPtr = ProxyProcessComponentPtr->FindSocket(Iter.PassiveSocketTag);
				if (
					PassiveSocketSPtr->ProxySPtr &&
					PassiveSocketSPtr->ProxySPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::Unit_Skill_Active)
					)
				{
					TempSocketSPtr = MakeShared<FSocket_FASI>();
					*TempSocketSPtr = *SocketSPtr;

					TempSocketSPtr->ProxySPtr = PassiveSocketSPtr->ProxySPtr;
				}
			}
		}
		CanActiveSocketMap.Add(Iter.ActiveSocketTag, TempSocketSPtr);
	}
	ProxyProcessComponentPtr->UpdateCanbeActiveSkills_UsePassiveSocket(CanActiveSocketMap);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
		K2_CancelAbility();
	}
#endif
}
