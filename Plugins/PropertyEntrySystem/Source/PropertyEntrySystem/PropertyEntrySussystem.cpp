#include "PropertyEntrySussystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Tools.h"

bool UPropertyEntrySussystem::ShouldCreateSubsystem(
	UObject* Outer
	) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

UPropertyEntrySussystem* UPropertyEntrySussystem::GetInstance()
{
	return Cast<UPropertyEntrySussystem>(
	                                     USubsystemBlueprintLibrary::GetWorldSubsystem(
		                                      GetWorldImp(),
		                                      UPropertyEntrySussystem::StaticClass()
		                                     )
	                                    );
}

TArray<FGeneratedPropertyEntryInfo> UPropertyEntrySussystem::GenerationPropertyEntry(
	const TArray<int32>& LevelAry
	)
{
	GetCheckMap();

	TArray<FGeneratedPropertyEntryInfo> Result;

	for (const auto Iter : LevelAry)
	{
		if (CheckMap.Contains(Iter))
		{
			const auto Ref = CheckMap[Iter];
			int32 TotalWeight = 0;
			for (const auto SecondIter : Ref)
			{
				TotalWeight += SecondIter.Value.Weight;
			}

			if (TotalWeight < 1)
			{
				continue;
			}

			auto WeightIndex = FMath::RandRange(1, TotalWeight);

			for (const auto SecondIter : Ref)
			{
				if (WeightIndex > SecondIter.Value.Weight)
				{
					WeightIndex -= SecondIter.Value.Weight;
				}
				else
				{
					FGeneratedPropertyEntryInfo GeneratedPropertyEntryInfo;

					GeneratedPropertyEntryInfo.PropertyTag = SecondIter.Key;
					GeneratedPropertyEntryInfo.bIsValue = SecondIter.Value.bIsValue;
					if (GeneratedPropertyEntryInfo.bIsValue)
					{
						GeneratedPropertyEntryInfo.Value = FMath::RandRange(
						                                                    SecondIter.Value.MinValue,
						                                                    SecondIter.Value.MaxValue
						                                                   );
					}
					else
					{
						GeneratedPropertyEntryInfo.Percent = FMath::RandRange(
						                                                      SecondIter.Value.MinValue,
						                                                      SecondIter.Value.MaxValue
						                                                     );
					}
					GeneratedPropertyEntryInfo.Level = Iter;

					Result.Add(GeneratedPropertyEntryInfo);
					break;
				}
			}
		}
	}

	return Result;
}

void UPropertyEntrySussystem::GetCheckMap()
{
	auto ReigsterClearCache = [this]
	{
		GetWorld()->GetTimerManager().SetTimer(
		                                       TimerHandle,
		                                       [this]()
		                                       {
			                                       CheckMap.Empty();
			                                       TimerHandle = FTimerHandle();
		                                       },
		                                       CacheTime,
		                                       false
		                                      );
	};

	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(
		                                         TimerHandle
		                                        );
		ReigsterClearCache();
		return;
	}

	auto GetPropertyEntrysDTInterfacePtr = Cast<IGetPropertyEntrysDTInterface>(GetWorldImp()->GetWorldSettings());
	auto PropertyEntrysDTPtr = GetPropertyEntrysDTInterfacePtr->GetPropertyEntrysDT().LoadSynchronous();

	PropertyEntrysDTPtr->ForeachRow<FTableRowProxy_GeneratiblePropertyEntrys>(
	                                                                          TEXT("GetUnit"),
	                                                                          [this](
	                                                                          const FName& Key,
	                                                                          const
	                                                                          FTableRowProxy_GeneratiblePropertyEntrys&
	                                                                          Value
	                                                                          )
	                                                                          {
		                                                                          for (const auto& Iter : Value.Map)
		                                                                          {
			                                                                          if (CheckMap.Contains((Iter.Key)))
			                                                                          {
				                                                                          CheckMap[Iter.Key].Add(
					                                                                           FGameplayTag::RequestGameplayTag(
						                                                                            Key
						                                                                           ),
					                                                                           Iter.Value
					                                                                          );
			                                                                          }
			                                                                          else
			                                                                          {
				                                                                          auto& Ref = CheckMap.Add(
					                                                                           Iter.Key
					                                                                          );
				                                                                          Ref.Add(
					                                                                           FGameplayTag::RequestGameplayTag(
						                                                                            Key
						                                                                           ),
					                                                                           Iter.Value
					                                                                          );
			                                                                          }
		                                                                          }
	                                                                          }
	                                                                         );

	ReigsterClearCache();
}
