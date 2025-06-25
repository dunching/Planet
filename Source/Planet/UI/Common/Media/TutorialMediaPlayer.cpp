#include "TutorialMediaPlayer.h"

#include "Components/Button.h"

void UTutorialMediaPlayer::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmBtn)
	{
		ConfirmBtn->OnClicked.AddDynamic(this, &ThisClass::OnConfirmBtnClicked);
	}
}

void UTutorialMediaPlayer::Display()
{
	DisplayVideo();
}

void UTutorialMediaPlayer::SetOnClicked(
	const std::function<void()>& InOnClicked
	)
{
	auto DelegateHdnel = OnClicked.AddCallback(InOnClicked);
	DelegateHdnel->bIsAutoUnregister = false;
}

inline void UTutorialMediaPlayer::OnConfirmBtnClicked()
{
	RemoveFromParent();
	
	OnClicked();
}

