#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "GAEvent_Helper.h"

#include "InteractiveComponent.generated.h"

class UGameplayAbility;

class ATool_Base;
class UBasicFuturesBase;
class UPlanetGameplayAbility_HumanSkillBase;
class IGAEventModifyInterface;
class AEquipmentBase;
class USkill_Base;
class USkill_Active_Base;
class AWeapon_Base;
class UGAEvent_Send;
class UGAEvent_Received;
class USkill_Element_Gold;

struct FGameplayAbilityTargetData_GASendEvent;
class UWeaponUnit;

struct FCanbeActivedInfo
{
	enum class EType
	{
		kSwitchToTool,
		kActiveTool,

		kActiveSkill,
		kWeaponActiveSkill,
	};

	EType Type = EType::kActiveSkill;

	FKey Key;

	FGameplayTag Socket;
};

UCLASS(BlueprintType, Blueprintable)
class UInteractiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	UInteractiveComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	virtual TArray<TSharedPtr<FCanbeActivedInfo>> GetCanbeActiveAction()const;

	virtual bool ActiveAction(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	);

	virtual void CancelAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr);

protected:

	virtual void GenerationCanbeActiveEvent();

	FDelegateHandle AbilityActivatedCallbacksHandle;

};