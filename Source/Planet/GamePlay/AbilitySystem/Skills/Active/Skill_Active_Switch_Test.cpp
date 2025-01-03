
#include "Skill_Active_Switch_Test.h"

#include "Net/UnrealNetwork.h"

#include "ProxyProcessComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"
#include "HoldingItemsComponent.h"

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
		{UGameplayTagsLibrary::ActiveSocket_1,UGameplayTagsLibrary::PassiveSocket_1 },
		{UGameplayTagsLibrary::ActiveSocket_2,UGameplayTagsLibrary::PassiveSocket_2 },
		{UGameplayTagsLibrary::ActiveSocket_3,UGameplayTagsLibrary::PassiveSocket_3 },
		{UGameplayTagsLibrary::ActiveSocket_4,UGameplayTagsLibrary::PassiveSocket_4 },
	};

	TMap<FGameplayTag, FCharacterSocket>CanActiveSocketMap;
	auto ProxyProcessComponentPtr = CharacterPtr->GetProxyProcessComponent();
	auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();
	for (const auto& Iter : Ary)
	{
		auto SocketSPtr = ProxyProcessComponentPtr->FindSocket(Iter.ActiveSocketTag);
		auto TempSocketSPtr = HoldingItemsComponentPtr->FindProxy_BySocket(SocketSPtr);
		if (
			TempSocketSPtr &&
			(TempSocketSPtr == SkillProxyPtr)
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

			}
		}
	}
	ProxyProcessComponentPtr->UpdateCanbeActiveSkills_UsePassiveSocket(CanActiveSocketMap);

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
		K2_CancelAbility();
	}
#endif
}
