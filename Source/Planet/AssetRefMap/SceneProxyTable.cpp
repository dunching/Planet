#include "SceneProxyTable.h"

UItemProxy_Description_Character::UItemProxy_Description_Character(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	CharacterGrowthAttributeAry.SetNum(27);
}
