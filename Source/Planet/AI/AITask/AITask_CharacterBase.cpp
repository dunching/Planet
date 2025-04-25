
#include "AITask_CharacterBase.h"

inline void UAITask_CharacterBase::SetUpController(
	AHumanCharacter_AI* InPlayerCharacterPtr,
	AHumanAIController* InControllerPtr
)
{
	PlayerCharacterPtr = InPlayerCharacterPtr;
	ControllerPtr = InControllerPtr;
}
