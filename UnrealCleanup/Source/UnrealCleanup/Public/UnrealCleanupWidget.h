// C 2024 Bianca Bellof @github.com/Finderadeis

#pragma once

#include "Widgets/SCompoundWidget.h"

class SUnrealCleanupWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SUnrealCleanupWidget) {}

		SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, StoredAssetsDataArray)
		SLATE_ARGUMENT(FString, PathToDisplay)

	SLATE_END_ARGS()

public:
		void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> ConstructDeletionTab();
	TSharedRef<SWidget> ConstructFoldersTab();
	TSharedRef<SWidget> ConstructRenamingTab();
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();
	TSharedRef<STextBlock> ConstructInfoTextBlock();
	TSharedRef<STextBlock> ConstructPathTextBlock();
	TSharedRef<SWidget> OnGenerateComboWidget(TSharedPtr<FString> SourceItem);
	void OnComboSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnRowWidgetClicked(TSharedPtr<FAssetData> ClickedAssetData);
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData> AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);
	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse);
	TSharedRef<SButton> ConstructButtonForRowWidget(const TSharedPtr<FAssetData> AssetDataToDisplay);
	FReply OnDeleteAssetClicked(TSharedPtr<FAssetData> AssetData);
	FReply OnDeleteAllClicked();
	FReply OnSelectAllClicked();
	FReply OnDeselectAllClicked();


	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetListView;
	TSharedPtr<STextBlock> InfoTextBlock;
	TArray< TSharedRef<SCheckBox>> CheckBoxArray;
	TArray<TSharedPtr<FAssetData>> StoredAssetsDataArray;
	TArray<TSharedPtr<FAssetData>> DisplayedAssetsDataArray;
	TArray<TSharedPtr<FAssetData>> SelectedAssetsData;
	FString SelectedFolderPath;
	TArray<TSharedPtr<FString>> ComboBoxSource;
	TSharedPtr<STextBlock> ComboDisplayText;
	FSlateFontInfo TitleFont;



};
