#include "Weapon_Base.h"

void AWeapon_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AWeapon_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWeapon_Base::AttachToCharacter(ACharacter* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);
}
