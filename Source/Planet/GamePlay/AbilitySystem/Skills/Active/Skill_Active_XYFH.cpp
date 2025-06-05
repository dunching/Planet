#include "Skill_Active_XYFH.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include <Engine/OverlapResult.h>
#include <GameFramework/SpringArmComponent.h>
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "PlanetModule.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_TimerHelper.h"
#include "Helper_RootMotionSource.h"
#include "GameplayTask_Tornado.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "AssetRefMap.h"
#include "GameplayCameraCommon.h"
#include "GameplayCameraHelper.h"
#include "HumanCharacter_Player.h"
#include "ItemProxy_Skills.h"
#include "KismetGravityLibrary.h"
#include "NiagaraActor.h"
#include "PlanetPlayerController.h"
#include "PlayerComponent.h"

struct FSkill_Active_XYFH : public TStructVariable<FSkill_Active_XYFH>
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
};

ASkill_Active_XYFH_Projectile::ASkill_Active_XYFH_Projectile(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(CollisionComp);

	ProjectileMovementCompPtr->HomingAccelerationMagnitude = 10000.f;
	ProjectileMovementCompPtr->InitialSpeed = 1500.f;
	ProjectileMovementCompPtr->MaxSpeed = 1500.f;
	InitialLifeSpan = 10.f;
}

USkill_Active_XYFH::USkill_Active_XYFH() :
                                         Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CurrentWaitInputTime = 1.f;
}

void USkill_Active_XYFH::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
		                                                            DynamicCastSharedPtr<FActiveSkillProxy>(
			                                                             SkillProxyPtr
			                                                            )->GetTableRowProxy_ActiveSkillExtendInfo()
		                                                           );
	}
}

void USkill_Active_XYFH::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
	)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	Index = 0;

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		if (!SPlineActorPtr)
		{
			SPlineActorPtr = GetWorld()->SpawnActor<ASPlineActor>(
			                                                      ItemProxy_DescriptionPtr->SPlineActorClass,
			                                                      CharacterPtr->GetActorTransform()
			                                                     );
		}
	}

	if (CharacterPtr)
	{
#if UE_EDITOR || UE_CLIENT
		if (
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
		{
			if (SPlineActorPtr && CharacterPtr)
			{
				CharacterPtr->GetController<APlanetPlayerController>()->AddProvideEyesViewActor(SPlineActorPtr);
			}

			UGameplayCameraHelper::SwitchGaplayCameraType(
			                                              Cast<AHumanCharacter_Player>(CharacterPtr),
			                                              ECameraType::kReleasing_ActiveSkill_XYFH
			                                             );
		}
#endif
	}
}

void USkill_Active_XYFH::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USkill_Active_XYFH::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
	) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_XYFH::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	UGameplayCameraHelper::SwitchGaplayCameraType(Cast<AHumanCharacter_Player>(CharacterPtr), ECameraType::kAction);

#if UE_EDITOR || UE_CLIENT
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		if (SPlineActorPtr && CharacterPtr && IsLocallyControlled())
		{
			CharacterPtr->GetController<APlanetPlayerController>()->RemoveProvideEyesViewActor(SPlineActorPtr);
		}
	}
#endif

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		if (SPlineActorPtr)
		{
			SPlineActorPtr->Destroy();
		}
		SPlineActorPtr = nullptr;
	}

	//	TargetOffsetValue.ReStore();

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_XYFH::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void USkill_Active_XYFH::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_CD);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
		                                               UGameplayTagsLibrary::GEData_Duration,
		                                               ItemProxy_DescriptionPtr->CD.PerLevelValue[0]
		                                              );

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void USkill_Active_XYFH::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
		                                               UGameplayTagsLibrary::GEData_ModifyItem_Mana,
		                                               -ItemProxy_DescriptionPtr->Cost.PerLevelValue[0]
		                                              );

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void USkill_Active_XYFH::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WaitInput = false;

	if (AbilityTask_TimerHelperPtr)
	{
		AbilityTask_TimerHelperPtr->ExternalCancel();
	}
	AbilityTask_TimerHelperPtr = nullptr;
	RemainTime = 0;

	if (CharacterPtr)
	{
		switch (Index)
		{
		case 0:
			{
				PlayMontage(
				            ItemProxy_DescriptionPtr->HumanMontage1.LoadSynchronous(),
				            ItemProxy_DescriptionPtr->Montage1SectionName
				           );
				MoveAlongSPlineTask(
				                    Index,
				                    Index + 1,
				                    ItemProxy_DescriptionPtr->HumanMontage1->GetSectionLength(
					                     ItemProxy_DescriptionPtr->HumanMontage1->GetSectionIndex(
						                      ItemProxy_DescriptionPtr->Montage1SectionName
						                     )
					                    )
				                   );
			}
			break;
		case 1:
			{
				auto NiagaraActorPtr = GetWorld()->SpawnActor<ANiagaraActor>(
				                                                             ItemProxy_DescriptionPtr->
				                                                             NiagaraActorClass,
				                                                             CharacterPtr->GetActorTransform()
				                                                            );
				GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(UGameplayTagsLibrary::State_Invisible);
				MoveAlongSPlineTask(Index, Index + 1, ItemProxy_DescriptionPtr->MoveDuration);
			}
			break;
		case 2:
			{
				auto NiagaraActorPtr = GetWorld()->SpawnActor<ANiagaraActor>(
				                                                             ItemProxy_DescriptionPtr->
				                                                             NiagaraActorClass,
				                                                             CharacterPtr->GetActorTransform()
				                                                            );
				GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(UGameplayTagsLibrary::State_Invisible);
				MoveAlongSPlineTask(Index, Index + 1, ItemProxy_DescriptionPtr->MoveDuration);
			}
			break;
		case 3:
			{
				auto NiagaraActorPtr = GetWorld()->SpawnActor<ANiagaraActor>(
				                                                             ItemProxy_DescriptionPtr->
				                                                             NiagaraActorClass,
				                                                             CharacterPtr->GetActorTransform()
				                                                            );
				GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(UGameplayTagsLibrary::State_Invisible);
				MoveAlongSPlineTask(Index, Index + 1, ItemProxy_DescriptionPtr->MoveDuration);
			}
			break;
		case 4:
			{
				auto NiagaraActorPtr = GetWorld()->SpawnActor<ANiagaraActor>(
				                                                             ItemProxy_DescriptionPtr->
				                                                             NiagaraActorClass,
				                                                             CharacterPtr->GetActorTransform()
				                                                            );
				GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(UGameplayTagsLibrary::State_Invisible);
				MoveAlongSPlineTask(Index, Index + 1, ItemProxy_DescriptionPtr->MoveDuration);
			}
			break;
		default:
			break;
		}
	}
	else
	{
	}
}

float USkill_Active_XYFH::GetRemainTime() const
{
	return RemainTime;
}

void USkill_Active_XYFH::MoveAlongSPlineTask(
	int32 StartPtIndex,
	int32 EndPtIndex,
	float Duration
	)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		// 角色移动
		auto TaskPtr = UAbilityTask_ApplyRootMotionBySPline::NewTask(
		                                                             this,
		                                                             TEXT(""),
		                                                             Duration,
		                                                             SPlineActorPtr,
		                                                             StartPtIndex,
		                                                             EndPtIndex
		                                                            );

		switch (Index)
		{
		case 0:
			{
			}
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			{
				TaskPtr->OnFinish.BindUObject(this, &ThisClass::MoveAlongSPlineTaskComplete);
			}
			break;
		default:
			break;
		}

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_XYFH::PlayMontage(
	UAnimMontage* AnimMontagePtr,
	const FName& SectionName
	)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			 this,
			 TEXT(""),
			 AnimMontagePtr,
			 InPlayRate,
			 SectionName
			);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::OnPlayMontageEnd);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::OnPlayMontageEnd);
		TaskPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_XYFH::OnPlayMontageEnd()
{
	Index++;

	if (Index >= ItemProxy_DescriptionPtr->MaxIndex)
	{
#if UE_EDITOR || UE_SERVER
		if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
		{
			K2_CancelAbility();
		}
#endif
	}
	else
	{
		PerformIfContinue();
	}
}

void USkill_Active_XYFH::MoveAlongSPlineTaskComplete()
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(UGameplayTagsLibrary::State_Invisible);
	switch (Index)
	{
	case 0:
		{
		}
		break;
	case 1:
		{
			PlayMontage(
			            ItemProxy_DescriptionPtr->HumanMontage2.LoadSynchronous(),
			            ItemProxy_DescriptionPtr->Montage2SectionName
			           );
		}
		break;
	case 2:
		{
			PlayMontage(
			            ItemProxy_DescriptionPtr->HumanMontage3.LoadSynchronous(),
			            ItemProxy_DescriptionPtr->Montage3SectionName
			           );
		}
		break;
	case 3:
		{
			PlayMontage(
			            ItemProxy_DescriptionPtr->HumanMontage4.LoadSynchronous(),
			            ItemProxy_DescriptionPtr->Montage4SectionName
			           );
		}
		break;
	case 4:
		{
			PlayMontage(
			            ItemProxy_DescriptionPtr->HumanMontage5.LoadSynchronous(),
			            ItemProxy_DescriptionPtr->Montage5SectionName
			           );
		}
		break;
	default:
		break;
	}
}

bool USkill_Active_XYFH::CanOncemorePerformAction() const
{
	return WaitInput;
}

void USkill_Active_XYFH::OnNotifyBeginReceived(
	FName NotifyName
	)
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		if (NotifyName == FSkill_Active_XYFH::Get().Hit)
		{
			EmitProjectile();
		}
	}
#endif
}

void USkill_Active_XYFH::OnProjectileBounce(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	if (OtherActor && OtherActor->IsA(ACharacterBase::StaticClass()))
	{
		auto OtherCharacterPtr = Cast<ACharacterBase>(OtherActor);
		if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
		{
			return;
		}
		if (OtherCharacterPtr->GetCharacterAbilitySystemComponent()->IsCantBeDamage())
		{
			return;
		}

		MakeDamage(OtherCharacterPtr);
	}

	OverlappedComponent->GetOwner()->Destroy();
}

bool USkill_Active_XYFH::PerformIfContinue()
{
	WaitInput = true;

#if UE_EDITOR || UE_CLIENT
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		if (GetIsContinue())
		{
			Cast<UPlanetAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo())->
				ReplicatePerformAction_Server(
				                              GetCurrentAbilitySpecHandle(),
				                              GetCurrentActivationInfo()
				                             );
			return true;
		}
		else
		{
		}

		AbilityTask_TimerHelperPtr = UAbilityTask_TimerHelper::DelayTask(this);
		AbilityTask_TimerHelperPtr->SetDuration(ItemProxy_DescriptionPtr->WaitInputTime);
		AbilityTask_TimerHelperPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
		AbilityTask_TimerHelperPtr->OnFinished.BindLambda(
		                                                  [this](
		                                                  auto
		                                                  )
		                                                  {
			                                                  CancelAbility_Server();
			                                                  return true;
		                                                  }
		                                                 );
		AbilityTask_TimerHelperPtr->ReadyForActivation();
	}
#endif

	return false;
}

void USkill_Active_XYFH::DurationDelegate(
	UAbilityTask_TimerHelper*,
	float CurrentTiem,
	float TotalTime
	)
{
	if (FMath::IsNearlyZero(TotalTime))
	{
		return;
	}

	RemainTime = (TotalTime - CurrentTiem) / TotalTime;
	if (RemainTime < 0.f)
	{
		RemainTime = 0.f;
	}
}

void USkill_Active_XYFH::EmitProjectile() const
{
	auto EmitTransform = CharacterPtr->GetActorTransform();

	// 初始角度
	const auto OriginRot = UKismetMathLibrary::MakeRotFromZX(
	                                                         -UKismetGravityLibrary::GetGravity(),
	                                                         CharacterPtr->GetActorForwardVector()
	                                                        );
	EmitTransform.SetRotation(OriginRot.Quaternion());

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.CustomPreSpawnInitalization = [this](
		AActor* ActorPtr
		)
		{
			Cast<AProjectileBase>(ActorPtr)->MaxMoveRange = ItemProxy_DescriptionPtr->AttackDistance;
		};

	switch (Index)
	{
	case 4:
		{
			TArray<TObjectPtr<ASkill_Active_XYFH_Projectile>> ProjectileAry;

			const auto ForwardOffset = OriginRot.Quaternion().GetForwardVector() * ItemProxy_DescriptionPtr->
			                           ForwardOffset;

			const auto Mid = ItemProxy_DescriptionPtr->LastSecondNum / 2;
			for (int32 TargetIndex = 0; TargetIndex < ItemProxy_DescriptionPtr->LastSecondNum; TargetIndex++)
			{
				auto TempEmitTransform = EmitTransform;
				const auto Offset = (TargetIndex - Mid) * (
					                    OriginRot.Quaternion().GetRightVector() * ItemProxy_DescriptionPtr->Offset);

				TempEmitTransform.SetLocation(EmitTransform.GetLocation() + Offset + ForwardOffset);

				auto ProjectilePtr = GetWorld()->SpawnActor<ASkill_Active_XYFH_Projectile>(
					 ItemProxy_DescriptionPtr->ProjectileClass,
					 TempEmitTransform,
					 SpawnParameters
					);
				ProjectilePtr->SetSpeed(1);

				if (ProjectilePtr)
				{
					ProjectilePtr->CollisionComp->OnComponentBeginOverlap.AddDynamic(
						 this,
						 &ThisClass::OnProjectileBounce
						);
					ProjectileAry.Add(ProjectilePtr);
				}

				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
				                                       TimerHandle,
				                                       [ProjectileAry]()
				                                       {
					                                       for (auto ProjectilePtr : ProjectileAry)
					                                       {
						                                       if (ProjectilePtr)
						                                       {
							                                       ProjectilePtr->SetSpeed(
								                                        ProjectilePtr->ProjectileMovementCompPtr->
								                                        MaxSpeed
								                                       );
						                                       }
					                                       }
				                                       },
				                                       1,
				                                       false
				                                      );
			}
		}
		break;
	case 0:
	case 1:
	case 2:
	case 3:
		{
		}
	default:
		{
			auto TargetsAry = GetTargetsInDistanceByNearestCharacterViewDirection(
				 ItemProxy_DescriptionPtr->AttackDistance,
				 ItemProxy_DescriptionPtr->UpOffset,
				 ItemProxy_DescriptionPtr->DownOffset
				);

			if (TargetsAry.IsEmpty())
			{
				auto ProjectilePtr = GetWorld()->SpawnActor<ASkill_Active_XYFH_Projectile>(
					 ItemProxy_DescriptionPtr->ProjectileClass,
					 EmitTransform,
					 SpawnParameters
					);

				if (ProjectilePtr)
				{
					ProjectilePtr->CollisionComp->OnComponentBeginOverlap.AddDynamic(
						 this,
						 &ThisClass::OnProjectileBounce
						);
				}
			}
			else
			{
				for (int32 TargetIndex = 0; (TargetIndex < ItemProxy_DescriptionPtr->MaxTargetNum) && (
					                            TargetIndex < TargetsAry.Num());
				     TargetIndex++)
				{
					auto ProjectilePtr = GetWorld()->SpawnActor<ASkill_Active_XYFH_Projectile>(
						 ItemProxy_DescriptionPtr->ProjectileClass,
						 EmitTransform,
						 SpawnParameters
						);

					if (ProjectilePtr)
					{
						ProjectilePtr->SetHomingTarget(TargetsAry[TargetIndex]);
						ProjectilePtr->CollisionComp->OnComponentBeginOverlap.AddDynamic(
							 this,
							 &ThisClass::OnProjectileBounce
							);
					}
				}
			}
		}
		break;
	}
}

void USkill_Active_XYFH::MakeDamage(
	const TObjectPtr<ACharacterBase>& TargetCharacterPtr
	) const
{
	const int32 BaseDamage = ItemProxy_DescriptionPtr->Elemental_Damage;

	FGameplayEffectSpecHandle SpecHandle = MakeDamageToTargetSpecHandle(
																  ItemProxy_DescriptionPtr->ElementalType,
																  ItemProxy_DescriptionPtr->Elemental_Damage,
																  ItemProxy_DescriptionPtr->Elemental_Damage_Magnification
																 );

	TArray<TWeakObjectPtr<AActor>> Ary;
	Ary.Add(TargetCharacterPtr);
	FGameplayAbilityTargetDataHandle TargetData;

	auto GameplayAbilityTargetData_ActorArrayPtr = new FGameplayAbilityTargetData_ActorArray;
	GameplayAbilityTargetData_ActorArrayPtr->SetActors(Ary);

	TargetData.Add(GameplayAbilityTargetData_ActorArrayPtr);
	const auto GEHandleAry = MyApplyGameplayEffectSpecToTarget(
	                                                           GetCurrentAbilitySpecHandle(),
	                                                           GetCurrentActorInfo(),
	                                                           GetCurrentActivationInfo(),
	                                                           SpecHandle,
	                                                           TargetData
	                                                          );
}
