
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
#include <StateTagExtendInfo.h>
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
#include "PlanetControllerInterface.h"
#include "InteractiveBaseGAComponent.h"
#include "HorseCharacter.h"
#include "PlanetEditor_Tools.h"
#include "CS_RootMotion.h"
#include "CS_PeriodicPropertyModify.h"
#include "KismetCollisionHelper.h"
#include "PlanetPlayerCameraManager.h"
#include "GravityPlayerController.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"

void TestCommand::CopyID2RowName()
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();

	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit.LoadSynchronous();

	TArray<FTableRowUnit*> OutRowArray;
	DataTable->GetAllRows<FTableRowUnit>(TEXT("FPCGDataTableRowToParamDataTest"), OutRowArray);

	for (const auto Iter : OutRowArray)
	{
	}
}

void TestCommand::TestPlayerCharacterMoveTo(const TArray< FString >& Args)
{
	if (Args.IsValidIndex(0))
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorldImp(), AActor::StaticClass(), *Args[0], OutActors);

		if (OutActors.IsValidIndex(0))
		{
			auto PCPtr = Cast<AGravityPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
			if (PCPtr)
			{
				PCPtr->MoveToLocation(OutActors[0]->GetActorLocation(), 50);
			}
		}
	}
}

void TestCommand::TestCameraManager(const TArray< FString >& Args)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorldImp(), AActor::StaticClass(), TEXT("CameraTest"), OutActors);

	auto CameraManagerPtr = Cast<APlanetPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorldImp(), 0));
	if (CameraManagerPtr && OutActors.IsValidIndex(0) && Args.IsValidIndex(0))
	{
		FViewTargetTransitionParams TransitionParams;

		TransitionParams.BlendTime = 1.f;

		if (Args[0] == TEXT("0"))
		{
			CameraManagerPtr->SetViewTarget(OutActors[0], TransitionParams);
		}
		else
		{
			CameraManagerPtr->SetViewTarget(CameraManagerPtr->GetOwningPlayerController()->GetPawn(), TransitionParams);
		}
	}
}

void TestCommand::TestSectorCollision()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams Params;

	UKismetCollisionHelper::OverlapMultiSectorByObjectType(GetWorldImp(), FVector(100, 100, 100), FVector(200, 100, 100), 90, 9, ObjectQueryParams, Params);
}

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
		// 		auto UnitClass = USceneUnitExtendInfoMap::GetInstance()->SkillToolsMap[ESkillUnitType::kHumanSkill_Active_Displacement];
		// 
		// 		Handle1 = CharacterPtr->GetAbilitySystemComponent()->K2_GiveAbility(UnitClass.GetDefaultObject()->SkillClass);
		// 		CharacterPtr->GetAbilitySystemComponent()->TryActivateAbility(Handle1);
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
				auto CharacterPtr = Cast<AHumanCharacter>(ActorPtr);
				if (Args.IsValidIndex(3))
				{
					CharacterPtr->RowName = FGameplayTag::RequestGameplayTag(*Args[3]);
				}
			};

		auto NewCharacterPtr = GetWorldImp()->SpawnActor<AHumanCharacter>(
			UAssetRefMap::GetInstance()->TestNPC_HumanClass,
			CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * 600),
			FRotator::ZeroRotator,
			SpawnParameters
		);

		if (!NewCharacterPtr)
		{
			return;
		}

		if (Args.IsValidIndex(1))
		{
			NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->Name = *Args[1];
		}

		if (Args.IsValidIndex(0))
		{
			NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->HPReplay.AddCurrentValue(
				UKismetStringLibrary::Conv_StringToInt(Args[0]),
				UGameplayTagsSubSystem::GetInstance()->DataSource_Regular
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
				CharacterPtr->GetGroupMnaggerComponent()->AddCharacterToGroup(TargetCharacterPtr->GetCharacterUnit());
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
	auto CharacterAttributesSPtr = TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	if (Args[1] == TEXT("1"))
	{
		CharacterAttributesSPtr->GoldElement.SetCurrentValue(Value, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
	}
	else if (Args[1] == TEXT("2"))
	{
		CharacterAttributesSPtr->WoodElement.SetCurrentValue(Value, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
	}
	else if (Args[1] == TEXT("3"))
	{
		CharacterAttributesSPtr->WaterElement.SetCurrentValue(Value, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
	}
	else if (Args[1] == TEXT("4"))
	{
		CharacterAttributesSPtr->FireElement.SetCurrentValue(Value, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
	}
	else if (Args[1] == TEXT("5"))
	{
		CharacterAttributesSPtr->SoilElement.SetCurrentValue(Value, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
	}
	else if (Args[1] == TEXT("6"))
	{
		CharacterAttributesSPtr->CriticalHitRate.SetCurrentValue(Value, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
	}
	else if (Args[1] == TEXT("7"))
	{
		CharacterAttributesSPtr->HitRate.SetCurrentValue(Value, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
	}
	else if (Args[1] == TEXT("8"))
	{
		CharacterAttributesSPtr->Evade.SetCurrentValue(Value, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
	}
}

void TestCommand::TestGAState2Self(const TArray< FString >& Args)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	auto CharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));

	float Duration = 10.f;
	LexFromString(Duration, *Args[1]);
	auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_RootMotion(
		FGameplayTag::RequestGameplayTag(*Args[0])
	);

	GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
	GameplayAbilityTargetDataPtr->TargetCharacterPtr = CharacterPtr;

	auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
	ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
}

void TestCommand::TestGATagState2Target(const TArray< FString >& Args)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

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
				float Duration = 10.f;
				LexFromString(Duration, *Args[1]);
				auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_RootMotion(
					FGameplayTag::RequestGameplayTag(*Args[0])
				);

				GameplayAbilityTargetDataPtr->TriggerCharacterPtr = TargetCharacterPtr;
				GameplayAbilityTargetDataPtr->TargetCharacterPtr = CharacterPtr;

				auto ICPtr = TargetCharacterPtr->GetInteractiveBaseGAComponent();
				ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
			}
		}
	}
}
