// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUnrealCleanupModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void GetAllAssetsData(const TArray<TSharedPtr<FAssetData>> AssetsDataToFilter, TArray<TSharedPtr<FAssetData>>& OutAssetsData);
	void GetUnusedAssetsData(const TArray<TSharedPtr<FAssetData>> AssetsDataToFilter, TArray<TSharedPtr<FAssetData>>& OutAssetsData);
	bool DeleteSingleAsset(const FAssetData& AssetData);
	bool DeleteMultipleAssets(const TArray<FAssetData>& AssetsToDelete);


private:
	void RegisterCustomMenuExtension();
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);
	void OnProjectCleanupClicked();
	void RegisterProjectCleanupTab();
	TSharedRef<SDockTab> OnSpawnProjectCleanupTab(const FSpawnTabArgs& TabSpawnArgs);
	void FixUpRedirectors();
	TArray<TSharedPtr<FAssetData>> GetAssetsDataArray();


	TArray<FString> SelectedFolderPath;
	TArray<TSharedPtr<FAssetData>> StoredAssetsDataArray;

};
