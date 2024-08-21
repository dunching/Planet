#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractiveSkillComponent.h"
#include "GAEvent_Helper.h"

#include "InteractiveBaseGAComponent.generated.h"

class UGAEvent_Received;
class UConsumableUnit;
class UCS_PeriodicPropertyModify;
class UCS_RootMotion;
class UCS_Base;

struct FGameplayAbilityTargetData_RootMotion;
struct FGameplayAbilityTargetData_PropertyModify;
struct FGameplayAbilityTargetData_StateModify;
struct FGameplayAbilityTargetData_AddTemporaryTag;
struct FGameplayAbilityTargetData_MoveToAttaclArea;
struct FGameplayAbilityTargetData;

TMap<ECharacterPropertyType, FBaseProperty> GetAllData();

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UInteractiveBaseGAComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	friend UGAEvent_Received;

	using FOwnerPawnType = ACharacterBase;

	using FCharacterStateChanged = TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>;

	using FMakedDamage = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	static FName ComponentName;

	void OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData);

	void OnReceivedEventModifyData(FGameplayAbilityTargetData_GAReceivedEvent& OutGAEventData);

	void AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	void RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	FGameplayAbilitySpecHandle AddTemporaryTag(
		ACharacterBase* TargetCharacterPtr, 
		FGameplayAbilityTargetData_AddTemporaryTag* GameplayAbilityTargetDataPtr
	);

	void ClearData2Self(
		const FGameplayTag &DataSource
	);

	void SendEvent2Other(
		const TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap,
		const FGameplayTag& DataSource
	);

	void SendEvent2Self(
		const TMap<ECharacterPropertyType, FBaseProperty>& ModifyPropertyMap,
		const FGameplayTag& DataSource
	);

	void SendEventImp(
		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr
	);
	
	void SendEventImp(
		FGameplayAbilityTargetData_RootMotion* GameplayAbilityTargetDataPtr
	);

	void SendEventImp(
		FGameplayAbilityTargetData_StateModify* GameplayAbilityTargetDataPtr
	);

	void SendEventImp(
		FGameplayAbilityTargetData_PropertyModify* GameplayAbilityTargetDataPtr
	);

	void InitialBaseGAs();

	bool SwitchWalkState(bool bIsRun);
	
	bool Dash(EDashDirection DashDirection);

	void MoveToAttackDistance(
		FGameplayAbilityTargetData_MoveToAttaclArea * MoveToAttaclAreaPtr
	);

	void BreakMoveToAttackDistance();

	void ExcuteAttackedEffect(EAffectedDirection AffectedDirection);

	UCS_Base* GetCharacterState(const FGameplayTag&CSTag)const;

	FGameplayAbilitySpecHandle SendEventHandle;

	FGameplayAbilitySpecHandle ReceivedEventHandle;

	FCharacterStateChanged CharacterStateChangedContainer;

	FMakedDamage MakedDamage;

protected:

	// ע�⣺�����RootMotion���͵�״̬�޸ģ�������͵���״ֻ̬��Ϊһ�֣����磺�����ȱ�����2s��1s֮���ֱ�����2s����ˢ�»���ʱ��Ϊ2s
	// ��Щ״̬֮��ụ�⣬�类����֮���ٱ����䣬�������ȡ������Ч��
	// ����Щ״̬��ͨ���ἤ���Ӧ��GA����ѡ���Ӧ��montage
	// 
	// ��״̬��RootMotion��ͨ��GA��������Ŀɲٿ�״̬
	FGameplayAbilitySpecHandle ExcuteEffects(
		FGameplayAbilityTargetData_RootMotion* GameplayAbilityTargetDataPtr
	);

	void ExcuteEffects(
		TSharedPtr<FGameplayAbilityTargetData_PropertyModify> GameplayAbilityTargetDataSPtr
	);

	// ��ͨ��״̬�޸ģ���ֹ�ƶ���ѣ�Ρ���������Ĭ�����������ƣ��£�ͨ��������ͼѡ���Ӧ�Ķ�������Ϊ��Щ״̬���Թ��棬��������
	// �ȱ�ѣ��2s����ʱ״̬��ѡ��ѣ�εĶ�����Ȼ������1s����ʱ��������ѡ���Ƿ���ȶ�������������֮��ص�ѣ�ε�״̬��
	// 
	// ��״̬ͨ��Tag��������Ŀɲٿ�״̬
	FGameplayAbilitySpecHandle ExcuteEffects(
		FGameplayAbilityTargetData_StateModify* GameplayAbilityTargetDataPtr
	);

	void AddSendGroupEffectModify();

	void AddSendWuXingModify();

	void AddReceivedWuXingModify();

	void AddReceivedModify();

	void OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base * CharacterStatePtr);

#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilities;

	// 	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	// 	TSubclassOf<UGA_Periodic_PropertyModefy> GA_Periodic_PropertyModefyClass;
	// 
	// 	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	// 	TSubclassOf<UGA_Periodic_StateTagModefy> GA_Periodic_StateTagModefyClass;
#pragma endregion GAs

	std::multiset<TSharedPtr<IGAEventModifySendInterface>, FGAEventModify_key_compare>SendEventModifysMap;

	std::multiset<TSharedPtr<IGAEventModifyReceivedInterface>, FGAEventModify_key_compare>ReceivedEventModifysMap;

	TMap<FGameplayTag, UCS_Base*>CharacterStateMap;

	TMap<FGameplayTag, FGameplayAbilitySpecHandle>PeriodicStateTagModifyMap;
	
	TPair<FGameplayTag, FGameplayAbilitySpecHandle>PreviousRootMotionModify;

};