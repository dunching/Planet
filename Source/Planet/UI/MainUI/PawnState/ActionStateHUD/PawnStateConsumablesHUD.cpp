
#include "PawnStateConsumablesHUD.h"

#include <Components/GridPanel.h>
#include <Kismet/GameplayStatics.h>

#include "ActionConsumablesIcon.h"
#include "CharacterBase.h"
#include "InteractiveConsumablesComponent.h"

struct FPawnStateConsumablesHUD : public TStructVariable<FPawnStateConsumablesHUD>
{
	const FName GridPanel = TEXT("GridPanel");
};

void UPawnStateConsumablesHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UPawnStateConsumablesHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	{
		auto UIPtr = Cast<UGridPanel>(GetWidgetFromName(FPawnStateConsumablesHUD::Get().GridPanel));
		auto Ary = UIPtr->GetAllChildren();

		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<UActionConsumablesIcon>(Iter);
			if (IconPtr && IconPtr->UnitPtr)
			{
				IconPtr->UpdateState();
			}
		}
	}
}

void UPawnStateConsumablesHUD::ResetUIByData()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetInteractiveConsumablesComponent();

	auto UIPtr = Cast<UGridPanel>(GetWidgetFromName(FPawnStateConsumablesHUD::Get().GridPanel));
	auto Ary = UIPtr->GetAllChildren();

	for (const auto& Iter : Ary)
	{
		auto IconPtr = Cast<UActionConsumablesIcon>(Iter);
		if (IconPtr)
		{
			const auto ConsumableSocketInfoSPtr = EICPtr->FindConsumable(IconPtr->IconSocket);
			if (ConsumableSocketInfoSPtr)
			{
				IconPtr->ResetToolUIByData(ConsumableSocketInfoSPtr->UnitPtr);
			}
		}
	}
}
