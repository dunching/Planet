#include "RegularActionLayout.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Border.h"
#include "Slate/SGameLayerManager.h"

#include "TemplateHelper.h"
#include "PlanetPlayerController.h"
#include "UIManagerSubSystem.h"
#include "EffectsList.h"
#include "CharacterBase.h"
#include "FocusIcon.h"
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterRisingTips.h"
#include "EventSubjectComponent.h"
#include "UICommon.h"
#include "FocusTitle.h"
#include "GameOptions.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "GuideActor.h"
#include "GuideList.h"
#include "GuideSubSystem.h"
#include "GuideThread.h"
#include "GuideThreadChallenge.h"
#include "HUD_TeamInfo.h"
#include "HumanCharacter_Player.h"
#include "MainHUDLayout.h"
#include "PawnStateActionHUD.h"
#include "PawnStateConsumablesHUD.h"
#include "PlanetGameViewportClient.h"
#include "PlayerGameplayTasks.h"
#include "ProgressTips.h"
#include "TeamMatesHelperComponent.h"
#include "UpgradePromt.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"

struct FRegularActionLayout : public TStructVariable<FRegularActionLayout>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnStateActionHUD = TEXT("PawnStateActionHUD");

	FName LowerHPSocket = TEXT("LowerHPSocket");

	FName PawnStateConsumablesHUD = TEXT("PawnStateConsumablesHUD");

	FName QuitChallengeBtn = TEXT("QuitChallengeBtn");

	FName CharacterRisingTipsCavans = TEXT("CharacterRisingTipsCavans");

	FName TeamInfo = TEXT("TeamInfo");

	FName EffectsList = TEXT("EffectsList");

	FName GuideList = TEXT("GuideList");

	FName FocusTitle = TEXT("FocusTitle");
};

void URegularActionLayout::NativeConstruct()
{
	Super::NativeConstruct();
}

void URegularActionLayout::NativeDestruct()
{
	Super::NativeDestruct();
}

void URegularActionLayout::Enable()
{
	ILayoutInterfacetion::Enable();

	{
		auto UIPtr = Cast<UPawnStateActionHUD>(GetWidgetFromName(FRegularActionLayout::Get().PawnStateActionHUD));
		if (UIPtr)
		{
			UIPtr->Enable();
		}
	}
	{
		auto UIPtr = Cast<UPawnStateConsumablesHUD>(
		                                            GetWidgetFromName(
		                                                              FRegularActionLayout::Get().
		                                                              PawnStateConsumablesHUD
		                                                             )
		                                           );
		if (UIPtr)
		{
			UIPtr->Enable();
		}
	}
	{
		auto UIPtr = Cast<UGuideList>(GetWidgetFromName(FRegularActionLayout::Get().GuideList));
		if (UIPtr)
		{
			UIPtr->Enable();
		}
	}

	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacterPtr)
	{
		DisplayTeamInfo(true);

		InitialEffectsList(true);

		ViewProgressTips(false);

		OnFocusCharacter(nullptr);

		FocusCharacterDelegateSPtr =
			PlayerCharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->OnFocusCharacterDelegate.
			                    AddCallback(
			                                std::bind(&ThisClass::OnFocusCharacter, this, std::placeholders::_1)
			                               );

		auto CharacterAttributesRef =
			PlayerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

		Max_HPChangedDelegateHandle = PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->
		                                                  GetGameplayAttributeValueChangeDelegate(
			                                                   CharacterAttributesRef->GetMax_HPAttribute()
			                                                  ).AddUObject(this, &ThisClass::OnHPChanged);

		HPChangedDelegateHandle = PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->
		                                              GetGameplayAttributeValueChangeDelegate(
			                                               CharacterAttributesRef->GetHPAttribute()
			                                              ).AddUObject(this, &ThisClass::OnHPChanged);

		EffectOhterCharacterCallbackDelegate = PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->
		                                                           MakedDamageDelegate.
		                                                           AddCallback(
		                                                                       std::bind(
			                                                                        &ThisClass::OnEffectOhterCharacter,
			                                                                        this,
			                                                                        std::placeholders::_1
			                                                                       )
		                                                                      );

		auto CharacterProxySPtr = PlayerCharacterPtr->GetCharacterProxy();
		if (!CharacterProxySPtr)
		{
			return;
		}

		LevelChangedDelegateHandle = CharacterProxySPtr->LevelChangedDelegate.AddOnValueChanged(
			 std::bind(&ThisClass::OnLevelChanged, this, std::placeholders::_2)
			);
	}

	{
		StartGuideDelegateHandle = UGuideSubSystem::GetInstance()->GetOnStartGuide().AddUObject(
			 this,
			 &ThisClass::OnStartGuide
			);
		StopGuideDelegateHandle = UGuideSubSystem::GetInstance()->GetOnStopGuide().AddUObject(
			 this,
			 &ThisClass::OnStopGuide
			);

		auto UIPtr = Cast<UButton>(GetWidgetFromName(FRegularActionLayout::Get().QuitChallengeBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnQuitChallengeBtnClicked);

			auto GroupManaggerGASPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()))
			                           ->
			                           GetGroupManagger()->GetAbilitySystemComponent();
			if (GroupManaggerGASPtr->HasMatchingGameplayTag(UGameplayTagsLibrary::State_ActiveGuide_Challenge))
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void URegularActionLayout::DisEnable()
{
	{
		auto UIPtr = Cast<UPawnStateActionHUD>(GetWidgetFromName(FRegularActionLayout::Get().PawnStateActionHUD));
		if (UIPtr)
		{
			UIPtr->DisEnable();
		}
	}

	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacterPtr)
	{
		DisplayTeamInfo(false);

		InitialEffectsList(false);

		if (FocusCharacterDelegateSPtr)
		{
			FocusCharacterDelegateSPtr->UnBindCallback();
		}

		auto CharacterAttributesRef =
			PlayerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

		PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
			 CharacterAttributesRef->GetMax_HPAttribute()
			).Remove(Max_HPChangedDelegateHandle);

		PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
			 CharacterAttributesRef->GetHPAttribute()
			).Remove(HPChangedDelegateHandle);
	}

	if (EffectOhterCharacterCallbackDelegate)
	{
		EffectOhterCharacterCallbackDelegate->UnBindCallback();
	}

	if (FocusCharacterDelegateSPtr)
	{
		FocusCharacterDelegateSPtr->UnBindCallback();
	}

	if (LevelChangedDelegateHandle)
	{
		LevelChangedDelegateHandle->UnBindCallback();
	}

	{
		UGuideSubSystem::GetInstance()->GetOnStartGuide().Remove(StartGuideDelegateHandle);
		UGuideSubSystem::GetInstance()->GetOnStopGuide().Remove(StopGuideDelegateHandle);

		auto UIPtr = Cast<UButton>(GetWidgetFromName(FRegularActionLayout::Get().QuitChallengeBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.RemoveDynamic(this, &ThisClass::OnQuitChallengeBtnClicked);
		}
	}

	ILayoutInterfacetion::DisEnable();
}

ELayoutCommon URegularActionLayout::GetLayoutType() const
{
	return ELayoutCommon::kActionLayout;
}

void URegularActionLayout::OnStartGuide(
	AGuideThread* GuideThread
	)
{
	if (GuideThread)
	{
		if (GuideThread->IsA(AGuideThread_Challenge::StaticClass()))
		{
			auto UIPtr = Cast<UButton>(GetWidgetFromName(FRegularActionLayout::Get().QuitChallengeBtn));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void URegularActionLayout::OnStopGuide(
	AGuideThread* GuideThread
	)
{
	if (GuideThread)
	{
		if (GuideThread->IsA(AGuideThread_Challenge::StaticClass()))
		{
			auto UIPtr = Cast<UButton>(GetWidgetFromName(FRegularActionLayout::Get().QuitChallengeBtn));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void URegularActionLayout::OnQuitChallengeBtnClicked()
{
	// 停止挑战任务
	UGuideSubSystem::GetInstance()->StopParallelGuideThread(
	                                                        UAssetRefMap::GetInstance()->GuideThreadChallengeActorClass
	                                                       );

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}

	PCPtr->GetGameplayTasksComponent()->TeleportPlayerToOpenWorld();
}

void URegularActionLayout::OnFocusCharacter(
	ACharacterBase* TargetCharacterPtr
	)
{
	auto UIPtr = Cast<UFocusTitle>(GetWidgetFromName(FRegularActionLayout::Get().FocusTitle));
	if (!UIPtr)
	{
		return;
	}

	auto ClearPrevious = [this, TargetCharacterPtr, UIPtr]
	{
		if (FocusIconPtr)
		{
			auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
				 GetWorld(),
				 TargetPointSharedLayerName
				);
			if (ScreenLayer)
			{
				ScreenLayer->RemoveHoverWidget(FocusIconPtr);
			}

			FocusIconPtr = nullptr;
		}

		if (TargetCharacterPtr)
		{
		}
		else
		{
			UIPtr->SetVisibility(ESlateVisibility::Hidden);
		}
	};

	if (TargetCharacterPtr)
	{
		if (TargetCharacterPtr != PreviousTargetCharacterPtr)
		{
			ClearPrevious();
			PreviousTargetCharacterPtr = TargetCharacterPtr;
		}

		// 悬浮的锁定UI
		auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
			 GetWorld(),
			 TargetPointSharedLayerName
			);
		if (ScreenLayer)
		{
			auto AssetRefMapPtr = UAssetRefMap::GetInstance();
			auto TempFocusIconPtr = CreateWidget<UFocusIcon>(GetWorldImp(), AssetRefMapPtr->FocusIconClass);
			if (TempFocusIconPtr)
			{
				TempFocusIconPtr->TargetCharacterPtr = TargetCharacterPtr;
				TempFocusIconPtr->OnNativeDestruct.AddUObject(this, &ThisClass::OnFocusDestruct);
				// FocusIconPtr->AddToViewport(EUIOrder::kFocus);

				FocusIconPtr = TempFocusIconPtr;

				ScreenLayer->AddHoverWidget(TempFocusIconPtr);
			}
		}

		// 上方的状态条
		if (!UIPtr)
		{
			UIPtr->SetVisibility(ESlateVisibility::Visible);
		}
		if (UIPtr)
		{
			UIPtr->SetTargetCharacter(TargetCharacterPtr);
		}
	}
	else
	{
		ClearPrevious();
	}
}

UEffectsList* URegularActionLayout::InitialEffectsList(
	bool bIsDisplay
	)
{
	auto UIPtr = Cast<UEffectsList>(GetWidgetFromName(FRegularActionLayout::Get().EffectsList));
	if (!UIPtr)
	{
		return nullptr;
	}

	// 绑定效果状态栏
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (CharacterPtr)
	{
		if (bIsDisplay)
		{
			UIPtr->BindCharacterState(CharacterPtr);
			UIPtr->Enable();
		}
		else
		{
			UIPtr->DisEnable();
		}
	}

	return UIPtr;
}

void URegularActionLayout::DisplayTeamInfo(
	bool bIsDisplay,
	AHumanCharacter* HumanCharacterPtr
	)
{
	auto UIPtr = Cast<UHUD_TeamInfo>(GetWidgetFromName(FRegularActionLayout::Get().TeamInfo));
	if (!UIPtr)
	{
		return;
	}

	if (bIsDisplay)
	{
		UIPtr->Enable();
	}
	else
	{
		UIPtr->DisEnable();
	}
}

UProgressTips* URegularActionLayout::ViewProgressTips(
	bool bIsViewMenus
	)
{
	return nullptr;
}

void URegularActionLayout::OnHPChanged(
	const FOnAttributeChangeData&


	
	)
{
	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacterPtr)
	{
		return;
	}

	auto CharacterAttributesRef = PlayerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

	const auto CurrentValue =
		CharacterAttributesRef->GetHP();
	const auto MaxValue =
		CharacterAttributesRef->GetMax_HP();
	const auto LowerHP_Percent =
		UGameOptions::GetInstance()->LowerHP_Percent;

	const auto bIsLowerHP = CurrentValue < (MaxValue * (LowerHP_Percent / 100.f));

	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FRegularActionLayout::Get().LowerHPSocket));
	if (!BorderPtr)
	{
		return;
	}

	BorderPtr->SetVisibility(bIsLowerHP ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void URegularActionLayout::OnFocusDestruct(
	UUserWidget* UIPtr
	)
{
	auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
		 GetWorld(),
		 TargetPointSharedLayerName
		);
	if (ScreenLayer)
	{
		ScreenLayer->RemoveHoverWidget(FocusIconPtr);
	}

	FocusIconPtr = nullptr;
}

void URegularActionLayout::OnEffectOhterCharacter(
	const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
	)
{
	auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
		 GetWorld(),
		 TargetPointSharedLayerName
		);
	if (ScreenLayer)
	{
		auto WidgetPtr = CreateWidget<UCharacterRisingTips>(this, FightingTipsClass);
		if (WidgetPtr)
		{
			if (WidgetPtr->SetData(ReceivedEventModifyDataCallback))
			{
				ScreenLayer->AddHoverWidget(WidgetPtr);
			}
		}
	}
}

void URegularActionLayout::OnLevelChanged(
	int32 Level
	)
{
	if (UUIManagerSubSystem::GetInstance()->GetMainHUDLayout())
	{
		UUIManagerSubSystem::GetInstance()->GetMainHUDLayout()->DisplayWidget(
		                                                                      UpgradePromtClass,
		                                                                      [Level](
		                                                                      UUserWidget* WidgetPtr
		                                                                      )
		                                                                      {
			                                                                      auto UIPtr = Cast<UUpgradePromt>(
				                                                                       WidgetPtr
				                                                                      );
			                                                                      if (UIPtr)
			                                                                      {
				                                                                      UIPtr->SetLevel(Level);
			                                                                      }
		                                                                      }
		                                                                     );
	}
}
