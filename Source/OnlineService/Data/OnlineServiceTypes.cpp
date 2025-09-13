
#include "OnlineServiceTypes.h"

#include "OnlineService.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OnlineServiceTypes)

void FHTTPMetaData::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("MetaData:"));
	UE_LOG(LogOnlineService, Log, TEXT("httpStatusCode: %d"), httpStatusCode);
	UE_LOG(LogOnlineService, Log, TEXT("requestId: %s"), *requestId);
	UE_LOG(LogOnlineService, Log, TEXT("attempts: %d"), attempts);
	UE_LOG(LogOnlineService, Log, TEXT("totalRetryDelay: %f"), totalRetryDelay);
}

void FCodeDeliveryDetails::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("CodeDeliveryDetails:"));
	UE_LOG(LogOnlineService, Log, TEXT("   AttributeName: %s"), *AttributeName);
	UE_LOG(LogOnlineService, Log, TEXT("   DeliveryMedium: %s"), *DeliveryMedium);
	UE_LOG(LogOnlineService, Log, TEXT("   Destination: %s"), *Destination);
}

void FSignUpResponse::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("FDSSignUpResponse Details:"));
	CodeDeliveryDetails.Dump();
	UE_LOG(LogOnlineService, Log, TEXT("UserConfirmed: %s"), UserConfirmed ? TEXT("true") : TEXT("false"));
	UE_LOG(LogOnlineService, Log, TEXT("UserSub: %s"), *UserSub);
}

void FGameSessionInfo::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("GameSession:"));
	UE_LOG(LogOnlineService, Log, TEXT("  CreationTime: %s"), *CreationTime);
	UE_LOG(LogOnlineService, Log, TEXT("  CreatorId: %s"), *CreatorId);
	UE_LOG(LogOnlineService, Log, TEXT("  CurrentPlayerSessionCount: %d"), CurrentPlayerSessionCount);
	UE_LOG(LogOnlineService, Log, TEXT("  DnsName: %s"), *DnsName);
	UE_LOG(LogOnlineService, Log, TEXT("  FleetArn: %s"), *FleetArn);
	UE_LOG(LogOnlineService, Log, TEXT("  FleetId: %s"), *FleetId);
    
	UE_LOG(LogOnlineService, Log, TEXT("  GameProperties:"));
	for (const auto& GameProperty : GameProperties)
	{
		UE_LOG(LogOnlineService, Log, TEXT("    %s: %s"), *GameProperty.Key, *GameProperty.Value);
	}
    
	UE_LOG(LogOnlineService, Log, TEXT("  GameSessionData: %s"), *GameSessionData);
	UE_LOG(LogOnlineService, Log, TEXT("  GameSessionId: %s"), *GameSessionId);
	UE_LOG(LogOnlineService, Log, TEXT("  IpAddress: %s"), *IpAddress);
	UE_LOG(LogOnlineService, Log, TEXT("  Location: %s"), *Location);
	UE_LOG(LogOnlineService, Log, TEXT("  MatchmakerData: %s"), *MatchmakerData);
	UE_LOG(LogOnlineService, Log, TEXT("  MaximumPlayerSessionCount: %d"), MaximumPlayerSessionCount);
	UE_LOG(LogOnlineService, Log, TEXT("  Name: %s"), *Name);
	UE_LOG(LogOnlineService, Log, TEXT("  PlayerSessionCreationPolicy: %s"), *PlayerSessionCreationPolicy);
	UE_LOG(LogOnlineService, Log, TEXT("  Port: %d"), Port);
	UE_LOG(LogOnlineService, Log, TEXT("  Status: %s"), *Status);
	UE_LOG(LogOnlineService, Log, TEXT("  StatusReason: %s"), *StatusReason);
	UE_LOG(LogOnlineService, Log, TEXT("  TerminationTime: %s"), *TerminationTime);
}

void FDescribeGameSessionsResponse::Dump() const
{
	for (const FGameSessionInfo& GameSession : GameSessions)
	{
		GameSession.Dump();
	}
}

void FPlayerSessionInfo::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("PlayerSession:"));
	UE_LOG(LogOnlineService, Log, TEXT("  CreationTime: %s"), *CreationTime);
	UE_LOG(LogOnlineService, Log, TEXT("  DnsName: %s"), *DnsName);
	UE_LOG(LogOnlineService, Log, TEXT("  FleetArn: %s"), *FleetArn);
	UE_LOG(LogOnlineService, Log, TEXT("  FleetId: %s"), *FleetId);
	UE_LOG(LogOnlineService, Log, TEXT("  GameSessionId: %s"), *GameSessionId);
	UE_LOG(LogOnlineService, Log, TEXT("  IpAddress: %s"), *IpAddress);
	UE_LOG(LogOnlineService, Log, TEXT("  PlayerData: %s"), *PlayerData);
	UE_LOG(LogOnlineService, Log, TEXT("  PlayerId: %s"), *PlayerId);
	UE_LOG(LogOnlineService, Log, TEXT("  PlayerSessionId: %s"), *PlayerSessionId);
	UE_LOG(LogOnlineService, Log, TEXT("  Port: %d"), Port);
	UE_LOG(LogOnlineService, Log, TEXT("  Status: %s"), *Status);
	UE_LOG(LogOnlineService, Log, TEXT("  TerminationTime: %s"), *TerminationTime);
}

void FNewDeviceMetaData::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("NewDeviceMetadata:"));
	UE_LOG(LogOnlineService, Log, TEXT("   DeviceGroupKey: %s"), *DeviceGroupKey);
	UE_LOG(LogOnlineService, Log, TEXT("   DeviceKey: %s"), *DeviceKey);
}

void FAuthenticationResult::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("AuthenticationResult:"));
	UE_LOG(LogOnlineService, Log, TEXT("   AccessToken: %s"), *AccessToken);
	UE_LOG(LogOnlineService, Log, TEXT("   ExpiresIn: %d"), ExpiresIn);
	UE_LOG(LogOnlineService, Log, TEXT("   IdToken: %s"), *IdToken);
	NewDeviceMetadata.Dump();
	UE_LOG(LogOnlineService, Log, TEXT("   RefreshToken: %s"), *RefreshToken);
	UE_LOG(LogOnlineService, Log, TEXT("   TokenType: %s"), *TokenType);
}

void FChallengeParameters::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("ChallengeParameters:"));
	for (const auto& Param : Parameters)
	{
		UE_LOG(LogOnlineService, Log, TEXT("   %s: %s"), *Param.Key, *Param.Value);
	}
}

void FInitiateAuthResponse::Dump() const
{
	UE_LOG(LogOnlineService, Log, TEXT("FDSSignUpResponse Details:"));
	AuthenticationResult.Dump();
	UE_LOG(LogOnlineService, Log, TEXT("ChallengeName: %s"), *ChallengeName);
	ChallengeParameters.Dump();
	UE_LOG(LogOnlineService, Log, TEXT("Session: %s"), *Session);
	UE_LOG(LogOnlineService, Log, TEXT("Email: %s"), *Email);
}
