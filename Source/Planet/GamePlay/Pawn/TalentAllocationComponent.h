#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TalentUnit.h"
#include "GenerateType.h"

#include "TalentAllocationComponent.generated.h"

class IPlanetControllerInterface;
class ACharacterBase;

UCLASS(BlueprintType, Blueprintable)
class UTalentAllocationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = ACharacterBase;

	static FName ComponentName;

	FTalentHelper AddCheck(FTalentHelper& TalentHelper);

	FTalentHelper SubCheck(FTalentHelper& TalentHelper);
	
	FTalentHelper* GetCheck(FTalentHelper& TalentHelper);

	void Clear(FTalentHelper& TalentHelper);

	int32 GetTotalTalentPointNum() const;

	int32 GetUsedTalentPointNum() const;

	void SyncToHolding();

	TOnValueChangedCallbackContainer<int32>CallbackContainerHelper;

protected:

	virtual void BeginPlay()override;

	void InitialTalentData();

	void CalculorUsedTalentPointNum();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TalentAry")
	TMap<FGameplayTag, FTalentHelper>TalentMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities Tag")
	int32 MaxTalentLevel = 3;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities Tag")
	int32 TotalTalentPointNum = 18;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities Tag")
	int32 UsedTalentPointNum = 0;

	EPointSkillType PreviousSkillType = EPointSkillType::kDuXing;

	FGuid PropertuModify_GUID = FGuid::NewGuid();

};