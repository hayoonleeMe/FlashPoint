// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAssetData.h"

#include "UObject/ObjectSaveContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAssetData)

void UFPAssetData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	
	AssetIdToPath.Empty();

	// AssetIdToPath를 채운다.
	for (const auto& Pair : AssetIdToEntry)
	{
		FSoftObjectPath AssetPath = Pair.Value.AssetPath;
		ResolveAssetPath(AssetPath);
		AssetIdToPath.Emplace(Pair.Key, AssetPath);
	}

	// AssetTagToPath를 채운다.
	for (const auto& Pair : AssetTagToEntry)
	{
		FSoftObjectPath AssetPath = Pair.Value.AssetPath;
		ResolveAssetPath(AssetPath);
		AssetTagToPath.Emplace(Pair.Key, AssetPath);
	}
}

#if WITH_EDITOR
EDataValidationResult UFPAssetData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (const auto& Pair : AssetIdToEntry)
	{
		const FName& AssetId = Pair.Key;
		const FSoftObjectPath& AssetPath = Pair.Value.AssetPath;

		if (AssetId.IsNone())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Asset Id is None"))));
			Result = EDataValidationResult::Invalid;
		}
			
		if (!AssetPath.IsValid())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Asset Path is Invalid : [Asset Id %s]"), *AssetId.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}

	for (const auto& Pair : AssetTagToEntry)
	{
		const FGameplayTag& AssetTag = Pair.Key;
		const FSoftObjectPath& AssetPath = Pair.Value.AssetPath;

		if (!AssetTag.IsValid())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Asset Tag is None"))));
			Result = EDataValidationResult::Invalid;
		}
			
		if (!AssetPath.IsValid())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Asset Path is Invalid : [Asset Tag %s]"), *AssetTag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

FSoftObjectPath UFPAssetData::GetAssetPathById(const FName& AssetId) const
{
	const FSoftObjectPath* PathPtr = AssetIdToPath.Find(AssetId);
	if (ensureAlwaysMsgf(PathPtr, TEXT("Can't find AssetPath by AssetName %s"), *AssetId.ToString()))
	{
		return *PathPtr;
	}
	return FSoftObjectPath();
}

FSoftObjectPath UFPAssetData::GetAssetPathByTag(const FGameplayTag& AssetTag) const
{
	const FSoftObjectPath* PathPtr = AssetTagToPath.Find(AssetTag);
	if (ensureAlwaysMsgf(PathPtr, TEXT("Can't find AssetPath by AssetTag %s"), *AssetTag.ToString()))
	{
		return *PathPtr;
	}
	return FSoftObjectPath();
}

TArray<FSoftObjectPath> UFPAssetData::GetPreloadAssetPaths() const
{
	TArray<FSoftObjectPath> Paths;
	
	for (const auto& Pair : AssetIdToEntry)
	{
		if (Pair.Value.bPreLoad)
		{
			Paths.Emplace(Pair.Value.AssetPath);
		}
	}
	
	for (const auto& Pair : AssetTagToEntry)
	{
		if (Pair.Value.bPreLoad)
		{
			Paths.Emplace(Pair.Value.AssetPath);
		}
	}
	
	return Paths;
}

void UFPAssetData::ResolveAssetPath(FSoftObjectPath& AssetPath)
{
	const FString& AssetName = AssetPath.GetAssetName();
		
	if (AssetName.StartsWith(TEXT("BP_")) || AssetName.StartsWith(TEXT("WBP_")) || AssetName.StartsWith(TEXT("ABP_")) || AssetName.StartsWith(TEXT("GA_")) || AssetName.StartsWith(TEXT("GE_")))
	{
		FString AssetPathString = AssetPath.GetAssetPathString();
		AssetPathString.Append(TEXT("_C"));
		AssetPath = FSoftObjectPath(AssetPathString);
	}
}
