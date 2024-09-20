#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "CDCaculator.h"

#include "CDCaculatorComponent.generated.h"

struct FSkillCooldownHelper;
struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;
struct FCD_FASI_Container;
struct FCDItem_FASI;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UCDCaculatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	friend FCD_FASI_Container;
	friend FCDItem_FASI;

	static FName ComponentName;

	UCDCaculatorComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(
		float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
	)override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 重置技能CD（包含公共CD）至满CD
	void ApplyCooldown(FActiveSkillProxy* ActiveSkillUnitPtr);
	
	TSharedPtr<FSkillCooldownHelper> GetCooldown(const FActiveSkillProxy* ActiveSkillUnitPtr)const;

	UPROPERTY(Replicated)
	FCD_FASI_Container CD_FASI_Container;
	
private:

	TMap<FGuid, TSharedPtr<FSkillCooldownHelper>>Separate_Map;

	TMap<FGameplayTag, TSharedPtr<FSkillCooldownHelper>>Common_Map;

};