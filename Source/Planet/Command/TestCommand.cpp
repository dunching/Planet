
#include "TestCommand.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/StreamableManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Components/SplineComponent.h"
#include "GameplayEffect.h"
#include "Kismet/KismetStringLibrary.h"

#include "HumanCharacter.h"
#include "GameInstance/PlanetGameInstance.h"
#include "Pawn/HoldingItemsComponent.h"
#include "AssetRefMap.h"
#include <AssetRefrencePath.h>
#include "Planet.h"
#include "HumanCharacter.h"
#include "SPlineActor.h"
#include "SceneElement.h"
#include "Skill_Base.h"
#include "TalentUnit.h"
#include "TalentAllocationComponent.h"
#include "CharacterBase.h"
#include "CollisionDataStruct.h"
#include "GroupMnaggerComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "HumanControllerInterface.h"

void TestCommand::AddCahracterTestData()
{
	auto CharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (!CharacterPtr)
	{
		return;
	}
	AddPlayerCharacterTestDataImp(CharacterPtr);
}

void TestCommand::AddPlayerCharacterTestDataImp(AHumanCharacter* CharacterPtr)
{
	{
		auto& HoldItemComponent = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();
		HoldItemComponent.AddUnit(EToolUnitType::kPickAxe);

		HoldItemComponent.AddUnit(EWeaponUnitType::kPickAxe);
		HoldItemComponent.AddUnit(EWeaponUnitType::kWeaponHandProtection);

		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkills_ZMJZ);
		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Displacement);
		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_TalentSkill_NuQi);
		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_GroupTherapy);
	}
}

void TestCommand::AddAICharacterTestDataImp(AHumanCharacter* CharacterPtr)
{
	{
		auto& HoldItemComponent = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();
		HoldItemComponent.AddUnit(EToolUnitType::kPickAxe);

		HoldItemComponent.AddUnit(EWeaponUnitType::kPickAxe);
		HoldItemComponent.AddUnit(EWeaponUnitType::kWeaponHandProtection);

		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkills_ZMJZ);
		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Displacement);
		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_TalentSkill_NuQi);
		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_GroupTherapy);
	}
}

void TestCommand::TestAsyncAssetLoad()
{
}

void TestCommand::TestSubSystem()
{
	// 	auto Class = LoadClass<UAssetRefMap>(nullptr, *AssetRefMapClass.ToString());
	// 	auto CacheAssetManagerPtr = USubsystemBlueprintLibrary::GetEngineSubsystem(Class);

	UE_LOG(LogTemp, Warning, TEXT("123"));
}

void TestCommand::TestSpline()
{
	TArray<AActor*>Ary;
	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ACharacterBase::StaticClass(), Ary);

	if (Ary.IsValidIndex(1))
	{
		auto CharacterPtr = Cast<ACharacterBase>(Ary[0]);
		auto TargetCharacterPtr = Cast<ACharacterBase>(Ary[1]);

		const auto Pt1 = CharacterPtr->GetActorLocation();
		const auto Pt2 = TargetCharacterPtr->GetActorLocation();
		const auto Pt3 = CharacterPtr->GetActorLocation() + ((TargetCharacterPtr->GetActorLocation() - CharacterPtr->GetActorLocation()) / 2);

		const auto MidPt = Pt3 - (CharacterPtr->GetGravityDirection() * 100.f);

		auto SPlineActorPtr = GetWorldImp()->SpawnActor<ASPlineActor>();
		SPlineActorPtr->SplineComponentPtr->ClearSplinePoints();

		SPlineActorPtr->SplineComponentPtr->AddSplinePoint(Pt1, ESplineCoordinateSpace::World);
		SPlineActorPtr->SplineComponentPtr->AddSplinePoint(MidPt, ESplineCoordinateSpace::World);
		SPlineActorPtr->SplineComponentPtr->AddSplinePoint(Pt2, ESplineCoordinateSpace::World);

		SPlineActorPtr->SplineComponentPtr->SetTangentsAtSplinePoint(0, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::World);

		SPlineActorPtr->SplineComponentPtr->SetTangentsAtSplinePoint(2, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::World);
	}
}

FGameplayAbilitySpecHandle Handle1;

void TestCommand::TestDisplacementSkill()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		auto UnitClass = UAssetRefMap::GetInstance()->SkillToolsMap[ESkillUnitType::kHumanSkill_Displacement];

		Handle1 = CharacterPtr->GetAbilitySystemComponent()->K2_GiveAbility(UnitClass.GetDefaultObject()->SkillClass);
		CharacterPtr->GetAbilitySystemComponent()->TryActivateAbility(Handle1);
	}
}

void TestCommand::TestCooldown()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		auto GAHandle = CharacterPtr->GetAbilitySystemComponent()->FindAbilitySpecFromHandle(Handle1);
		auto GEHandle1 = CharacterPtr->GetAbilitySystemComponent()->FindActiveGameplayEffectHandle(Handle1);

		auto GEHandle = Cast<USkill_Base>(GAHandle->GetPrimaryInstance())->CoolDownGEHanlde;

		auto ActiveGameplayEffectPtr = CharacterPtr->GetAbilitySystemComponent()->GetActiveGameplayEffect(GEHandle);

		TArray<FGameplayEffectSpec> OutSpecCopies;
		CharacterPtr->GetAbilitySystemComponent()->GetAllActiveGameplayEffectSpecs(OutSpecCopies);

		auto f = ActiveGameplayEffectPtr->GetTimeRemaining(CharacterPtr->GetAbilitySystemComponent()->GetActiveGameplayEffects().GetWorldTime());

		for (auto& Iter : OutSpecCopies)
		{
			if (&Iter == &ActiveGameplayEffectPtr->Spec)
			{
			}
			Iter.SetDuration(5, true);
		}

		auto f2 = ActiveGameplayEffectPtr->GetTimeRemaining(CharacterPtr->GetAbilitySystemComponent()->GetActiveGameplayEffects().GetWorldTime());


		UE_LOG(LogTemp, Log, TEXT(""));
	}
}

void TestCommand::TestGAEventModify()
{
	TMap<int32, int32>map;
	for (int Index = 0; Index < 10000; Index++)
	{
		map.Add(Index, Index);
		map.Add(1000 - Index, Index);
	}

	struct MyStruct : public IGAEventModifyInterface
	{
		MyStruct(int32 P, int32 a1a) :
			IGAEventModifyInterface(P)
		{
			aa = a1a;
		}
		int32 aa = 1;
	};

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		CharacterPtr->GetEquipmentItemsComponent()->AddGAEventModify(MakeShared<MyStruct>(123, 523));
		CharacterPtr->GetEquipmentItemsComponent()->AddGAEventModify(MakeShared<MyStruct>(1, 423));
		CharacterPtr->GetEquipmentItemsComponent()->AddGAEventModify(MakeShared<MyStruct>(12, 323));
		CharacterPtr->GetEquipmentItemsComponent()->AddGAEventModify(MakeShared<MyStruct>(13, 223));

		auto d = MakeShared<MyStruct>(14, 1231);
		CharacterPtr->GetEquipmentItemsComponent()->AddGAEventModify(d);
		CharacterPtr->GetEquipmentItemsComponent()->AddGAEventModify(MakeShared<MyStruct>(14, 1232));
		CharacterPtr->GetEquipmentItemsComponent()->AddGAEventModify(MakeShared<MyStruct>(14, 1233));

		CharacterPtr->GetEquipmentItemsComponent()->RemoveGAEventModify(d);


	}

	TSet<int32>set;
	for (int Index = 0; Index < 10000; Index++)
	{
		set.Add(Index);
		set.Add(1000 - Index);
	}

}

void TestCommand::SpawnCharacter(const TArray< FString >& Args)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		auto NewCharacterPtr = GetWorldImp()->SpawnActor<AHumanCharacter>(
			UAssetRefMap::GetInstance()->HumanClass,
			CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * 600),
			FRotator::ZeroRotator
		);

		if (Args.IsValidIndex(0))
		{
			NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().HPReplay.AddCurrentValue(
				UKismetStringLibrary::Conv_StringToInt(Args[0])
			);
		}
		if (Args.IsValidIndex(1))
		{
			NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name = *Args[1];
		}
	}
}

void TestCommand::RecruitCharacter()
{
	auto CharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (!CharacterPtr)
	{
		return;
	}
	auto PlayerCameraManagerPtr = UGameplayStatics::GetPlayerCameraManager(GetWorldImp(), 0);
	if (PlayerCameraManagerPtr)
	{
		FVector OutCamLoc;
		FRotator OutCamRot;
		PlayerCameraManagerPtr->GetCameraViewPoint(OutCamLoc, OutCamRot);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(PawnECC);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CharacterPtr);

		FHitResult OutHit;
		if (GetWorldImp()->LineTraceSingleByObjectType(OutHit, OutCamLoc, OutCamLoc + (OutCamRot.Vector() * 1000), ObjectQueryParams, Params))
		{
			auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
			if (TargetCharacterPtr)
			{
				CharacterPtr->GetGroupMnaggerComponent()->AddCharacterToGroup(TargetCharacterPtr);
			}
		}
	}
}

