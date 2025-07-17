// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "FPAssetManager.generated.h"

struct FGameplayTag;
class UFPAssetData;

// 비동기 애셋 로드가 완료되었을 때 호출되는 델레게이트
DECLARE_DELEGATE_OneParam(FAsyncLoadCompletedDelegate, UObject*/*LoadedAsset*/);

/**
 * UFPAssetData에 설정된 애셋을 동기, 비동기 방식으로 로드하는 기능을 제공하는 Custom Asset Manager class이다.
 * Preload를 수행하도록 설정된 애셋을 게임 초기화 시 로드한다.
 */
UCLASS()
class FLASHPOINT_API UFPAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// UFPAssetManager의 싱글톤 인스턴스를 반환한다.
	static UFPAssetManager& Get();

	static void Initialize();

	// AssetPath에 해당하는 애셋을 동기적으로 로드하고 반환한다.
	// bKeepInMemory가 true면, 로드할 때 LoadedAssets에 저장해 GC에 제거되지 않도록 메모리에 유지한다.
	static UObject* LoadSyncByPath(const FSoftObjectPath& AssetPath, bool bKeepInMemory = true);
	
	template <typename AssetType>
	static AssetType* GetAssetByPath(const FSoftObjectPath& AssetPath, bool bKeepInMemory = true)
	{
		return Cast<AssetType>(LoadSyncByPath(AssetPath, bKeepInMemory));
	}

	template <typename ClassType>
	static TSubclassOf<ClassType> GetSubclassByPath(const FSoftObjectPath& AssetPath, bool bKeepInMemory = true)
	{
		TSubclassOf<ClassType> LoadedSubclass = Cast<UClass>(LoadSyncByPath(AssetPath, bKeepInMemory));
		return LoadedSubclass;
	}

	// AssetId에 해당하는 애셋을 동기적으로 로드하고 반환한다.
	// bKeepInMemory가 true면, 로드할 때 LoadedAssets에 저장해 GC에 제거되지 않도록 메모리에 유지한다.
	static UObject* LoadSyncById(const FName& AssetId, bool bKeepInMemory = true);
	
	template <typename AssetType>
	static AssetType* GetAssetById(const FName& AssetId, bool bKeepInMemory = true)
	{
		return Cast<AssetType>(LoadSyncById(AssetId, bKeepInMemory));
	}

	template <typename ClassType>
	static TSubclassOf<ClassType> GetSubclassById(const FName& AssetId, bool bKeepInMemory = true)
	{
		TSubclassOf<ClassType> LoadedSubclass = Cast<UClass>(LoadSyncById(AssetId, bKeepInMemory));
		return LoadedSubclass;
	}

	// AssetTag에 해당하는 애셋을 동기적으로 로드하고 반환한다.
	// bKeepInMemory가 true면, 로드할 때 LoadedAssets에 저장해 GC에 제거되지 않도록 메모리에 유지한다.
	static UObject* LoadSyncByTag(const FGameplayTag& AssetTag, bool bKeepInMemory = true);
	
	template <typename AssetType>
	static AssetType* GetAssetByTag(const FGameplayTag& AssetTag, bool bKeepInMemory = true)
	{
		return Cast<AssetType>(LoadSyncByTag(AssetTag, bKeepInMemory));
	}

	template <typename ClassType>
	static TSubclassOf<ClassType> GetSubclassByTag(const FGameplayTag& AssetTag, bool bKeepInMemory = true)
	{
		TSubclassOf<ClassType> LoadedSubclass = Cast<UClass>(LoadSyncByTag(AssetTag, bKeepInMemory));
		return LoadedSubclass;
	}

	// AssetPath에 해당하는 애셋을 비동기적으로 로드한다.
	// 로드가 끝나면 AsyncLoadCompletedDelegate가 실행되고, 애셋을 전달한다.
	// bKeepInMemory가 true면, 로드할 때 LoadedAssets에 저장해 GC에 제거되지 않도록 메모리에 유지한다.
	static void LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate AsyncLoadCompletedDelegate = FAsyncLoadCompletedDelegate(), bool bKeepInMemory = true);

	// AssetId에 해당하는 애셋을 비동기적으로 로드한다.
	// 로드가 끝나면 AsyncLoadCompletedDelegate가 실행되고, 애셋을 전달한다.
	// bKeepInMemory가 true면, 로드할 때 LoadedAssets에 저장해 GC에 제거되지 않도록 메모리에 유지한다.
	static void LoadAsyncById(const FName& AssetId, FAsyncLoadCompletedDelegate AsyncLoadCompletedDelegate = FAsyncLoadCompletedDelegate(), bool bKeepInMemory = true);

	// AssetTag에 해당하는 애셋을 비동기적으로 로드한다.
	// 로드가 끝나면 AsyncLoadCompletedDelegate가 실행되고, 애셋을 전달한다.
	// bKeepInMemory가 true면, 로드할 때 LoadedAssets에 저장해 GC에 제거되지 않도록 메모리에 유지한다.
	static void LoadAsyncByTag(const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate AsyncLoadCompletedDelegate = FAsyncLoadCompletedDelegate(), bool bKeepInMemory = true);

	// 로드된 모든 애셋을 메모리에서 해제한다.
	// LoadedAssets에서 제거해 GC에 의해 제거되도록 한다.
	static void ReleaseAll();

private:
	// UFPAssetData에서 Preload를 수행하도록 설정된 모든 애셋을 로드한다.
	void LoadPreloadAssets();

	// 로드된 애셋을 메모리에 유지하기 위해 LoadedAssets에 추가한다.
	void AddLoadedAsset(const UObject* LoadedAsset);

	UPROPERTY()
	TObjectPtr<UFPAssetData> LoadedAssetData;

	// 로드된 애셋을 메모리에 유지하기 위한 Set
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// LoadedAssets에 대해 Thread Safe한 접근을 보장하기 위한 Critical Section
	FCriticalSection LoadedAssetsCritical;
};
