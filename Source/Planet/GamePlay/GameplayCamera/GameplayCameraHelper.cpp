#include "GameplayCameraHelper.h"

#include "GameFramework/Character.h"

#include "HumanCharacter_Player.h"
#include "PlayerComponent.h"

void UGameplayCameraHelper::SwitchGaplayCameraType(
	const TObjectPtr<AHumanCharacter_Player>& PlayerCharacterPtr,
	ECameraType CameraType
	)
{
	if (
		PlayerCharacterPtr
	)
	{
		switch (CameraType)
		{
		case ECameraType::kAction:
			break;
		case ECameraType::kDashing:
			break;
		case ECameraType::kRunning:
			break;
		case ECameraType::kReleasing_ActiveSkill_XYFH:
			{
			}
			break;
		}
		
#if UE_EDITOR || UE_CLIENT
		if (
			(PlayerCharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
		{
			PlayerCharacterPtr->GetPlayerComponent()->SetCameraType(CameraType);
		}
#endif
	}
}
