
#include "RegularActionLayout.h"

#include "Kismet/GameplayStatics.h"

#include "TemplateHelper.h"
#include "PlanetPlayerController.h"
#include "UIManagerSubSystem.h"
#include "EffectsList.h"
#include "CharacterBase.h"

struct FMainUILayout : public TStructVariable<FMainUILayout>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName FocusCharacterSocket = TEXT("FocusCharacterSocket");
};

void URegularActionLayout::NativeConstruct()
{
	Super::NativeConstruct();

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PCPtr)
	{
		// 绑定效果状态栏
		auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
		if (EffectPtr)
		{
			EffectPtr->BindCharacterState(PCPtr->GetPawn<ACharacterBase>());
		}
	}
}