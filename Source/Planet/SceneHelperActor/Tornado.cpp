#include "Tornado.h"

#include "AssetRefMap.h"
#include "AS_Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "Skill_Active_Tornado.h"

ATornado::ATornado(
	const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/
) :
  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	CapsuleComponentPtr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponentPtr->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponentPtr->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponentPtr->CanCharacterStepUpOn = ECB_No;
	CapsuleComponentPtr->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponentPtr->SetCanEverAffectNavigation(false);
	CapsuleComponentPtr->bDynamicObstacle = true;
	CapsuleComponentPtr->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicatingMovement(true);
}

void ATornado::Tick(
	float DeltaTime
)
{
	Super::Tick(DeltaTime);

	if (!ItemProxy_DescriptionPtr)
	{
		return;
	}

#if UE_EDITOR || UE_SERVER
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime > ItemProxy_DescriptionPtr->Inverval)
		{
			CurrentTime = 0.f;

			auto OwnerCharacterPtr = GetOwner<FOwnerType>();
			auto CharacterAttributesPtr = OwnerCharacterPtr->GetCharacterAttributesComponent()->
			                                                 GetCharacterAttributes();
			auto CharacterAbilitySystemComponentPtr = OwnerCharacterPtr->GetCharacterAbilitySystemComponent();
			auto SpecHandle = CharacterAbilitySystemComponentPtr->MakeOutgoingSpec(
				UAssetRefMap::GetInstance()->DamageClass,
				1,
				CharacterAbilitySystemComponentPtr->MakeEffectContext()
			);

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
			SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());

			const auto Value = ItemProxy_DescriptionPtr->BaseDamage + (ItemProxy_DescriptionPtr->AD *
					CharacterAttributesPtr->GetAD())
				+ (ItemProxy_DescriptionPtr->AP * CharacterAttributesPtr->GetAP());
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				UGameplayTagsLibrary::GEData_ModifyItem_Damage_Base,
				Value
			);

			for (const auto& Iter : HasBeenEffectedSet)
			{
				if (Iter)
				{
					auto TargetCharacterPtr = Cast<ACharacterBase>(Iter);
					if (TargetCharacterPtr)
					{
						auto TargetCharacterAbilitySystemComponentPtr = TargetCharacterPtr->
							GetCharacterAbilitySystemComponent();
						const auto GEHandle = CharacterAbilitySystemComponentPtr->ApplyGameplayEffectSpecToTarget(
							*SpecHandle.Data.Get(),
							TargetCharacterAbilitySystemComponentPtr
						);
						if (GEHandle.IsValid())
						{
						}
						else
						{
							// checkNoEntry();
						}
					}
				}
			}
		}
	}
#endif

	const auto Offset = Direction * (DeltaTime * ItemProxy_DescriptionPtr->MoveSpeed);
	const auto NewPt = GetActorLocation() + Offset;

	FHitResult OutHit;
	const FVector LineTraceOffset = FVector(0, 0, 1000);

	FCollisionObjectQueryParams ObjectQueryParams;

	ObjectQueryParams.AddObjectTypesToQuery(LandScape_Object);

	if (GetWorld()->LineTraceSingleByObjectType(
		OutHit,
		NewPt + LineTraceOffset,
		NewPt - LineTraceOffset,
		ObjectQueryParams
	))
	{
		SetActorLocation(OutHit.ImpactPoint);
	}
}

void ATornado::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetLocalRole() == ROLE_Authority)
	{
		// HasBeenEffectedSet.Add(GetOwner<ACharacterBase>());
		CapsuleComponentPtr->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	}
#endif
}

void ATornado::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ItemProxy_DescriptionPtr, COND_InitialOnly);
}

void ATornado::SetData(
	const TObjectPtr<UItemProxy_Description_ActiveSkill_Tornado>& InItemProxy_DescriptionPtr,

	const TSharedPtr<FSkillProxy>& InSkillProxyPtr,
	FVector InDirection
)
{
	ItemProxy_DescriptionPtr = InItemProxy_DescriptionPtr;
	SkillProxyPtr = InSkillProxyPtr;
	Direction = InDirection;

	CapsuleComponentPtr->InitCapsuleSize(ItemProxy_DescriptionPtr->OuterRadius, ItemProxy_DescriptionPtr->Height);

	SetLifeSpan(ItemProxy_DescriptionPtr->Duration.PerLevelValue[0]);
}

void ATornado::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor)
	{
		auto TargetCharacterPtr = Cast<ACharacterBase>(OtherActor);
		if (TargetCharacterPtr)
		{
			auto OwnerCharacterPtr = GetOwner<FOwnerType>();
			if (TargetCharacterPtr == OwnerCharacterPtr)
			{
				return;
			}
			if (OwnerCharacterPtr->IsGroupmate(TargetCharacterPtr))
			{
				return;
			}
			if (TargetCharacterPtr->GetCharacterAbilitySystemComponent()->IsCanBeDamage())
			{
				return;
			}
			if (HasBeenEffectedSet.Contains(TargetCharacterPtr))
			{
				return;
			}
			HasBeenEffectedSet.Add(TargetCharacterPtr);
			TargetCharacterPtr->GetCharacterAbilitySystemComponent()->HasbeenTornodo(this);
		}
	}
}
