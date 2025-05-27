#include "ViewSetting.h"

#include "Components/Slider.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "GameOptions.h"
#include "Planet_Tools.h"
#include "Tools.h"

struct FViewSetting : public TStructVariable<FViewSetting>
{
	const FName VolumeSlider = TEXT("VolumeSlider");
};

inline void UViewSetting::EnableMenu()
{
	{
		auto UIPtr = Cast<USlider>(GetWidgetFromName(FViewSetting::Get().VolumeSlider));
		if (UIPtr)
		{
			UIPtr->SetValue(UGameOptions::GetInstance()->VolumeMultiplier);
			UIPtr->OnValueChanged.AddDynamic(this, &ThisClass::OnValueChanged);
		}
	}
}

void UViewSetting::DisEnableMenu()
{
	{
		auto UIPtr = Cast<USlider>(GetWidgetFromName(FViewSetting::Get().VolumeSlider));
		if (UIPtr)
		{
			UIPtr->OnValueChanged.RemoveDynamic(this, &ThisClass::OnValueChanged);
		}
	}
}

inline EMenuType UViewSetting::GetMenuType() const
{
	return EMenuType::kViewSetting;
}

void UViewSetting::OnValueChanged(
	float Value
	)
{
	if (FMath::IsNearlyZero(Value))
	{
		// 如果为0，音乐会停止，再把值改为大于0的值 音乐不会重新播放
		Value = 0.01f;
	}
	
	UGameplayStatics::SetSoundMixClassOverride(
											   GetWorldImp(),
											   UAssetRefMap::GetInstance()->SoundMixRef.LoadSynchronous(),
											   UAssetRefMap::GetInstance()->BGMSoundClassRef.LoadSynchronous(),
											   Value
											  );
}
