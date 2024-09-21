#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "CDCaculator.h"

#include "CDCaculatorComponent.generated.h"

struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;
struct FCD_FASI_Container;
struct FCDItem_FASI;

struct FSkillCooldownHelper
{
	FSkillCooldownHelper();

	virtual bool CheckCooldown()const;

	void IncreaseCooldownTime(float DeltaTime);

	void AddCooldownConsumeTime(float NewTime);

	void FreshCooldownTime();

	void ResetCooldownTime();

	void OffsetCooldownTime();

	bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const;

	void SetCooldown(float CooldDown);

	// 独立的CD
	FGuid SkillProxy_ID;

	// 类型的CD
	FGameplayTag SkillType;

	// 
	int32 CooldownTime = -1;

	// 
	float CooldownConsumeTime = 0.f;

private:

};

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

	// 重置技能CD（包含公共CD）至满CD
	void ApplyCooldown(FConsumableProxy* ConsumableProxySPtr);
	
	TSharedPtr<FSkillCooldownHelper> GetCooldown(const FConsumableProxy* ConsumableProxySPtr)const;

	UPROPERTY(Replicated)
	FCD_FASI_Container CD_FASI_Container;
	
private:

	// 独立CD
	TMap<FGuid, TSharedPtr<FSkillCooldownHelper>>Separate_Map;

	// 公共CD
	TMap<FGameplayTag, TSharedPtr<FSkillCooldownHelper>>Common_Map;

};