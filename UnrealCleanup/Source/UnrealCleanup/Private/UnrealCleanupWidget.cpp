// C 2024 Bianca Bellof @github.com/Finderadeis


#include "UnrealCleanupWidget.h"
#include "SlateBasics.h"
#include "UnrealCleanup.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

#define T_List_All TEXT("List all")
#define T_List_Unused TEXT("List unused")

void SUnrealCleanupWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	CheckBoxArray.Empty();
	SelectedAssetsData.Empty();
	ComboBoxSource.Empty();

	StoredAssetsDataArray = InArgs._StoredAssetsDataArray;
	SelectedFolderPath = InArgs._PathToDisplay;

	DisplayedAssetsDataArray = StoredAssetsDataArray;

	TitleFont = FCoreStyle::Get().GetFontStyle("EmbossedText");
	TitleFont.Size = 20;

	ComboBoxSource.Add(MakeShareable(new FString(T_List_All)));
	ComboBoxSource.Add(MakeShareable(new FString(T_List_Unused)));


	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("UnrealCleanupLayout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->Split
			(
				FTabManager::NewStack()
				->AddTab("Asset Deletion", ETabState::OpenedTab)
				->AddTab("Folder Deletion", ETabState::OpenedTab)
				->AddTab("Asset Renaming", ETabState::OpenedTab)
				->SetForegroundTab(FName("Unreal Cleanup Utilities"))
			)
		);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner("Asset Deletion",
													  FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args) -> TSharedRef<SDockTab>
																				{
																					return SNew(SDockTab).TabRole(ETabRole::NomadTab)
																						[
																							ConstructDeletionTab()
																						];
																				}));

	FGlobalTabmanager::Get()->RegisterTabSpawner("Folder Deletion",
										   FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args) -> TSharedRef<SDockTab>
																	 {
																		  return SNew(SDockTab).TabRole(ETabRole::NomadTab)
																			  [
																				  ConstructFoldersTab()
																			  ];
																	  }));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner("Asset Renaming",
													  FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args) -> TSharedRef<SDockTab>
																				{
																					return SNew(SDockTab).TabRole(ETabRole::NomadTab)
																						[
																							ConstructRenamingTab()
																						];
																				}));
	ChildSlot
	[
		FGlobalTabmanager::Get()->RestoreFrom(Layout, TSharedPtr<SWindow>()).ToSharedRef()
	];

	OnComboSelectionChanged(MakeShareable(new FString(T_List_All)), ESelectInfo::OnMouseClick);
}

TSharedRef<SWidget> SUnrealCleanupWidget::ConstructDeletionTab()
{
	TSharedPtr<SWidget> RetVal =
		SNew(SBorder)
		[
			SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(STextBlock)
						.Text(FText::FromString("Delete unused assets"))
						.Font(TitleFont)
						.Justification(ETextJustify::Center)
						.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(10.f))
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(0.1f)
						.Padding(5.f)
						[
							ConstructComboBox()
						]
						+ SHorizontalBox::Slot()
						.FillWidth(0.5f)
						.Padding(5.f)
						[
							ConstructInfoTextBlock()
						]
						+ SHorizontalBox::Slot()
						.FillWidth(0.4f)
						.Padding(5.f)
						[
							ConstructPathTextBlock()
						]
				]

				// Buttons
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("Delete All Selected")))
								.OnClicked(this, &SUnrealCleanupWidget::OnDeleteAllClicked)
						]

						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("Select All")))
								.OnClicked(this, &SUnrealCleanupWidget::OnSelectAllClicked)
						]

						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("Deselect All")))
								.OnClicked(this, &SUnrealCleanupWidget::OnDeselectAllClicked)
						]
				]

				// Asset List
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Fill)
				[
					SNew(SScrollBox)

						+ SScrollBox::Slot()
						[
							ConstructAssetListView()
						]
				]
		];

	return RetVal.ToSharedRef();
}

TSharedRef<SWidget> SUnrealCleanupWidget::ConstructFoldersTab()
{
	TSharedPtr<SWidget> Retval =
		SNew(SBorder)
		[
			SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(STextBlock)
						.Text(FText::FromString("Delete empty folders - Coming soon!"))
						.Font(TitleFont)
						.Justification(ETextJustify::Center)
						.ColorAndOpacity(FLinearColor::White)
				]
		];
	return Retval.ToSharedRef();
}

TSharedRef<SWidget> SUnrealCleanupWidget::ConstructRenamingTab()
{
	TSharedPtr<SWidget> Retval =
		SNew(SBorder)
		[
			SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(STextBlock)
						.Text(FText::FromString("Rename Assets - Coming soon!"))
						.Font(TitleFont)
						.Justification(ETextJustify::Center)
						.ColorAndOpacity(FLinearColor::White)
				]
		];
	return Retval.ToSharedRef();
}

TSharedRef<SComboBox<TSharedPtr<FString>>> SUnrealCleanupWidget::ConstructComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> RetVal =
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ComboBoxSource)
		.OnGenerateWidget(this, &SUnrealCleanupWidget::OnGenerateComboWidget)
		.OnSelectionChanged(this, &SUnrealCleanupWidget::OnComboSelectionChanged)
		[
			SAssignNew(ComboDisplayText, STextBlock)
				.Text(FText::FromString(T_List_All))
		];
	return RetVal;
}

TSharedRef<STextBlock> SUnrealCleanupWidget::ConstructInfoTextBlock()
{
	FString ComboText = ComboDisplayText.Get()->GetText().ToString();
	TSharedRef<STextBlock> RetVal = SNew(STextBlock);

	if (ComboText == T_List_All)
	{
		RetVal = SNew(STextBlock)
			.Text(FText::FromString(TEXT("List all available assets")))
			.Font(TitleFont)
			.ColorAndOpacity(FLinearColor::White);
	}
	else if (ComboText == T_List_Unused)
	{
		RetVal = SNew(STextBlock)
			.Text(FText::FromString(TEXT("List all unused assets")))
			.Font(TitleFont)
			.ColorAndOpacity(FLinearColor::White);
	}

	InfoTextBlock = RetVal.ToSharedPtr();

	return RetVal;
}

TSharedRef<STextBlock> SUnrealCleanupWidget::ConstructPathTextBlock()
{
	FString DisplayedPath = SelectedFolderPath;
	DisplayedPath.RemoveFromStart(TEXT("/Game"));
	DisplayedPath = TEXT("Content") + DisplayedPath;
	return
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Selected Folder: \n") + DisplayedPath))
		.Font(FCoreStyle::Get().GetFontStyle("EmbossedText"))
		.ColorAndOpacity(FLinearColor::White);
}

TSharedRef<SWidget> SUnrealCleanupWidget::OnGenerateComboWidget(TSharedPtr<FString> SourceItem)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));
}

void SUnrealCleanupWidget::OnComboSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid() == false)
	{
		return;
	}
	ComboDisplayText->SetText(FText::FromString(*SelectedItem.Get()));

	FUnrealCleanupModule& CleanupModule = FModuleManager::LoadModuleChecked<FUnrealCleanupModule>("UnrealCleanup");
	if (*SelectedItem.Get() == T_List_All)
	{
		CleanupModule.GetAllAssetsData(StoredAssetsDataArray, DisplayedAssetsDataArray);
	}
	else if (*SelectedItem.Get() == T_List_Unused)
	{
		CleanupModule.GetUnusedAssetsData(StoredAssetsDataArray, DisplayedAssetsDataArray);
	}
	SelectedAssetsData.Empty();
	CheckBoxArray.Empty();
	AssetListView->RebuildList();
	InfoTextBlock->SetText(FText::FromString(*SelectedItem.Get() + " assets"));
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SUnrealCleanupWidget::ConstructAssetListView()
{
	AssetListView =
		SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(24.f)
		.ListItemsSource(&DisplayedAssetsDataArray)
		.OnGenerateRow(this, &SUnrealCleanupWidget::OnGenerateRowForList)
		.OnMouseButtonClick(this, &SUnrealCleanupWidget::OnRowWidgetClicked);

	return AssetListView.ToSharedRef();
}

TSharedRef<ITableRow> SUnrealCleanupWidget::OnGenerateRowForList(TSharedPtr<FAssetData> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (Item.IsValid() == false)
	{
		return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	}

	const FString DisplayAssetClassName = Item->GetClass()->GetName();
	const FString DisplayAssetName = Item->AssetName.ToString();
	FString DisplayAssetPath = Item->ObjectPath.ToString();

	// Make assetpath more readable for user
	DisplayAssetPath.RemoveFromStart(TEXT("/Game"));
	DisplayAssetPath = TEXT("Content") + DisplayAssetPath;
	int32 DotIndex = 0;
	DisplayAssetPath.FindLastChar('.', DotIndex);
    if (DotIndex != INDEX_NONE)
    {
		DisplayAssetPath = DisplayAssetPath.Left(DotIndex);
    }


	FSlateFontInfo AssetClassFont = FCoreStyle::Get().GetFontStyle("EmbossedText");
	AssetClassFont.Size = 8;
	FSlateFontInfo AssetNameFont = FCoreStyle::Get().GetFontStyle("EmbossedText");
	AssetNameFont.Size = 10;

	return
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		.Padding(FMargin(5.f))
		[
			SNew(SHorizontalBox)

				//Checkbox
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(10.f)
				[
					ConstructCheckBox(Item)
				]
				// Display Asset Class Name
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(0.2f)
				[
					ConstructTextForRowWidget(DisplayAssetClassName, AssetClassFont)
				]
				// Display Asset Name
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(0.4f)
				[
					ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
				]
				// Display Asset Path
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(0.4f)
				[
					ConstructTextForRowWidget(DisplayAssetPath, AssetClassFont)
				]
				// Button to delete asset
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					ConstructButtonForRowWidget(Item)
				]
		];
}

void SUnrealCleanupWidget::OnRowWidgetClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	if (ClickedAssetData.IsValid() == false)
	{
		return;
	}

	FString AssetPath = ClickedAssetData->ObjectPath.ToString();
	FString AssetName = ClickedAssetData->AssetName.ToString();
	FString AssetClass = ClickedAssetData->GetClass()->GetName();

	UE_LOG(LogTemp, Warning, TEXT("Clicked Asset: %s, %s, %s"), *AssetPath, *AssetName, *AssetClass);
}

TSharedRef<SCheckBox> SUnrealCleanupWidget::ConstructCheckBox(const TSharedPtr<FAssetData> AssetDataToDisplay)
{
	TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SUnrealCleanupWidget::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);
	CheckBoxArray.Add(CheckBox);
	return CheckBox;
}

void SUnrealCleanupWidget::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
		case ECheckBoxState::Unchecked:
		if (SelectedAssetsData.Contains(AssetData))
		{
			SelectedAssetsData.Remove(AssetData);
		}
		break;
		case ECheckBoxState::Checked:
		if (!SelectedAssetsData.Contains(AssetData))
		{
			SelectedAssetsData.Add(AssetData);
		}
		break;
		case ECheckBoxState::Undetermined:
		break;
		default:
		break;
	}
}

TSharedRef<STextBlock> SUnrealCleanupWidget::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontToUse)
		.ColorAndOpacity(FLinearColor::White);
}

TSharedRef<SButton> SUnrealCleanupWidget::ConstructButtonForRowWidget(const TSharedPtr<FAssetData> AssetDataToDisplay)
{
	return
		SNew(SButton)
		.Text(FText::FromString(TEXT("Delete")))
		.OnClicked(this, &SUnrealCleanupWidget::OnDeleteAssetClicked, AssetDataToDisplay);
}

FReply SUnrealCleanupWidget::OnDeleteAssetClicked(TSharedPtr<FAssetData> AssetData)
{
	const bool Success = FModuleManager::LoadModuleChecked<FUnrealCleanupModule>("UnrealCleanup").DeleteSingleAsset(*AssetData.Get());
	if (Success)
	{
		if (StoredAssetsDataArray.Contains(AssetData))
		{
			StoredAssetsDataArray.Remove(AssetData);
		}
		if (DisplayedAssetsDataArray.Contains(AssetData))
		{
			DisplayedAssetsDataArray.Remove(AssetData);
		}
		SelectedAssetsData.Empty();
		CheckBoxArray.Empty();
		AssetListView->RebuildList();
	}
	return FReply::Handled();
}

FReply SUnrealCleanupWidget::OnDeleteAllClicked()
{
	if (SelectedAssetsData.Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("No asset selected"), FText::FromString("Warning"));
		return FReply::Handled();
	}

	TArray<FAssetData> AssetsToDelete;
	for (TSharedPtr<FAssetData> AssetData : SelectedAssetsData)
	{
		AssetsToDelete.Add(*AssetData.Get());
	}

	const bool Success = FModuleManager::LoadModuleChecked<FUnrealCleanupModule>("UnrealCleanup").DeleteMultipleAssets(AssetsToDelete);
	if (Success)
	{
		for (const TSharedPtr<FAssetData>& AssetData : SelectedAssetsData)
		{
			if (StoredAssetsDataArray.Contains(AssetData))
			{
				StoredAssetsDataArray.Remove(AssetData);
			}
			if (DisplayedAssetsDataArray.Contains(AssetData))
			{
				DisplayedAssetsDataArray.Remove(AssetData);
			}
		}
		SelectedAssetsData.Empty();
		CheckBoxArray.Empty();
		AssetListView->RebuildList();
	}
	return FReply::Handled();
}

FReply SUnrealCleanupWidget::OnSelectAllClicked()
{
	SelectedAssetsData.Empty();
	if (CheckBoxArray.Num() == 0)
	{
		return FReply::Handled();
	}

	for (const TSharedRef<SCheckBox> Checkbox : CheckBoxArray)
	{
		Checkbox->SetIsChecked(ECheckBoxState::Unchecked);
		Checkbox->ToggleCheckedState();
	}
	return FReply::Handled();
}

FReply SUnrealCleanupWidget::OnDeselectAllClicked()
{
	SelectedAssetsData.Empty();
	if (CheckBoxArray.Num() == 0)
	{
		return FReply::Handled();
	}

	for (const TSharedRef<SCheckBox> Checkbox : CheckBoxArray)
	{
		Checkbox->SetIsChecked(ECheckBoxState::Checked);
		Checkbox->ToggleCheckedState();
	}
	return FReply::Handled();
}
