
#pragma once

// ============================================================================
// Log Category
// ============================================================================

DECLARE_LOG_CATEGORY_EXTERN(LogFP, Log, All);

// ============================================================================
// Log Macro
// ============================================================================

#define LOG_NETMODEINFO(Owner) ((Owner->GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("NM_Client%d"), static_cast<int32>(GPlayInEditorID)) : ((Owner->GetNetMode() == ENetMode::NM_Standalone) ? TEXT("NM_Standalone") : TEXT("NM_DedicatedServer")))

#define LOG_LOCALROLEINFO(Owner) *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), Owner->GetLocalRole()))

#define LOG_REMOTEROLEINFO(Owner) *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), Owner->GetRemoteRole()))

#define NET_LOG(Owner, LogCategory, Verbosity, Format, ...) \
	UE_LOG(LogCategory, Verbosity, TEXT("[%s][%s/%s][%s][%hs] %s"), \
		LOG_NETMODEINFO(Owner), \
		LOG_LOCALROLEINFO(Owner), \
		LOG_REMOTEROLEINFO(Owner), \
		*Owner->GetName(), \
		__FUNCTION__, \
		*FString::Printf(Format, ##__VA_ARGS__) \
	)
