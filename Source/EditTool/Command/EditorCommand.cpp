#include "EditorCommand.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "WorldPartition/WorldPartitionMiniMap.h"
#include "WorldPartition/WorldPartitionMiniMapVolume.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"

#include "HumanCharacter.h"
#include "AssetRefMap.h"
#include "SPlineActor.h"
#include "ItemProxy_Minimal.h"
#include "Talent_FASI.h"
#include "CharacterBase.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAbilitySystemComponent.h"
#include "HorseCharacter.h"
#include "KismetCollisionHelper.h"
#include "PlanetPlayerCameraManager.h"
#include "GravityPlayerController.h"
#include "DataTableCollection.h"
#include "JsonObjectConverter.h"
#include "PlanetPlayerController.h"
#include "Tools.h"
#include "Value.h"

void EditorCommand::CopyID2RowName()
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();

	// auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Proxy.LoadSynchronous();
	//
	// TArray<FTableRowProxy*> OutRowArray;
	// DataTable->GetAllRows<FTableRowProxy>(TEXT("FPCGDataTableRowToParamDataTest"), OutRowArray);

	// for (const auto Iter : OutRowArray)
	// {
	// }
}

void EditorCommand::TestPlayerCharacterMoveTo(
	const TArray<FString>& Args
	)
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
				//				PCPtr->MoveToLocation(OutActors[0]->GetActorLocation(), 50);
			}
		}
	}
}

void EditorCommand::TestCameraManager(
	const TArray<FString>& Args
	)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorldImp(), AActor::StaticClass(), TEXT("CameraTest"), OutActors);

	auto CameraManagerPtr = Cast<APlanetPlayerCameraManager>(
	                                                         UGameplayStatics::GetPlayerCameraManager(GetWorldImp(), 0)
	                                                        );
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

void EditorCommand::TestSectorCollision()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams Params;

	UKismetCollisionHelper::OverlapMultiSectorByObjectType(
	                                                       GetWorldImp(),
	                                                       FVector(100, 100, 100),
	                                                       FVector(200, 100, 100),
	                                                       90,
	                                                       9,
	                                                       ObjectQueryParams,
	                                                       Params
	                                                      );
}

void EditorCommand::TestAsyncAssetLoad()
{
}

void EditorCommand::TestContainer()
{
	// 	auto Class = LoadClass<UAssetRefMap>(nullptr, *AssetRefMapClass.ToString());
	// 	auto CacheAssetManagerPtr = USubsystemBlueprintLibrary::GetEngineSubsystem(Class);
	{
		std::map<int32, TFunction<void(
			         bool
			         )>> Map;

		for (int32 Index = 0; Index < 10000; Index++)
		{
			Map.emplace(
			            Index,
			            [](
			            bool
			            )
			            {
				            UE_LOG(LogTemp, Warning, TEXT("123"));
			            }
			           );
		}

		for (const auto& Iter : Map)
		{
			Iter.second(true);
		}

		UE_LOG(LogTemp, Warning, TEXT("123"));
	}
	{
		TArray<TTuple<int32, TFunction<void(
			              bool
			              )>>> Map;

		for (int32 Index = 0; Index < 10000; Index++)
		{
			Map.Add(
			        {
				        Index, [](
				        bool
				        )
				        {
					        UE_LOG(LogTemp, Warning, TEXT("123"));
				        }
			        }
			       );
		}

		for (const auto& Iter : Map)
		{
			Iter.Get<1>()(true);
		}

		UE_LOG(LogTemp, Warning, TEXT("123"));
	}
	{
		TMap<int32, TFunction<void(
			     bool
			     )>> Map;

		for (int32 Index = 0; Index < 10000; Index++)
		{
			Map.Add(
			        Index,
			        [](
			        bool
			        )
			        {
				        UE_LOG(LogTemp, Warning, TEXT("123"));
			        }
			       );
		}

		for (const auto& Iter : Map)
		{
			Iter.Value(true);
		}

		UE_LOG(LogTemp, Warning, TEXT("123"));
	}
}

void EditorCommand::TestSpline()
{
	TArray<AActor*> Ary;
	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ACharacterBase::StaticClass(), Ary);

	if (Ary.IsValidIndex(1))
	{
		auto CharacterPtr = Cast<ACharacterBase>(Ary[0]);
		auto TargetCharacterPtr = Cast<ACharacterBase>(Ary[1]);

		const auto Pt1 = CharacterPtr->GetActorLocation();
		const auto Pt2 = TargetCharacterPtr->GetActorLocation();
		const auto Pt3 = CharacterPtr->GetActorLocation() + (
			                 (TargetCharacterPtr->GetActorLocation() - CharacterPtr->GetActorLocation()) / 2);

		const auto MidPt = Pt3 - (CharacterPtr->GetGravityDirection() * 100.f);

		auto SPlineActorPtr = GetWorldImp()->SpawnActor<ASPlineActor>();
		SPlineActorPtr->SplineComponentPtr->ClearSplinePoints();

		SPlineActorPtr->SplineComponentPtr->AddSplinePoint(Pt1, ESplineCoordinateSpace::World);
		SPlineActorPtr->SplineComponentPtr->AddSplinePoint(MidPt, ESplineCoordinateSpace::World);
		SPlineActorPtr->SplineComponentPtr->AddSplinePoint(Pt2, ESplineCoordinateSpace::World);

		SPlineActorPtr->SplineComponentPtr->SetTangentsAtSplinePoint(
		                                                             0,
		                                                             FVector::ZeroVector,
		                                                             FVector::ZeroVector,
		                                                             ESplineCoordinateSpace::World
		                                                            );

		SPlineActorPtr->SplineComponentPtr->SetTangentsAtSplinePoint(
		                                                             2,
		                                                             FVector::ZeroVector,
		                                                             FVector::ZeroVector,
		                                                             ESplineCoordinateSpace::World
		                                                            );
	}
}

FGameplayAbilitySpecHandle Handle1;

void EditorCommand::TestDisplacementSkill()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		// 		auto UnitClass = USceneUnitExtendInfoMap::GetInstance()->SkillToolsMap[ESkillUnitType::kHumanSkill_Active_Displacement];
		// 
		// 		Handle1 = CharacterPtr->GetCharacterAbilitySystemComponent()->K2_GiveAbility(UnitClass.GetDefaultObject()->SkillClass);
		// 		CharacterPtr->GetCharacterAbilitySystemComponent()->TryActivateAbility(Handle1);
	}
}

void EditorCommand::TestCooldown()
{
}

void EditorCommand::TestGAEventModify()
{
	TMap<int32, int32> map;
	for (int Index = 0; Index < 10000; Index++)
	{
		map.Add(Index, Index);
		map.Add(1000 - Index, Index);
	}

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
	}

	TSet<int32> set;
	for (int Index = 0; Index < 10000; Index++)
	{
		set.Add(Index);
		set.Add(1000 - Index);
	}
}

void EditorCommand::SpawnHumanCharacter(
	const TArray<FString>& Args
	)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.CustomPreSpawnInitalization = [&](
			AActor* ActorPtr
			)
			{
				auto CharacterPtr = Cast<AHumanCharacter>(ActorPtr);
				if (Args.IsValidIndex(3))
				{
					// CharacterPtr->CharacterGrowthAttribute = FGameplayTag::RequestGameplayTag(*Args[3]);
				}
			};

		auto NewCharacterPtr = GetWorldImp()->SpawnActor<AHumanCharacter>(
		                                                                  UAssetRefMap::GetInstance()->
		                                                                  TestNPC_HumanClass,
		                                                                  CharacterPtr->GetActorLocation() + (
			                                                                  CharacterPtr->GetActorForwardVector() *
			                                                                  600),
		                                                                  FRotator::ZeroRotator,
		                                                                  SpawnParameters
		                                                                 );

		if (!NewCharacterPtr)
		{
			return;
		}

		if (Args.IsValidIndex(1))
		{
			// NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name = *Args[1];
		}

		if (Args.IsValidIndex(0))
		{
			// NewCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().HP_Replay.AddCurrentValue(
			// 	UKismetStringLibrary::Conv_StringToInt(Args[0]),
			// 	UGameplayTagsLibrary::DataSource_Regular
			// );
		}

		if (Args.IsValidIndex(2))
		{
			if (Args[2] == TEXT("1"))
			{
				//				NewCharacterPtr->GetGroupManagger()->GetTeamHelper()->SwitchTeammateOption(ETeammateOption::kEnemy);
			}
			else if (Args[2] == TEXT("2"))
			{
				//			NewCharacterPtr->GetGroupManagger()->GetTeamHelper()->SwitchTeammateOption(ETeammateOption::kTest);
			}
		}
	}
}

void EditorCommand::SpawnHorseCharacter(
	const TArray<FString>& Args
	)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (CharacterPtr)
	{
		FActorSpawnParameters SpawnParameters;

		auto NewCharacterPtr = GetWorldImp()->SpawnActor<AHorseCharacter>(
		                                                                  UAssetRefMap::GetInstance()->HorseClass,
		                                                                  CharacterPtr->GetActorLocation() + (
			                                                                  CharacterPtr->GetActorForwardVector() *
			                                                                  600),
		                                                                  FRotator::ZeroRotator,
		                                                                  SpawnParameters
		                                                                 );
	}
}

void EditorCommand::RecruitCharacter()
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
		if (GetWorldImp()->LineTraceSingleByObjectType(
		                                               OutHit,
		                                               OutCamLoc,
		                                               OutCamLoc + (OutCamRot.Vector() * 1000),
		                                               ObjectQueryParams,
		                                               Params
		                                              ))
		{
			auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
			if (TargetCharacterPtr)
			{
			}
		}
	}
}

void EditorCommand::ModifyWuXingProperty(
	const TArray<FString>& Args
	)
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
			if (GetWorldImp()->LineTraceSingleByObjectType(
			                                               OutHit,
			                                               OutCamLoc,
			                                               OutCamLoc + (OutCamRot.Vector() * 1000),
			                                               ObjectQueryParams,
			                                               Params
			                                              ))
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
	auto CharacterAttributes = TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	// if (Args[1] == TEXT("1"))
	// {
	// 	CharacterAttributes.GoldElement.SetCurrentValue(Value, UGameplayTagsLibrary::DataSource_Regular);
	// }
	// else if (Args[1] == TEXT("2"))
	// {
	// 	CharacterAttributes.WoodElement.SetCurrentValue(Value, UGameplayTagsLibrary::DataSource_Regular);
	// }
	// else if (Args[1] == TEXT("3"))
	// {
	// 	CharacterAttributes.WaterElement.SetCurrentValue(Value, UGameplayTagsLibrary::DataSource_Regular);
	// }
	// else if (Args[1] == TEXT("4"))
	// {
	// 	CharacterAttributes.FireElement.SetCurrentValue(Value, UGameplayTagsLibrary::DataSource_Regular);
	// }
	// else if (Args[1] == TEXT("5"))
	// {
	// 	CharacterAttributes.SoilElement.SetCurrentValue(Value, UGameplayTagsLibrary::DataSource_Regular);
	// }
	// else if (Args[1] == TEXT("6"))
	// {
	// 	CharacterAttributes.CriticalHitRate.SetCurrentValue(Value, UGameplayTagsLibrary::DataSource_Regular);
	// }
	// else if (Args[1] == TEXT("7"))
	// {
	// 	CharacterAttributes.HitRate.SetCurrentValue(Value, UGameplayTagsLibrary::DataSource_Regular);
	// }
	// else if (Args[1] == TEXT("8"))
	// {
	// 	CharacterAttributes.Evade.SetCurrentValue(Value, UGameplayTagsLibrary::DataSource_Regular);
	// }
}

void EditorCommand::TestGAState2Self(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	auto CharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));

	float Duration = 10.f;
	LexFromString(Duration, *Args[1]);

	auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
}

void EditorCommand::TestGATagState2Target(
	const TArray<FString>& Args
	)
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
		if (GetWorldImp()->LineTraceSingleByObjectType(
		                                               OutHit,
		                                               OutCamLoc,
		                                               OutCamLoc + (OutCamRot.Vector() * 1000),
		                                               ObjectQueryParams,
		                                               Params
		                                              ))
		{
			auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
			if (TargetCharacterPtr)
			{
				float Duration = 10.f;
				LexFromString(Duration, *Args[1]);
				auto ICPtr = TargetCharacterPtr->GetCharacterAbilitySystemComponent();
			}
		}
	}
}

void EditorCommand::MakeTrueDamege(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->MakeTrueDamege(Args);
}

void EditorCommand::MakeTrueDamegeInArea(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->MakeTrueDamegeInArea(Args);
}

void EditorCommand::MakeTherapy(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->MakeTherapy(Args);
}

void EditorCommand::ReplyStamina(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->ReplyStamina(Args);
}

void EditorCommand::MakeRespawn(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->MakeRespawn(Args);
}

void EditorCommand::AddShield(
	const TArray<FString>& Args
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->AddShield(Args);
}

void EditorCommand::AddShieldToTarget(
	const TArray<FString>& Args
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->AddShieldToTarget(Args);
}

void EditorCommand::ModifyElementalData(
	const TArray<FString>& Args
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->ModifyElementalData(Args);
}

void EditorCommand::ModifyElementalDataToTarget(
	const TArray<FString>& Args
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->ModifyElementalDataToTarget(Args);
}

void EditorCommand::ExportMinimapTexture()
{
	auto World = GEditor->GetEditorWorldContext().World();
	{
		// 纹理
		auto ActorPtr = Cast<AWorldPartitionMiniMap>(UGameplayStatics::GetActorOfClass(World, AWorldPartitionMiniMap::StaticClass()));
		if (!ActorPtr)
		{
			return;
		}
		FString PicturePath = FPaths::ProjectSavedDir() + TEXT("Minimap.png");

		auto Texture = ActorPtr->MiniMapTexture;
	
		// record old settings
		TextureCompressionSettings OldCompressionSettings = Texture->CompressionSettings;
		TextureMipGenSettings OldMipGenSettings = Texture->MipGenSettings;
		bool OldSRGB = Texture->SRGB;

		// modified to exportable settings
		Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
		Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
		Texture->SRGB = false;
		Texture->UpdateResource();

		// export texture to image
		FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
		uint8* TextureData = (uint8*) Mip.BulkData.Lock(LOCK_READ_WRITE);
		Mip.BulkData.Unlock();

		int32 SizeX = Texture->GetPlatformData()->SizeX;
		int32 SizeY = Texture->GetPlatformData()->SizeY;
		TArray<FColor> ColorData;
		for (int32 IndexY = 0; IndexY < SizeY; IndexY++)
		{
			for (int32 IndexX = 0; IndexX < SizeX; IndexX++)
			{
				FColor PixelColor;
				PixelColor.B = TextureData[(IndexY * SizeX + IndexX) * 4 + 0];
				PixelColor.G = TextureData[(IndexY * SizeX + IndexX) * 4 + 1];
				PixelColor.R = TextureData[(IndexY * SizeX + IndexX) * 4 + 2];
				PixelColor.A = TextureData[(IndexY * SizeX + IndexX) * 4 + 3];
				ColorData.Add(PixelColor);
			}
		}

		TArray64<uint8> ImageData;
		FImageUtils::PNGCompressImageArray(SizeX, SizeY, ColorData, ImageData);
		FFileHelper::SaveArrayToFile(ImageData, *PicturePath);

		// return to old settings
		Texture->CompressionSettings = OldCompressionSettings;
		Texture->MipGenSettings = OldMipGenSettings;
		Texture->SRGB = OldSRGB;
		Texture->UpdateResource();
	}
	{
		// 大小
		auto ActorPtr = Cast<AWorldPartitionMiniMapVolume>(UGameplayStatics::GetActorOfClass(World, AWorldPartitionMiniMapVolume::StaticClass()));
		if (!ActorPtr)
		{
			return;
		}
		
		const auto Box = ActorPtr->GetBounds();

		// 创建 JSON 对象
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		
		JsonObject->SetStringField(TEXT("Origin"), Box.Origin.ToString());
		JsonObject->SetStringField(TEXT("BoxExtent"), Box.BoxExtent.ToString());
		
		// 序列化为 JSON 字符串
		FString JsonString;
		TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		// 写入文件
		const auto Path = GetOpenWorldBoundBox();
		FFileHelper::SaveStringToFile(JsonString, *Path);
	}
}
