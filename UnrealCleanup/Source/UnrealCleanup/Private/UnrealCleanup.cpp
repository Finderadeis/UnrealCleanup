// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealCleanup.h"
#include "ContentBrowserModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetViewUtils.h"
#include "AssetToolsModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "UnrealCleanupWidget.h"



#define LOCTEXT_NAMESPACE "FUnrealCleanupModule"

void FUnrealCleanupModule::StartupModule()
{
	RegisterCustomMenuExtension();
	RegisterProjectCleanupTab();
}

void FUnrealCleanupModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("ProjectCleanup"));
}

void FUnrealCleanupModule::GetAllAssetsData(const TArray<TSharedPtr<FAssetData>> AssetsDataToFilter, TArray<TSharedPtr<FAssetData>>& OutAssetsData)
{
	OutAssetsData.Empty();
	for (TSharedPtr<FAssetData> DataPtr : AssetsDataToFilter)
	{
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(DataPtr->ObjectPath.ToString());
		OutAssetsData.Add(DataPtr);
	}
}

void FUnrealCleanupModule::GetUnusedAssetsData(const TArray<TSharedPtr<FAssetData>> AssetsDataToFilter, TArray<TSharedPtr<FAssetData>>& OutAssetsData)
{
	FixUpRedirectors();
	OutAssetsData.Empty();
	for (TSharedPtr<FAssetData> DataPtr : AssetsDataToFilter)
	{
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(DataPtr->ObjectPath.ToString());

		if (AssetReferencers.Num() == 0)
		{
			OutAssetsData.Add(DataPtr);
		}
	}
}

bool FUnrealCleanupModule::DeleteSingleAsset(const FAssetData& AssetData)
{
	if (ObjectTools::DeleteAssets({ AssetData }, true) > 0) return true;
	return false;
}

bool FUnrealCleanupModule::DeleteMultipleAssets(const TArray<FAssetData>& AssetsToDelete)
{
	if (ObjectTools::DeleteAssets(AssetsToDelete, true) > 0) return true;
	return false;
}

void FUnrealCleanupModule::RegisterCustomMenuExtension()
{
	FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBModuleMenuExtenders = CBModule.GetAllPathViewContextMenuExtenders();
	CBModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FUnrealCleanupModule::CustomCBMenuExtender));
}

TSharedRef<FExtender> FUnrealCleanupModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(FName("Delete"),
									   EExtensionHook::After,
									   TSharedPtr<FUICommandList>(),
									   FMenuExtensionDelegate::CreateRaw(this, &FUnrealCleanupModule::AddCBMenuEntry));

		SelectedFolderPath = SelectedPaths;
	}

	return MenuExtender;
}

void FUnrealCleanupModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuSeparator();

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Project Cleanup")),
		FText::FromString(TEXT("Open Dialog for full project cleanup process")),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FUnrealCleanupModule::OnProjectCleanupClicked))
	);

	MenuBuilder.AddMenuSeparator();
}

void FUnrealCleanupModule::OnProjectCleanupClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("ProjectCleanup"));
}

void FUnrealCleanupModule::RegisterProjectCleanupTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("ProjectCleanup"), FOnSpawnTab::CreateRaw(this, &FUnrealCleanupModule::OnSpawnProjectCleanupTab))
		.SetDisplayName(FText::FromString(TEXT("Project Cleanup")))
		.SetTooltipText(FText::FromString(TEXT("Open Project Cleanup Dialog")))
		.SetIcon(FSlateIcon());
}

TSharedRef<SDockTab> FUnrealCleanupModule::OnSpawnProjectCleanupTab(const FSpawnTabArgs& TabSpawnArgs)
{
	if (SelectedFolderPath.Num() == 0)
	{
		SelectedFolderPath.Emplace("/Game");
	}
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SUnrealCleanupWidget)
				.StoredAssetsDataArray(GetAssetsDataArray())
				.PathToDisplay(SelectedFolderPath[0])
		];
}

void FUnrealCleanupModule::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFix;

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());

	TArray<FAssetData> OutRedirectors;
	AssetRegistry.GetAssets(Filter, OutRedirectors);

	if (OutRedirectors.Num() == 0) return;

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* Redirector = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFix.Add(Redirector);
		}
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFix);
}

TArray<TSharedPtr<FAssetData>> FUnrealCleanupModule::GetAssetsDataArray()
{
	TArray<TSharedPtr<FAssetData>> AvailableAssetsData;
	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(SelectedFolderPath[0]);

	for (const FString& AssetPathName : AssetsPathNames)
	{
		//Dont touch hidden stuff in root folder
		if (AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("__ExternalActors__")) ||
			AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPathName);
		AvailableAssetsData.Add(MakeShared<FAssetData>(Data));
	}

	return AvailableAssetsData;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealCleanupModule, UnrealCleanup)