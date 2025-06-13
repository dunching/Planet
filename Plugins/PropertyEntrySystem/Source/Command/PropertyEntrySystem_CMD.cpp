
#include "PropertyEntrySystem_CMD.h"

#include "Kismet/KismetStringLibrary.h"

#include "PropertyEntrySussystem.h"

void TestGenerationPropertyEntry(
	const TArray<FString>& Args
	)
{
	TArray<int32> LevelAry;
	for (const auto&Iter:Args)
	{
		LevelAry.Add(UKismetStringLibrary::Conv_StringToInt(Iter));
	}

	const auto Value = UPropertyEntrySussystem::GetInstance()->GenerationPropertyEntry(LevelAry);
}
