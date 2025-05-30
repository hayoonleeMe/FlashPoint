// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAssetManager.h"

#include "FPLogChannels.h"
#include "Data/FPAssetData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAssetManager)

UFPAssetManager& UFPAssetManager::Get()
{
	if (UFPAssetManager* AssetManager = Cast<UFPAssetManager>(GEngine->AssetManager))
	{
		return *AssetManager;
	}

	UE_LOG(LogFP, Fatal, TEXT("Can't find UFPAssetManager. AssetManagerClassName in DefaultEngine.ini must be set to FPAssetManager."));

	// Fatal Log로 인해 실제로 수행되지 않음
	return *NewObject<UFPAssetManager>();
}

void UFPAssetManager::Initialize()
{
	Get().LoadPreloadAssets();
}

UObject* UFPAssetManager::LoadSyncByPath(const FSoftObjectPath& AssetPath, bool bKeepInMemory)
{
	if (AssetPath.IsValid())
	{
		UObject* LoadedAsset = AssetPath.ResolveObject();
		if (!LoadedAsset)
		{
			if (IsInitialized())
			{
				LoadedAsset = GetStreamableManager().LoadSynchronous(AssetPath);
			}
			else
			{
				LoadedAsset = AssetPath.TryLoad();
			}
		}

		if (LoadedAsset)
		{
			if (bKeepInMemory)
			{
				Get().AddLoadedAsset(LoadedAsset);
			}
		}
		else
		{
			UE_LOG(LogFP, Fatal, TEXT("Failed to load asset %s synchronously."), *AssetPath.ToString());
		}

		return LoadedAsset;
	}

	return nullptr;
}

UObject* UFPAssetManager::LoadSyncById(const FName& AssetId, bool bKeepInMemory)
{
	const UFPAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);
	
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathById(AssetId);
	return LoadSyncByPath(AssetPath, bKeepInMemory);
}

void UFPAssetManager::LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate AsyncLoadCompletedDelegate, bool bKeepInMemory)
{
	if (!IsInitialized())
	{
		AsyncLoadCompletedDelegate.ExecuteIfBound(nullptr);
		UE_LOG(LogFP, Error, TEXT("AssetManager must be initialized for async loading."));
		return;
	}

	if (!AssetPath.IsValid())
	{
		AsyncLoadCompletedDelegate.ExecuteIfBound(nullptr);
		UE_LOG(LogFP, Error, TEXT("Invalid Asset Path %s"), *AssetPath.ToString());
		return;
	}

	const TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(AssetPath);
	Handle->BindCompleteDelegate(FStreamableDelegate::CreateLambda([AssetPath, bKeepInMemory, CompletedDelegate = MoveTemp(AsyncLoadCompletedDelegate)]()
	{
		if (UObject* LoadedAsset = AssetPath.ResolveObject())
		{
			if (bKeepInMemory)
			{
				Get().AddLoadedAsset(LoadedAsset);
			}
					
			CompletedDelegate.ExecuteIfBound(LoadedAsset);
		}
	}));
}

void UFPAssetManager::LoadAsyncById(const FName& AssetId, FAsyncLoadCompletedDelegate AsyncLoadCompletedDelegate, bool bKeepInMemory)
{
	const UFPAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);
	
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathById(AssetId);
	LoadAsyncByPath(AssetPath, MoveTemp(AsyncLoadCompletedDelegate), bKeepInMemory);
}

void UFPAssetManager::ReleaseAll()
{
	UFPAssetManager& AssetManager = Get();
	FScopeLock ScopeLock(&AssetManager.LoadedAssetsCritical);
	AssetManager.LoadedAssets.Reset();
}

void UFPAssetManager::LoadPreloadAssets()
{
	if (LoadedAssetData)
	{
		return;
	}

	UFPAssetData* AssetData = nullptr;
	
	// DefaultGame.ini - AssetManagerSettings - PrimaryAssetTypesToScan에 설정된 Type의 애셋을 로드한다.
	const FPrimaryAssetType PrimaryAssetType(UFPAssetData::StaticClass()->GetFName());
	const TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
	
	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete(0.f, false);
		AssetData = Cast<UFPAssetData>(Handle->GetLoadedAsset());
	}
	
	if (AssetData)
	{
		LoadedAssetData = AssetData;

		// Preload를 수행해야 할 내부 애셋을 모두 로드한다.
		TArray<FSoftObjectPath> PreloadAssetPaths = LoadedAssetData->GetPreloadAssetPaths();
		for (const FSoftObjectPath& Path : PreloadAssetPaths)
		{
			LoadSyncByPath(Path);
		}
	}
	else
	{
		UE_LOG(LogFP, Fatal, TEXT("Failed to load PrimaryAssetType %s"), *PrimaryAssetType.ToString());
	}
}

void UFPAssetManager::AddLoadedAsset(const UObject* LoadedAsset)
{
	if (LoadedAsset)
	{
		FScopeLock ScopeLock(&LoadedAssetsCritical);
		LoadedAssets.Add(LoadedAsset);
	}
}
