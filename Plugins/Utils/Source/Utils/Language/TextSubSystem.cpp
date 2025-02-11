
#include "TextSubSystem.h"

#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

UTextSubSystem* UTextSubSystem::GetInstance()
{
	return Cast<UTextSubSystem>(
		USubsystemBlueprintLibrary::GetEngineSubsystem(UTextSubSystem::StaticClass())
	);
}

void UTextSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ReadData();

	// 补充缺失的
	SupplementDefaultData();
}

void UTextSubSystem::ReadData()
{
	FString jsonStr;
	// 加载 Json 文件  
	FString AbsolutePath = FString::Printf(TEXT("%s%s/%s"), *FPaths::ProjectContentDir(), TEXT("Settings"), TEXT("language.json"));
	if (FPaths::FileExists(AbsolutePath))
	{
		if (FFileHelper::LoadFileToString(jsonStr, *AbsolutePath))
		{
			// 创建 Json 阅读器
			TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<TCHAR>::Create(jsonStr);
			// 创建 Json 对象
			TSharedPtr<FJsonObject> jsonObject;
			// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
			FJsonSerializer::Deserialize(jsonReader, jsonObject);

			for (const auto& Iter : jsonObject->Values)
			{
				FString Value;
				switch (CurrentLanguageType)
				{
				case ELanguageType::kChinese:
				{
					const FString Type = TEXT("Chinese");

					Iter.Value->AsObject()->TryGetStringField(Type, Value);
				}
				break;
				case ELanguageType::kEnglish:
					break;
				default:
					break;
				}
				TextMap.Add(Iter.Key, Value);
			}
		}
	}
}

void UTextSubSystem::SupplementDefaultData()
{

}

FString UTextSubSystem::GetText(const FString& Text) const
{
	// ?
	auto Iter = TextMap.Find(Text);
	if (Iter)
	{
		return *Iter;
	}
	else
	{
		checkNoEntry();
		return Text;
	}
}
