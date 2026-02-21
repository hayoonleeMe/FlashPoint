// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FPAssetData.generated.h"

USTRUCT()
struct FDoubleAssetTag
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AssetTag;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag SubTag;
	
	bool operator==(const FDoubleAssetTag& Other) const
	{
		// SubTag는 비워두는 경우도 지원
		if (!SubTag.IsValid() && !Other.SubTag.IsValid())
		{
			return AssetTag.MatchesTagExact(Other.AssetTag);
		}
		return AssetTag.MatchesTagExact(Other.AssetTag) && SubTag.MatchesTagExact(Other.SubTag);
	}
	
	friend uint32 GetTypeHash(const FDoubleAssetTag& Key)
	{
		return HashCombine(GetTypeHash(Key.AssetTag), GetTypeHash(Key.SubTag));
	}
};

USTRUCT()
struct FAssetEntry
{
	GENERATED_BODY()
	
	FAssetEntry()
		: bPreLoad(false)
	{ }

	UPROPERTY(EditDefaultsOnly)
	FSoftObjectPath AssetPath;

	// true면, 이 애셋은 UFPAssetManager에 의해 게임 시작 시 로드된다.
	UPROPERTY(EditDefaultsOnly)
	uint8 bPreLoad : 1;
};

/**
 * FPAssetManager로 로드할 애셋의 정보를 저장하는 Primary Data Asset
 */
UCLASS(Const)
class FLASHPOINT_API UFPAssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 애셋을 저장하기 전에 호출되어 AssetIdToPath를 채운다.
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

	// AssetId에 대한 AssetPath를 반환한다.
	FSoftObjectPath GetAssetPathById(const FName& AssetId) const;

	// AssetTag와 SecondTag에 대한 AssetPath를 반환한다.
	FSoftObjectPath GetAssetPathByTag(const FGameplayTag& AssetTag, const FGameplayTag& SubTag) const;

	// 모든 Preload할 애셋의 AssetPath TArray를 반환한다.
	TArray<FSoftObjectPath> GetPreloadAssetPaths() const;

private:
	// AssetPath가 가리키는 애셋이 BP_, WBP_, ABP_, GA_, GE_로 시작한다면, _C를 붙여 클래스 경로로 저장한다.
	static void ResolveAssetPath(FSoftObjectPath& AssetPath);
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FAssetEntry> AssetIdToEntry;

	UPROPERTY()
	TMap<FName, FSoftObjectPath> AssetIdToPath;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FDoubleAssetTag, FAssetEntry> AssetTagToEntry;
	
	UPROPERTY()
	TMap<FDoubleAssetTag, FSoftObjectPath> AssetTagToPath;
};
