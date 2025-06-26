#include "GE_Component.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

inline bool UCancelAbilityGameplayEffectComponent::OnActiveGameplayEffectAdded(
	FActiveGameplayEffectsContainer& ActiveGEContainer,
	FActiveGameplayEffect& ActiveGE
	) const
{
	auto AbilitySystemComponentPtr = ActiveGEContainer.Owner;
	if (AbilitySystemComponentPtr)
	{
		AbilitySystemComponentPtr->CancelAbilities(&CancelAbility);
	}

	return Super::OnActiveGameplayEffectAdded(ActiveGEContainer, ActiveGE);
}

bool UActivationOwnedTagsGameplayEffectComponent::OnActiveGameplayEffectAdded(
	FActiveGameplayEffectsContainer& ActiveGEContainer,
	FActiveGameplayEffect& ActiveGE
	) const
{
	FActiveGameplayEffectHandle& ActiveGEHandle = ActiveGE.Handle;
	UAbilitySystemComponent* OwnerASC = ActiveGEContainer.Owner;

	OwnerASC->AddLooseGameplayTags(ActivationOwnedTags);
	OwnerASC->AddReplicatedLooseGameplayTags(ActivationOwnedTags);

	// Register our immunity query to potentially block applications of any Gameplay Effects
	FGameplayEffectApplicationQuery& BoundQuery = OwnerASC->GameplayEffectApplicationQueries.AddDefaulted_GetRef();

	// Now that we've bound that function, let's unbind it when we're removed.  This is safe because once we're removed, EventSet is gone.
	ActiveGE.EventSet.OnEffectRemoved.AddLambda(
	                                            [OwnerASC, QueryToRemove = BoundQuery.GetHandle(), this](
	                                            const FGameplayEffectRemovalInfo& RemovalInfo
	                                            )
	                                            {
		                                            if (ensure(IsValid(OwnerASC)))
		                                            {
			                                            TArray<FGameplayEffectApplicationQuery>& GEAppQueries = OwnerASC
				                                            ->GameplayEffectApplicationQueries;
			                                            for (auto It = GEAppQueries.CreateIterator(); It; ++It)
			                                            {
				                                            if (It->GetHandle() == QueryToRemove)
				                                            {
					                                            OwnerASC->RemoveLooseGameplayTags(
						                                             ActivationOwnedTags
						                                            );
					                                            OwnerASC->RemoveReplicatedLooseGameplayTags(
						                                             ActivationOwnedTags
						                                            );
					                                            It.RemoveCurrentSwap();
					                                            break;
				                                            }
			                                            }
		                                            }
	                                            }
	                                           );

	return Super::OnActiveGameplayEffectAdded(ActiveGEContainer, ActiveGE);
}
