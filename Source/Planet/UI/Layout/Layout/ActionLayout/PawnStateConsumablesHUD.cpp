#include "PawnStateConsumablesHUD.h"

#include <Components/GridPanel.h>
#include <Kismet/GameplayStatics.h>

#include "ActionConsumablesIcon.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "ItemProxy_Consumable.h"

struct FPawnStateConsumablesHUD : public TStructVariable<FPawnStateConsumablesHUD>
{
	const FName GridPanel = TEXT("GridPanel");
};

void UPawnStateConsumablesHUD::NativeConstruct()
{
	Super::NativeConstruct();
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
			if (IconPtr && IconPtr->ProxyPtr)
			{
				IconPtr->UpdateState();
			}
		}
	}
}

void UPawnStateConsumablesHUD::Enable()
{
	ILayoutInterfacetion::Enable();
	
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto ProxyProcessComponentPtr = CharacterPtr->GetProxyProcessComponent();

	auto UIPtr = Cast<UGridPanel>(GetWidgetFromName(FPawnStateConsumablesHUD::Get().GridPanel));
	auto Ary = UIPtr->GetAllChildren();

	for (const auto& Iter : Ary)
	{
		auto IconPtr = Cast<UActionConsumablesIcon>(Iter);
		if (IconPtr)
		{
			const auto ConsumableSocketInfoSPtr = ProxyProcessComponentPtr->
				FindConsumablesBySocket(IconPtr->IconSocket);
			if (ConsumableSocketInfoSPtr)
			{
			}
			IconPtr->ResetToolUIByData(ConsumableSocketInfoSPtr);
		}
	}
}

void UPawnStateConsumablesHUD::DisEnable()
{
	ILayoutInterfacetion::DisEnable();
}
