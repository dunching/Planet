
#include "TestCommand.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/StreamableManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Components/SplineComponent.h"
#include "Kismet/KismetStringLibrary.h"

#include "HumanCharacter.h"
#include "GameInstance/PlanetGameInstance.h"
#include "HoldingItemsComponent.h"
#include "AssetRefMap.h"
#include <AssetRefrencePath.h>
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
#include "InteractiveBaseGAComponent.h"
#include "HorseCharacter.h"
#include "PlanetEditor_Tools.h"

void TestCommand::TestAsyncAssetLoad()
{
}

void TestCommand::TestContainer()
{
	// 	auto Class = LoadClass<UAssetRefMap>(nullptr, *AssetRefMapClass.ToString());
	// 	auto CacheAssetManagerPtr = USubsystemBlueprintLibrary::GetEngineSubsystem(Class);
	{
		std::map<int32, TFunction<void(bool)>>Map;

		for (int32 Index = 0; Index < 10000; Index++)
		{
			Map.emplace(Index, [](bool) {

				UE_LOG(LogTemp, Warning, TEXT("123"));

				});
		}

		for (const auto& Iter : Map)
		{
			Iter.second(true);
		}

		UE_LOG(LogTemp, Warning, TEXT("123"));
	}
	{
		TArray<TTuple<int32, TFunction<void(bool)>>>Map;

		for (int32 Index = 0; Index < 10000; Index++)
		{
			Map.Add({ Index, [](bool) {

				UE_LOG(LogTemp, Warning, TEXT("123"));

				} });
		}

		for (const auto& Iter : Map)
		{
			Iter.Get<1>()(true);
		}

		UE_LOG(LogTemp, Warning, TEXT("123"));
	}
	{
		TMap<int32, TFunction<void(bool)>>Map;

		for (int32 Index = 0; Index < 10000; Index++)
		{
			Map.Add(Index, [](bool) {

				UE_LOG(LogTemp, Warning, TEXT("123"));

				});
		}

		for (const auto& Iter : Map)
		{
			Iter.Value(true);
		}

		UE_LOG(LogTemp, Warning, TEXT("123"));
	}
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
		auto UnitClass = UAssetRefMap::GetInstance()->SkillToolsMap[ESkillUnitType::kHumanSkill_Active_Displacement];

		Handle1 = CharacterPtr->GetAbilitySystemComponent()->K2_GiveAbility(UnitClass.GetDefaultObject()->SkillClass);
		CharacterPtr->GetAbilitySystemComponent()->TryActivateAbility(Handle1);
	}
}

void TestCommand::TestCooldown()
{
}

void TestCommand::TestGAEventModify()
{
	TMap<int32, int32>map;
	for (int Index = 0; Index < 10000; Index++)
	{
		map.Add(Index, Index);
		map.Add(1000 - Index, Index);
	}

	struct MyStruct : public IGAEventModifySendInterface
	{
		MyStruct(int32 P, int32 a1a) :
			IGAEventModifySendInterface(P)
		{
			aa = a1a;
		}
		int32 aa = 1;
	};

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		auto InteractiveBaseGAComponentPtr = CharacterPtr->GetInteractiveBaseGAComponent();
		InteractiveBaseGAComponentPtr->AddSendEventModify(MakeShared<MyStruct>(123, 523));
		InteractiveBaseGAComponentPtr->AddSendEventModify(MakeShared<MyStruct>(1, 423));
		InteractiveBaseGAComponentPtr->AddSendEventModify(MakeShared<MyStruct>(12, 323));
		InteractiveBaseGAComponentPtr->AddSendEventModify(MakeShared<MyStruct>(13, 223));

		auto d = MakeShared<MyStruct>(14, 1231);
		InteractiveBaseGAComponentPtr->AddSendEventModify(d);
		InteractiveBaseGAComponentPtr->AddSendEventModify(MakeShared<MyStruct>(14, 1232));
		InteractiveBaseGAComponentPtr->AddSendEventModify(MakeShared<MyStruct>(14, 1233));

		InteractiveBaseGAComponentPtr->RemoveSendEventModify(d);


	}

	TSet<int32>set;
	for (int Index = 0; Index < 10000; Index++)
	{
		set.Add(Index);
		set.Add(1000 - Index);
	}

}

void TestCommand::SpawnHumanCharacter(const TArray< FString >& Args)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.CustomPreSpawnInitalization = [&](AActor* ActorPtr)
			{
				if (Args.IsValidIndex(1))
				{
					auto NewCharacterPtr = Cast<AHumanCharacter>(ActorPtr);
					NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name = *Args[1];
				}
			};

		auto NewCharacterPtr = GetWorldImp()->SpawnActor<AHumanCharacter>(
			UAssetRefMap::GetInstance()->HumanClass,
			CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * 600),
			FRotator::ZeroRotator,
			SpawnParameters
		);

		if (Args.IsValidIndex(0))
		{
			NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().HPReplay.AddCurrentValue(
				UKismetStringLibrary::Conv_StringToInt(Args[0]),
				NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().PropertuModify_GUID
			);
		}

		if (Args.IsValidIndex(2))
		{
			if (Args[2] == TEXT("1"))
			{
				NewCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper()->SwitchTeammateOption(ETeammateOption::kEnemy);
			}
			else if (Args[2] == TEXT("2"))
			{
				NewCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper()->SwitchTeammateOption(ETeammateOption::kTest);
			}
		}
	}
}

void TestCommand::SpawnHorseCharacter(const TArray< FString >& Args)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		FActorSpawnParameters SpawnParameters;

		auto NewCharacterPtr = GetWorldImp()->SpawnActor<AHorseCharacter>(
			UAssetRefMap::GetInstance()->HorseClass,
			CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * 600),
			FRotator::ZeroRotator,
			SpawnParameters
		);
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
		ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

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

void TestCommand::ModifyWuXingProperty(const TArray< FString >& Args)
{
	if (!Args.IsValidIndex(2))
	{
		return;
	}

	AHumanCharacter* TargetCharacterPtr = nullptr;
	if (Args[0] == TEXT("0"))
	{
		TargetCharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
		if (!TargetCharacterPtr)
		{
			return;
		}
	}
	else
	{
		auto PlayerCameraManagerPtr = UGameplayStatics::GetPlayerCameraManager(GetWorldImp(), 0);
		if (PlayerCameraManagerPtr)
		{
			FVector OutCamLoc;
			FRotator OutCamRot;
			PlayerCameraManagerPtr->GetCameraViewPoint(OutCamLoc, OutCamRot);

			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

			FCollisionQueryParams Params;

			auto CharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
			Params.AddIgnoredActor(CharacterPtr);

			FHitResult OutHit;
			if (GetWorldImp()->LineTraceSingleByObjectType(OutHit, OutCamLoc, OutCamLoc + (OutCamRot.Vector() * 1000), ObjectQueryParams, Params))
			{
				TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
				if (!TargetCharacterPtr)
				{
					return;
				}
			}
		}
	}

	auto Value = UKismetStringLibrary::Conv_StringToInt(Args[2]);
	auto& CharacterAttributes = TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	if (Args[1] == TEXT("1"))
	{
		CharacterAttributes.Element.GoldElement.SetCurrentValue(Value, CharacterAttributes.PropertuModify_GUID);
	}
	else if (Args[1] == TEXT("2"))
	{
		CharacterAttributes.Element.WoodElement.SetCurrentValue(Value, CharacterAttributes.PropertuModify_GUID);
	}
	else if (Args[1] == TEXT("3"))
	{
		CharacterAttributes.Element.WaterElement.SetCurrentValue(Value, CharacterAttributes.PropertuModify_GUID);
	}
	else if (Args[1] == TEXT("4"))
	{
		CharacterAttributes.Element.FireElement.SetCurrentValue(Value, CharacterAttributes.PropertuModify_GUID);
	}
	else if (Args[1] == TEXT("5"))
	{
		CharacterAttributes.Element.SoilElement.SetCurrentValue(Value, CharacterAttributes.PropertuModify_GUID);
	}
	else if (Args[1] == TEXT("6"))
	{
		CharacterAttributes.CriticalHitRate.SetCurrentValue(Value, CharacterAttributes.PropertuModify_GUID);
	}
	else if (Args[1] == TEXT("7"))
	{
		CharacterAttributes.HitRate.SetCurrentValue(Value, CharacterAttributes.PropertuModify_GUID);
	}
	else if (Args[1] == TEXT("8"))
	{
		CharacterAttributes.Evade.SetCurrentValue(Value, CharacterAttributes.PropertuModify_GUID);
	}
}