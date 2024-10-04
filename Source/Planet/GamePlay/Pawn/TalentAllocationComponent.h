#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Talent_FASI.h"
#include "GenerateType.h"

#include "TalentAllocationComponent.generated.h"

class IPlanetControllerInterface;
class ACharacterBase;
struct FPropertySettlementModify_Talent;

UCLASS(BlueprintType, Blueprintable)
class UTalentAllocationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = ACharacterBase;

	static FName ComponentName;

	UTalentAllocationComponent(const FObjectInitializer& ObjectInitializer);

	void AddCheck(const FTalentHelper& TalentHelper);

	void SubCheck(const FTalentHelper& TalentHelper);

	FTalentHelper GetCheck(const FTalentHelper& TalentHelper);

	void Clear(FTalentHelper& TalentHelper);

	int32 GetTotalTalentPointNum() const;

	int32 GetUsedTalentPointNum() const;

	void SyncToHolding();

	void UpdateTalent_Client(const FTalentHelper& TalentHelper);

	TOnValueChangedCallbackContainer<int32>CallbackContainerHelper;
	
	UPROPERTY(Replicated)
	FTalent_FASI_Container Talent_FASI_Container;
	
protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay()override;

	void InitialTalentData();

	void CalculorUsedTalentPointNum();

	UFUNCTION(Server, Reliable)
	void AddCheck_Server(const FTalentHelper& TalentHelper);

	UFUNCTION(Server, Reliable)
	void SubCheck_Server(const FTalentHelper& TalentHelper);

	void UpdateTalent(const FTalentHelper& TalentHelper);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TalentAry")
	TMap<FGameplayTag, FTalentHelper>TalentMap;

	int32 UsedTalentPointNum = 0;

	int32 MaxTalentLevel = 3;

	int32 TotalTalentPointNum = 18;

	EPointSkillType PreviousSkillType = EPointSkillType::kDuXing;

	TMap<EPointPropertyType, TWeakPtr<FPropertySettlementModify_Talent>>ModifyMap;

};