// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelAddContentWidget.h"
#include "SVoxelAddContentTile.h"
#include "VoxelExampleContentManager.h"

#include "SPrimaryButton.h"
#include "SWidgetCarouselWithNavigation.h"
#include "Compression/OodleDataCompressionUtil.h"

#include "HttpModule.h"
#include "PlatformHttp.h"
#include "PackageTools.h"
#include "UObject/Linker.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void SVoxelAddContentWidget::Construct(const FArguments& InArgs)
{
	Filter = MakeVoxelShared<FContentSourceTextFilter>(
		FContentSourceTextFilter::FItemToStringArray::CreateLambda([](const TSharedPtr<FVoxelExampleContent> Item, TArray<FString>& Array)
		{
			Array.Add(Item->Name);
		})
	);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.f, 8.f, 0.f, 8.f))
				[
					SAssignNew(SearchBox, SSearchBox)
					.OnTextChanged(this, &SVoxelAddContentWidget::SearchTextChanged)
				]
				+ SVerticalBox::Slot()
				[
					CreateContentTilesView()
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(FMargin(18.f, 0.f, 0.f, 0.f))
			.AutoWidth()
			[
				SNew(SSeparator)
				.Orientation(Orient_Vertical)
				.Thickness(2.f)
			]
			+ SHorizontalBox::Slot()
			[
				SAssignNew(ContentDetailsBox, SBox)
				[
					CreateContentDetails(SelectedContent)
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
			.Thickness(2.f)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 16.f, 0.f, 16.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(SPrimaryButton)
				.OnClicked(this, &SVoxelAddContentWidget::AddButtonClicked)
				.Text(INVTEXT("Add to Project"))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(8.f, 0.f, 0.f, 0.f)
			[
				SNew(SButton)
				.TextStyle(FAppStyle::Get(), "DialogButtonText")
				.OnClicked(this, &SVoxelAddContentWidget::CancelButtonClicked)
				.Text(INVTEXT("Cancel"))
			]
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FReply SVoxelAddContentWidget::AddButtonClicked()
{
	const TSharedPtr<SWindow> AddContentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	if (!ensure(AddContentWindow))
	{
		return FReply::Handled();
	}

	AddContentWindow->RequestDestroyWindow();

	const TArray<TSharedPtr<FVoxelExampleContent>> Items = ContentTilesView->GetSelectedItems();
	ensure(Items.Num() <= 1);
	if (Items.Num() != 1)
	{
		return FReply::Handled();
	}

	const FVoxelExampleContent& Content = *Items[0];

	FString Url = "https://api.voxelplugin.dev/content/download";
	Url += "?name=" + FPlatformHttp::UrlEncode(Content.Name);
	Url += "&version=" + Content.Version;

	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr, const FHttpResponsePtr Response, const bool bConnectedSuccessfully)
	{
		if (!bConnectedSuccessfully ||
			!Response ||
			Response->GetResponseCode() != 200)
		{
			VOXEL_MESSAGE(Error, "Failed to query content url: {0} {1}", Response->GetResponseCode(), Response->GetContentAsString());
			return;
		}

		const FString ContentString = Response->GetContentAsString();

		TSharedPtr<FJsonValue> ParsedValue;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
		if (!ensure(FJsonSerializer::Deserialize(Reader, ParsedValue)) ||
			!ensure(ParsedValue))
		{
			return;
		}

		const TSharedPtr<FJsonObject> ContentObject = ParsedValue->AsObject();
		if (!ensure(ContentObject))
		{
			return;
		}

		const FString DownloadUrl = ContentObject->GetStringField("url");
		const int32 DownloadSize = ContentObject->GetNumberField("size");
		const FString Hash = ContentObject->GetStringField("hash");

		FNotificationInfo Info(FText::FromString("Downloading " + Content.Name));
		Info.bFireAndForget = false;
		const TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);

		const TSharedRef<IHttpRequest> NewRequest = FHttpModule::Get().CreateRequest();
		NewRequest->SetURL(DownloadUrl);
		NewRequest->SetVerb("GET");
		NewRequest->OnRequestProgress().BindLambda([=](FHttpRequestPtr, int32 BytesSent, int32 BytesReceived)
		{
			Notification->SetSubText(FText::FromString(FString::Printf(
				TEXT("%.1f/%.1fMB"),
				BytesReceived / double(1 << 20),
				DownloadSize / double(1 << 20))));
		});
		NewRequest->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr, const FHttpResponsePtr NewResponse, const bool bNewConnectedSuccessfully)
		{
			Notification->ExpireAndFadeout();

			if (!bNewConnectedSuccessfully ||
				!NewResponse ||
				NewResponse->GetResponseCode() != 200)
			{
				VOXEL_MESSAGE(Error, "Failed to download content: {0}", NewResponse->GetResponseCode());
				return;
			}

			const TArray<uint8> Result = NewResponse->GetContent();

			if (!ensure(FSHA1::HashBuffer(Result.GetData(), Result.Num()).ToString() == Hash))
			{
				VOXEL_MESSAGE(Error, "Failed to download {0}: invalid hash", Content.Name);
				return;
			}

			FinalizeDownload(Result);
		});
		NewRequest->ProcessRequest();
	});
	Request->ProcessRequest();

	return FReply::Handled();
}

FReply SVoxelAddContentWidget::CancelButtonClicked()
{
	const TSharedPtr<SWindow> AddContentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	if (!ensure(AddContentWindow))
	{
		return FReply::Handled();
	}

	AddContentWindow->RequestDestroyWindow();

	return FReply::Handled();
}

void SVoxelAddContentWidget::SearchTextChanged(const FText& Text)
{
	Filter->SetRawFilterText(Text);
	UpdateFilteredItems();
}

TSharedRef<SWidget> SVoxelAddContentWidget::CreateContentTilesView()
{
	ContentsSourceList = FVoxelExampleContentManager::Get().GetExamples();

	SAssignNew(ContentTilesView, STileView<TSharedPtr<FVoxelExampleContent>>)
	.ListItemsSource(&FilteredList)
	.OnGenerateTile(this, &SVoxelAddContentWidget::CreateContentSourceIconTile)
	.OnSelectionChanged_Lambda([this](TSharedPtr<FVoxelExampleContent> NewSelection, ESelectInfo::Type SelectInfo)
	{
		SelectedContent = NewSelection;

		if (ContentDetailsBox)
		{
			ContentDetailsBox->SetContent(CreateContentDetails(NewSelection));
		}
	})
	.ClearSelectionOnClick(false)
	.ItemAlignment(EListItemAlignment::LeftAligned)
	.ItemWidth(102.f)
	.ItemHeight(153.f)
	.SelectionMode(ESelectionMode::Single);

	UpdateFilteredItems();

	return ContentTilesView.ToSharedRef();
}

TSharedRef<SWidget> SVoxelAddContentWidget::CreateContentDetails(const TSharedPtr<FVoxelExampleContent>& Selection) const
{
	return
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		.Padding(FMargin(0.f, 0.f, 0.f, 5.f))
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFitX)
			[
				SNew(SImage)
				.Image_Lambda([=]() -> const FSlateBrush*
				{
					const ISlateBrushSource* BrushSource = Selection->Image->Get();
					if (!BrushSource)
					{
						return nullptr;
					}
					return BrushSource->GetSlateBrush();
				})
			]
		]
		+ SScrollBox::Slot()
		.Padding(FMargin(10.f, 0.f, 0.f, 5.f))
		[
			SNew(STextBlock)
			.TextStyle(FAppStyle::Get(), "DialogButtonText")
			.Font(FAppStyle::Get().GetFontStyle("HeadingExtraSmall"))
			.ColorAndOpacity(FStyleColors::ForegroundHover)
			.Text(FText::FromString(Selection->DisplayName))
			.AutoWrapText(true)
		]
		+ SScrollBox::Slot()
		.Padding(FMargin(10.f, 0.f, 0.f, 5.f))
		[
			SNew(STextBlock)
			.Text(FText::FromString(Selection->Description))
			.AutoWrapText(true)
		];
}

TSharedRef<ITableRow> SVoxelAddContentWidget::CreateContentSourceIconTile(const TSharedPtr<FVoxelExampleContent> Content, const TSharedRef<STableViewBase>& OwnerTable) const
{
	return
		SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		.Style(FAppStyle::Get(), "ProjectBrowser.TableRow")
		.Padding(2.f)
		[
			SNew(SVoxelAddContentTile)
			.Image_Lambda([=]() -> const FSlateBrush*
			{
				const ISlateBrushSource* BrushSource = Content->Thumbnail->Get();
				if (!BrushSource)
				{
					return nullptr;
				}
				return BrushSource->GetSlateBrush();
			})
			.DisplayName(FText::FromString(Content->DisplayName))
			.IsSelected_Lambda([this, WeakContent = MakeWeakPtr(Content)]
			{
				return WeakContent == SelectedContent;
			})
		];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelAddContentWidget::UpdateFilteredItems()
{
	FilteredList = ContentsSourceList.FilterByPredicate([&](const TSharedPtr<FVoxelExampleContent>& Content)
	{
		return Filter->PassesFilter(Content);
	});

	ContentTilesView->RequestListRefresh();

	if (FilteredList.IsEmpty())
	{
		ContentTilesView->SetSelection(nullptr, ESelectInfo::Direct);
		return;
	}

	if (!FilteredList.Contains(SelectedContent))
	{
		ContentTilesView->SetSelection(FilteredList[0], ESelectInfo::Direct);
	}
	else
	{
		ContentTilesView->SetSelection(SelectedContent, ESelectInfo::Direct);
	}
}

void SVoxelAddContentWidget::FinalizeDownload(const TArray<uint8>& InData) const
{
	TArray<uint8> UncompressedData;
	if (!FOodleCompressedArray::DecompressToTArray(UncompressedData, InData))
	{
		VOXEL_MESSAGE(Error, "Failed to download content: decompression failed");
		return;
	}

	TMap<FString, TArray<uint8>> Files;
	const FString ZipError = FVoxelSystemUtilities::Unzip(UncompressedData, Files);
	if (!ZipError.IsEmpty() ||
		!ensure(Files.Num() > 0))
	{
		VOXEL_MESSAGE(Error, "Failed to download content: unzip failed: {0}", ZipError);
		return;
	}

	TSet<FString> PackageNames;
	TSet<FString> ExistingPackageNames;
	for (const auto& It : Files)
	{
		if (!ensure(It.Key.StartsWith("Content")))
		{
			VOXEL_MESSAGE(Error, "Failed to download content: invalid path {0}", It.Key);
			return;
		}

		const FString Path = FPaths::ProjectDir() / It.Key;

		FString PackageName;
		if (!ensure(FPackageName::TryConvertFilenameToLongPackageName(Path, PackageName)))
		{
			continue;
		}

		PackageNames.Add(PackageName);

		if (FPaths::FileExists(Path))
		{
			ExistingPackageNames.Add(PackageName);
		}
	}

	if (ExistingPackageNames.Num() > 0)
	{
		FString Text = "The following assets already exist:\n";
		for (const FString& Path : ExistingPackageNames)
		{
			Text += Path + "\n";
		}
		Text += "\nDo you want to overwrite them?";

		const EAppReturnType::Type Return = FMessageDialog::Open(EAppMsgType::YesNoCancel, FText::FromString(Text));
		if (Return == EAppReturnType::Cancel)
		{
			return;
		}
		if (Return == EAppReturnType::No)
		{
			for (const FString& PackageName : ExistingPackageNames)
			{
				ensure(PackageNames.Remove(PackageName));
			}
		}
	}

	TArray<UPackage*> LoadedPackages;
	for (const FString& PackageName : PackageNames)
	{
		UPackage* Package = FindPackage(nullptr, *PackageName);
		if (!Package)
		{
			continue;
		}

		LoadedPackages.Add(Package);
	}

	for (UPackage* Package : LoadedPackages)
	{
		if (!Package->IsFullyLoaded())
		{
			FlushAsyncLoading();
			Package->FullyLoad();
		}
		ResetLoaders(Package);
	}

	for (const auto& It : Files)
	{
		const FString Path = FPaths::ProjectDir() / It.Key;

		FString PackageName;
		if (!ensure(FPackageName::TryConvertFilenameToLongPackageName(Path, PackageName)))
		{
			continue;
		}

		if (!PackageNames.Contains(PackageName))
		{
			continue;
		}

		if (!FFileHelper::SaveArrayToFile(It.Value, *Path))
		{
			VOXEL_MESSAGE(Error, "Failed to write {0}", Path);
		}
	}

	if (LoadedPackages.Num() > 0)
	{
		FText ErrorMessage;
		if (!UPackageTools::ReloadPackages(LoadedPackages, ErrorMessage, EReloadPackagesInteractionMode::AssumePositive))
		{
			VOXEL_MESSAGE(Error, "Failed to reload packages: {0}", ErrorMessage);
		}
	}

	VOXEL_MESSAGE(Info, "Content added");

	FVoxelSystemUtilities::DelayedCall([=]
	{
		TArray<UObject*> AssetObjects;
		for (const auto& It : Files)
		{
			FString PackageName;
			if (!ensure(FPackageName::TryConvertFilenameToLongPackageName(
				FPaths::ProjectDir() / It.Key,
				PackageName)))
			{
				continue;
			}

			const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);

			TArray<FAssetData> AssetDatas;
			AssetRegistryModule.Get().GetAssetsByPackageName(*PackageName, AssetDatas);

			for (const FAssetData& AssetData : AssetDatas)
			{
				if (UObject* AssetObject = AssetData.GetAsset())
				{
					AssetObjects.Add(AssetObject);
				}
			}
		}

		if (!ensure(AssetObjects.Num() > 0))
		{
			return;
		}

		for (UObject* Object : AssetObjects)
		{
			if (Object->IsA<UWorld>())
			{
				AssetObjects = { Object };
				GEditor->SyncBrowserToObjects({ AssetObjects });
				return;
			}
		}

		GEditor->SyncBrowserToObjects(AssetObjects);
	}, 1.f);
}