// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "SVoxelNotification.h"
#include "MessageLogModule.h"
#include "Logging/MessageLog.h"

class FVoxelMessagesEditor : public FVoxelEditorSingleton
{
public:
	//~ Begin FVoxelEditorSingleton Interface
	virtual void Initialize() override
	{
		FVoxelMessages::OnMessageLogged.AddLambda([this](const TSharedRef<FTokenizedMessage>& Message)
		{
			LogMessage(Message);
		});

		FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
		FMessageLogInitializationOptions InitOptions;
		InitOptions.bShowFilters = true;
		InitOptions.bShowPages = false;
		InitOptions.bAllowClear = true;
		MessageLogModule.RegisterLogListing("Voxel", INVTEXT("Voxel"), InitOptions);
	}
	//~ End FVoxelEditorSingleton Interface

public:
	void LogMessage(const TSharedRef<FTokenizedMessage>& Message)
	{
		if (GEditor->PlayWorld ||
			GIsPlayInEditorWorld)
		{
			FMessageLog("PIE").AddMessage(Message);
		}

		FMessageLog("Voxel").AddMessage(Message);

		const FText MessageText = Message->ToText();
		const double Time = FPlatformTime::Seconds();

		bool bFoundRecentMessage = false;
		for (int32 Index = 0; Index < RecentMessages.Num(); Index++)
		{
			FRecentMessage& RecentMessage = RecentMessages[Index];
			if (RecentMessage.LastTime + 5 < Time)
			{
				RecentMessages.RemoveAtSwap(Index);
				Index--;
				continue;
			}

			if (RecentMessage.Text.EqualTo(MessageText))
			{
				bFoundRecentMessage = true;
				RecentMessage.LastTime = Time;
				continue;
			}
		}

		if (!bFoundRecentMessage)
		{
			RecentMessages.Add(FRecentMessage
			{
				MessageText,
				Time
			});
		}

		if (RecentMessages.Num() > 3)
		{
			for (const TSharedPtr<FNotification>& Notification : Notifications)
			{
				if (const TSharedPtr<SNotificationItem> Item = Notification->WeakItem.Pin())
				{
					Item->SetExpireDuration(0);
					Item->SetFadeOutDuration(0);
					Item->ExpireAndFadeout();
				}
			}
			Notifications.Reset();

			const FText ErrorText = FText::FromString(FString::FromInt(RecentMessages.Num()) + " voxel errors");

			if (const TSharedPtr<SNotificationItem> Item = WeakGlobalNotification.Pin())
			{
				Item->SetText(ErrorText);
				// Reset expiration
				Item->ExpireAndFadeout();
				return;
			}

			FNotificationInfo Info = FNotificationInfo(ErrorText);
			Info.CheckBoxState = ECheckBoxState::Unchecked;
			Info.ExpireDuration = 10;
			Info.WidthOverride = FOptionalSize();

			Info.ButtonDetails.Add(FNotificationButtonInfo(
				INVTEXT("Dismiss"),
				FText(),
				MakeLambdaDelegate([this]
				{
					if (const TSharedPtr<SNotificationItem> Item = WeakGlobalNotification.Pin())
					{
						Item->SetFadeOutDuration(0);
						Item->SetFadeOutDuration(0);
						Item->ExpireAndFadeout();
					}
				}),
				SNotificationItem::CS_Fail));

			Info.ButtonDetails.Add(FNotificationButtonInfo(
				INVTEXT("Show Message Log"),
				FText(),
				MakeLambdaDelegate([this]
				{
					FMessageLogModule& MessageLogModule = FModuleManager::GetModuleChecked<FMessageLogModule>("MessageLog");
					MessageLogModule.OpenMessageLog("Voxel");
				}),
				SNotificationItem::CS_Fail));

			const TSharedPtr<SNotificationItem> GlobalNotification = FSlateNotificationManager::Get().AddNotification(Info);
			if (!ensure(GlobalNotification))
			{
				return;
			}

			GlobalNotification->SetCompletionState(SNotificationItem::CS_Fail);;
			WeakGlobalNotification = GlobalNotification;

			return;
		}

		for (int32 Index = 0; Index < Notifications.Num(); Index++)
		{
			const TSharedPtr<FNotification> Notification = Notifications[Index];
			const TSharedPtr<SNotificationItem> Item = Notification->WeakItem.Pin();
			if (!Item.IsValid())
			{
				Notifications.RemoveAtSwap(Index);
				Index--;
				continue;
			}

			if (!Notification->Text.EqualToCaseIgnored(MessageText))
			{
				continue;
			}

			Notification->Count++;
			Item->ExpireAndFadeout();
			return;
		}

		const TSharedRef<FNotification> Notification = MakeVoxelShared<FNotification>();
		Notification->Text = MessageText;

		FNotificationInfo Info = FNotificationInfo(MessageText);
		Info.CheckBoxState = ECheckBoxState::Unchecked;
		Info.ExpireDuration = 10;
		Info.WidthOverride = FOptionalSize();
		Info.ContentWidget =
			SNew(SVoxelNotification, Message)
			.Count_Lambda([=]
			{
				return Notification->Count;
			});

		Notification->WeakItem = FSlateNotificationManager::Get().AddNotification(Info);
		Notifications.Add(Notification);
	}

private:
	TWeakPtr<SNotificationItem> WeakGlobalNotification;

	struct FRecentMessage
	{
		FText Text;
		double LastTime = 0;
	};
	TVoxelArray<FRecentMessage> RecentMessages;

	struct FNotification
	{
		TWeakPtr<SNotificationItem> WeakItem;
		FText Text;
		int32 Count = 1;
	};
	TVoxelArray<TSharedPtr<FNotification>> Notifications;
};
FVoxelMessagesEditor* GVoxelMessagesEditor = MakeVoxelSingleton(FVoxelMessagesEditor);