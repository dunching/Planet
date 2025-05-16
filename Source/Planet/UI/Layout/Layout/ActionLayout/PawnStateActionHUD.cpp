#include "PawnStateActionHUD.h"

#include "PawnStateActionHUD.h"

#include "Kismet/GameplayStatics.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"

#include "PlanetPlayerState.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "ActionSkillsIcon.h"
#include "CharacterAttibutes.h"
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "MyProgressBar.h"
#include "MyBaseProperty.h"
#include "LogWriter.h"
#include "State_Talent_NuQi.h"
#include "State_Talent_YinYang.h"
#include "GameplayTagsLibrary.h"
#include "InventoryComponent.h"

struct FPawnStateActionHUD : public TStructVariable<FPawnStateActionHUD>
{
	const FName ActiveSkill1 = TEXT("ActiveSkill1");

	const FName ActiveSkill2 = TEXT("ActiveSkill2");

	const FName ActiveSkill3 = TEXT("ActiveSkill3");

	const FName ActiveSkill4 = TEXT("ActiveSkill4");

	const FName WeaponActiveSkill1 = TEXT("WeaponActiveSkill1");

	const FName WeaponActiveSkill2 = TEXT("WeaponActiveSkill2");

	const FName PassiveSkill1 = TEXT("PassiveSkill1");

	const FName MoveSpeed = TEXT("MoveSpeed");

	const FName GAPerformSpeed = TEXT("GAPerformSpeed");

	const FName HP = TEXT("HP");

	const FName PP = TEXT("PP");

	const FName Mana = TEXT("Mana");

	const FName Metal = TEXT("Metal");

	const FName Wood = TEXT("Wood");

	const FName Water = TEXT("Water");

	const FName Fire = TEXT("Fire");

	const FName Earth = TEXT("Earth");
};

void UPawnStateActionHUD::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	BindEvent();
}

void UPawnStateActionHUD::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPawnStateActionHUD::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
	)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	{
		TArray<FName> Ary
		{
			FPawnStateActionHUD::Get().ActiveSkill1,
			FPawnStateActionHUD::Get().ActiveSkill2,
			FPawnStateActionHUD::Get().ActiveSkill3,
			FPawnStateActionHUD::Get().ActiveSkill4,
			FPawnStateActionHUD::Get().WeaponActiveSkill1,
			FPawnStateActionHUD::Get().WeaponActiveSkill2,
		};

		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(Iter));
			if (IconPtr && IconPtr->ProxyPtr)
			{
				IconPtr->UpdateSkillState();
			}
		}
	}
}

void UPawnStateActionHUD::Enable()
{
	ILayoutInterfacetion::Enable();
	
	if (!CharacterPtr)
	{
		return;
	}
	{
		auto CharacterAttributeSetPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		auto AbilitySystemComponentPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		{
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(FPawnStateActionHUD::Get().HP));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(
			                     AbilitySystemComponentPtr,
			                     CharacterAttributeSetPtr->GetHPAttribute(),
			                     CharacterAttributeSetPtr->GetHP(),
			                     CharacterAttributeSetPtr->GetMax_HPAttribute(),
			                     CharacterAttributeSetPtr->GetMax_HP()
			                    );
		}
		{
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(FPawnStateActionHUD::Get().PP));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(
			                     AbilitySystemComponentPtr,
			                     CharacterAttributeSetPtr->GetStaminaAttribute(),
			                     CharacterAttributeSetPtr->GetStamina(),
			                     CharacterAttributeSetPtr->GetMax_StaminaAttribute(),
			                     CharacterAttributeSetPtr->GetMax_Stamina()
			                    );
		}
		{
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(FPawnStateActionHUD::Get().Mana));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(
			                     AbilitySystemComponentPtr,
			                     CharacterAttributeSetPtr->GetManaAttribute(),
			                     CharacterAttributeSetPtr->GetMana(),
			                     CharacterAttributeSetPtr->GetMax_ManaAttribute(),
			                     CharacterAttributeSetPtr->GetMax_Mana()
			                    );
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().MoveSpeed));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(
			                     AbilitySystemComponentPtr,
			                     CharacterAttributeSetPtr->GetMoveSpeedAttribute(),
			                     CharacterAttributeSetPtr->GetMoveSpeed()
			                    );
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().GAPerformSpeed));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(
			                     AbilitySystemComponentPtr,
			                     CharacterAttributeSetPtr->GetPerformSpeedAttribute(),
			                     CharacterAttributeSetPtr->GetPerformSpeed()
			                    );
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Wood));
			if (!UIPtr)
			{
				return;
			}
			// UIPtr->SetDataSource(CharacterAttributes.WoodElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Water));
			if (!UIPtr)
			{
				return;
			}
			// UIPtr->SetDataSource(CharacterAttributes.WaterElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Fire));
			if (!UIPtr)
			{
				return;
			}
			// UIPtr->SetDataSource(CharacterAttributes.FireElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Earth));
			if (!UIPtr)
			{
				return;
			}
			// UIPtr->SetDataSource(CharacterAttributes.SoilElement);
		}
	}

	InitialTalentUI();
	InitialActiveSkillIcon();
	InitialWeaponSkillIcon();
}

void UPawnStateActionHUD::DisEnable()
{
	for (auto Iter : OnAllocationSkillChangedDelegateAry)
	{
		Iter->UnBindCallback();
	}

	if (OnCanAciveSkillChangedHandle)
	{
		OnCanAciveSkillChangedHandle->UnBindCallback();
	}

	ILayoutInterfacetion::DisEnable();
}

void UPawnStateActionHUD::BindEvent()
{
	if (!CharacterPtr)
	{
		return;
	}
	//
	// OnInitaliedGroupSharedInfoHandle = 
	// 	CharacterPtr->OnInitaliedGroupSharedInfo.AddCallback(
	// 		std::bind(&ThisClass::ResetUIByData, this)
	// 	);

	OnAllocationSkillChangedDelegateAry.Add(
	                                        CharacterPtr->GetProxyProcessComponent()->OnCurrentWeaponChanged.
	                                                      AddCallback(
	                                                                  std::bind(
	                                                                            &ThisClass::InitialActiveSkillIcon,
	                                                                            this
	                                                                           )
	                                                                 )
	                                       );

	OnAllocationSkillChangedDelegateAry.Add(
	                                        CharacterPtr->GetProxyProcessComponent()->OnCurrentWeaponChanged.
	                                                      AddCallback(
	                                                                  std::bind(
	                                                                            &ThisClass::InitialWeaponSkillIcon,
	                                                                            this
	                                                                           )
	                                                                 )
	                                       );

	OnCanAciveSkillChangedHandle = CharacterPtr->GetProxyProcessComponent()->OnCanAciveSkillChanged.AddCallback(
		 std::bind(&ThisClass::InitialActiveSkillIcon, this)
		);
}

void UPawnStateActionHUD::InitialTalentUI()
{
}

void UPawnStateActionHUD::InitialActiveSkillIcon()
{
	if (!CharacterPtr)
	{
		return;
	}

	auto SkillsMap = CharacterPtr->GetProxyProcessComponent()->GetCanbeActiveSocket();
	TArray<FName> Ary
	{
		FPawnStateActionHUD::Get().ActiveSkill1,
		FPawnStateActionHUD::Get().ActiveSkill2,
		FPawnStateActionHUD::Get().ActiveSkill3,
		FPawnStateActionHUD::Get().ActiveSkill4,
	};

	for (const auto& Iter : Ary)
	{
		auto SkillIcon = Cast<UActionSkillsIcon>(GetWidgetFromName(Iter));
		if (SkillIcon)
		{
			auto SocketIter = CharacterPtr->GetProxyProcessComponent()->FindActiveSkillBySocket(SkillIcon->IconSocket);
			if (SocketIter)
			{
			}
			else
			{
			}
			SkillIcon->ResetToolUIByData(SocketIter);
		}
	}
}

void UPawnStateActionHUD::InitialWeaponSkillIcon()
{
	if (!bIsActive)
	{
		return;
	}

	if (!CharacterPtr)
	{
		return;
	}

	FCharacterSocket FirstWeaponSocketInfoSPtr;
	FCharacterSocket SecondWeaponSocketInfoSPtr;

	const auto CurrentWeaponSocket = CharacterPtr->GetProxyProcessComponent()->CurrentWeaponSocket;

	if (
		CurrentWeaponSocket.Socket ==
		UGameplayTagsLibrary::WeaponSocket_1
	)
	{
		CharacterPtr->GetProxyProcessComponent()->
		              GetWeaponSocket(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	}
	else if (
		CurrentWeaponSocket.Socket ==
		UGameplayTagsLibrary::WeaponSocket_2
	)
	{
		CharacterPtr->GetProxyProcessComponent()->
		              GetWeaponSocket(SecondWeaponSocketInfoSPtr, FirstWeaponSocketInfoSPtr);
	}
	else
	{
		{
			auto UIPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(FPawnStateActionHUD::Get().WeaponActiveSkill1));
			if (UIPtr)
			{
				UIPtr->ResetToolUIByData(nullptr);
			}
		}
		{
			auto UIPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(FPawnStateActionHUD::Get().WeaponActiveSkill2));
			if (UIPtr)
			{
				UIPtr->ResetToolUIByData(nullptr);
			}
		}
		return;
	}

	{
		auto UIPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(FPawnStateActionHUD::Get().WeaponActiveSkill1));
		if (UIPtr)
		{
			if (FirstWeaponSocketInfoSPtr.IsValid())
			{
				auto SocketIter = CharacterPtr->GetProxyProcessComponent()->FindWeaponSocket(
					 FirstWeaponSocketInfoSPtr.Socket
					);
				if (SocketIter && SocketIter->GetWeaponSkill())
				{
					UIPtr->ResetToolUIByData(SocketIter->GetWeaponSkill());
				}
				else
				{
					UIPtr->ResetToolUIByData(nullptr);
				}
			}
			else
			{
				UIPtr->ResetToolUIByData(nullptr);
			}
		}
	}
	{
		auto UIPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(FPawnStateActionHUD::Get().WeaponActiveSkill2));
		if (UIPtr)
		{
			if (SecondWeaponSocketInfoSPtr.IsValid())
			{
				auto SocketIter = CharacterPtr->GetProxyProcessComponent()->FindWeaponSocket(
					 SecondWeaponSocketInfoSPtr.Socket
					);
				if (SocketIter && SocketIter->GetWeaponSkill())
				{
					UIPtr->ResetToolUIByData(SocketIter->GetWeaponSkill());
				}
				else
				{
					UIPtr->ResetToolUIByData(nullptr);
				}
			}
			else
			{
				UIPtr->ResetToolUIByData(nullptr);
			}
		}
	}
}

void UPawnStateActionHUD::BindElementalData(
		const UAS_Character* CharacterAttributeSetPtr,
		UCharacterAbilitySystemComponent* AbilitySystemComponentPtr
		)
{
	{
		auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Metal));
		if (!UIPtr)
		{
			return;
		}
		UIPtr->SetDataSource(
							 AbilitySystemComponentPtr,
							 CharacterAttributeSetPtr->GetShieldAttribute(),
							 CharacterAttributeSetPtr->GetShield(),
							 CharacterAttributeSetPtr->GetMax_HPAttribute(),
							 CharacterAttributeSetPtr->GetMax_HP()
							);
	}
}
