#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace OnlineServiceTags
{
	namespace GameSessionsAPI
	{
		extern FNativeGameplayTag CreateGameSession;
		extern FNativeGameplayTag DescribeGameSessionStatus;
		extern FNativeGameplayTag DescribeGameSessions;
		extern FNativeGameplayTag CreatePlayerSession;
		extern FNativeGameplayTag TerminateGameSession;
	}

	namespace PlayerSignAPI
	{
		extern FNativeGameplayTag SignIn;
		extern FNativeGameplayTag SignUp;
		extern FNativeGameplayTag CheckUserAndResendConfirmCode;
		extern FNativeGameplayTag ConfirmSignUp;
		extern FNativeGameplayTag SignOut;
	}
}
