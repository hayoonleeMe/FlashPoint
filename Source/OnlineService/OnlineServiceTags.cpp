
#include "OnlineServiceTags.h"

namespace OnlineServiceTags
{
	namespace GameSessionsAPI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CreateGameSession, "OnlineServiceTags.GameSessionsAPI.CreateGameSession", "Creates a new game session.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(DescribeGameSessionStatus, "OnlineServiceTags.GameSessionsAPI.DescribeGameSessionStatus", "Describes the status of a game session.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(DescribeGameSessions, "OnlineServiceTags.GameSessionsAPI.DescribeGameSessions", "Describes for available game sessions.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CreatePlayerSession, "OnlineServiceTags.GameSessionsAPI.CreatePlayerSession", "Creates a new player session.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TerminateGameSession, "OnlineServiceTags.GameSessionsAPI.TerminateGameSession", "Terminates a game session.")
	}

	namespace PlayerSignAPI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignIn, "OnlineServiceTags.PlayerSignAPI.SignIn", "Retrieves Access Token, Id Token, and Refresh Token.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignUp, "OnlineServiceTags.PlayerSignAPI.SignUp", "Creates a new user.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CheckUserAndResendConfirmCode, "OnlineServiceTags.PlayerSignAPI.CheckUserAndResendConfirmCode", "Checks user status and resends a confirmation code if unconfirmed.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ConfirmSignUp, "OnlineServiceTags.PlayerSignAPI.ConfirmSignUp", "Confirms a user.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignOut, "OnlineServiceTags.PlayerSignAPI.SignOut", "Signs user out and invalidates tokens.")
	}
}
