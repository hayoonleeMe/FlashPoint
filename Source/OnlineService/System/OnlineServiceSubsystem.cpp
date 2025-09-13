// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineServiceSubsystem.h"

#include "GameLiftServerSDK.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "OnlineService.h"
#include "OnlineServiceTags.h"
#include "PlayerAuthSubsystem.h"
#include "Data/APIData.h"
#include "Interfaces/IHttpResponse.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OnlineServiceSubsystem)

UOnlineServiceSubsystem::UOnlineServiceSubsystem()
{
	MinimumPasswordLength = 8;
	CreatedGameSessionStatusPollInterval = 1.f;
	MaxCreatedGameSessionStatusPollAttempts = 10;
	MaxTerminateGameSessionPollAttempts = 3;
	TerminateGameSessionPollInterval = 3.f;
}

UOnlineServiceSubsystem* UOnlineServiceSubsystem::Get(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return UGameInstance::GetSubsystem<UOnlineServiceSubsystem>(World->GetGameInstance());
	}
	return nullptr;
}

bool UOnlineServiceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 블루프린트 버전만 사용
	return IsInBlueprint() && Super::ShouldCreateSubsystem(Outer);
}

void UOnlineServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	check(PlayerSignAPIData);
	check(GameSessionsAPIData);
}

FString UOnlineServiceSubsystem::SerializeJsonContent(const TMap<FString, FString>& Params)
{
	TSharedPtr<FJsonObject> ContentJsonObject = MakeShareable(new FJsonObject);
	for (const auto& Param : Params)
	{
		ContentJsonObject->SetStringField(Param.Key, Param.Value);
	}
	FString Content;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(ContentJsonObject.ToSharedRef(), JsonWriter);
	return Content;
}

void UOnlineServiceSubsystem::DumpMetaData(const TSharedPtr<FJsonObject>& JsonObject)
{
	if (JsonObject->HasField(TEXT("$metadata")))
	{
		const TSharedPtr<FJsonObject> MetaDataJsonObject = JsonObject->GetObjectField(TEXT("$metadata"));
		FHTTPMetaData HTTPMetaData;
		FJsonObjectConverter::JsonObjectToUStruct(MetaDataJsonObject.ToSharedRef(), &HTTPMetaData);
		HTTPMetaData.Dump();
	}
}

bool UOnlineServiceSubsystem::ContainsErrors(const TSharedPtr<FJsonObject>& JsonObject, FString& OutExceptionName, FString& OutErrorMessage)
{
	//FString ErrorMessage = TEXT("Unknown Error Message");
	OutErrorMessage = TEXT("Unknown Error Message");
	bool bHasError = false;

	if (JsonObject->HasField(TEXT("errorType")))
	{
		OutExceptionName = JsonObject->GetStringField(TEXT("errorType"));
		if (JsonObject->HasField(TEXT("errorMessage")))
		{
			OutErrorMessage = JsonObject->GetStringField(TEXT("errorMessage"));
		}
		bHasError = true;
	}
	else if (JsonObject->HasField(TEXT("__type")))
	{
		OutExceptionName = JsonObject->GetStringField(TEXT("__type"));
		if (JsonObject->HasField(TEXT("message")))
		{
			OutErrorMessage = JsonObject->GetStringField(TEXT("message"));
		}
		bHasError = true;
	}
	else if (JsonObject->HasField(TEXT("$fault")))
	{
		if (JsonObject->HasField(TEXT("name")))
		{
			OutExceptionName = JsonObject->GetStringField(TEXT("name"));
		}
		else
		{
			OutExceptionName = TEXT("FaultException");
		}
        
		if (JsonObject->HasField(TEXT("message")))
		{
			OutErrorMessage = JsonObject->GetStringField(TEXT("message"));
		}
		bHasError = true;
	}

	if (bHasError)
	{
		UE_LOG(LogOnlineService, Error, TEXT("AWS Error Detected. Type: [%s], Message: [%s]"), *OutExceptionName, *OutErrorMessage);
		return true;
	}
    
	return false;
}

template <typename T>
bool UOnlineServiceSubsystem::ParseAPIResponse(FHttpResponsePtr Response, const FString& APINameToLog, T& OutStruct, FString* OutExceptionName, FString* OutErrorMessage)
{
	if (!Response.IsValid())
	{
		UE_LOG(LogOnlineService, Error, TEXT("[%s] Request failed. Response is invalid."), *APINameToLog);
		return false;
	}

	const int32 ResponseCode = Response->GetResponseCode();
	UE_LOG(LogOnlineService, Log, TEXT("[%s] HTTP Response Code: %d"), *APINameToLog, ResponseCode);
	
	TSharedPtr<FJsonObject> JsonObject;
	const FString ResponseBody = Response->GetContentAsString();
	if (!ResponseBody.IsEmpty())
	{
		// JSON parsing
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseBody);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			// 오류가 존재하면 OutExceptionName, OutErrorMessage를 채우고 false 반환
			FString ExceptionName, ErrorMessage;
			if (ContainsErrors(JsonObject, ExceptionName, ErrorMessage))
			{
				if (OutExceptionName)
				{
					*OutExceptionName = ExceptionName;
				}
				if (OutErrorMessage)
				{
					*OutErrorMessage = ErrorMessage;
				}
				return false; 
			}
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("[%s] Response JSON parsing failed."), *APINameToLog);
			return false;
		}
	}

	if (!EHttpResponseCodes::IsOk(ResponseCode))
	{
		UE_LOG(LogOnlineService, Error, TEXT("[%s] HTTP Response Code is not OK."), *APINameToLog);
		return false;
	}

	if (JsonObject.IsValid())
	{
		if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &OutStruct))
		{
			UE_LOG(LogOnlineService, Error, TEXT("[%s] Response FJsonObjectConverter failed."), *APINameToLog);
			return false;
		}
	}

	return true;
}

bool UOnlineServiceSubsystem::ParseAPIResponse(FHttpResponsePtr Response, const FString& APINameToLog, FString* OutExceptionName, FString* OutErrorMessage)
{
	if (!Response.IsValid())
	{
		UE_LOG(LogOnlineService, Error, TEXT("[%s] Request failed. Response is invalid."), *APINameToLog);
		return false;
	}

	const int32 ResponseCode = Response->GetResponseCode();
	UE_LOG(LogOnlineService, Log, TEXT("[%s] HTTP Response Code: %d"), *APINameToLog, ResponseCode);
	
	TSharedPtr<FJsonObject> JsonObject;
	const FString ResponseBody = Response->GetContentAsString();
	if (!ResponseBody.IsEmpty())
	{
		// JSON parsing
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseBody);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			// 오류가 존재하면 OutExceptionName, OutErrorMessage를 채우고 false 반환
			FString ExceptionName, ErrorMessage;
			if (ContainsErrors(JsonObject, ExceptionName, ErrorMessage))
			{
				if (OutExceptionName)
				{
					*OutExceptionName = ExceptionName;
				}
				if (OutErrorMessage)
				{
					*OutErrorMessage = ErrorMessage;
				}
				return false; 
			}
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("[%s] Response JSON parsing failed."), *APINameToLog);
			return false;
		}
	}

	if (!EHttpResponseCodes::IsOk(ResponseCode))
	{
		UE_LOG(LogOnlineService, Error, TEXT("[%s] HTTP Response Code is not OK."), *APINameToLog);
		return false;
	}

	return true;
}

bool UOnlineServiceSubsystem::ParseAPIResponse(FHttpResponsePtr Response, const FString& APINameToLog, TSharedPtr<FJsonObject>& OutJsonObject, FString* OutExceptionName,
	FString* OutErrorMessage)
{
	if (!Response.IsValid())
	{
		UE_LOG(LogOnlineService, Error, TEXT("[%s] Request failed. Response is invalid."), *APINameToLog);
		return false;
	}

	const int32 ResponseCode = Response->GetResponseCode();
	UE_LOG(LogOnlineService, Log, TEXT("[%s] HTTP Response Code: %d"), *APINameToLog, ResponseCode);
	
	const FString ResponseBody = Response->GetContentAsString();
	if (!ResponseBody.IsEmpty())
	{
		// JSON parsing
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseBody);
		if (FJsonSerializer::Deserialize(JsonReader, OutJsonObject) && OutJsonObject.IsValid())
		{
			// 오류가 존재하면 OutExceptionName, OutErrorMessage를 채우고 false 반환
			FString ExceptionName, ErrorMessage;
			if (ContainsErrors(OutJsonObject, ExceptionName, ErrorMessage))
			{
				if (OutExceptionName)
				{
					*OutExceptionName = ExceptionName;
				}
				if (OutErrorMessage)
				{
					*OutErrorMessage = ErrorMessage;
				}
				return false; 
			}
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("[%s] Response JSON parsing failed."), *APINameToLog);
			return false;
		}
	}

	if (!EHttpResponseCodes::IsOk(ResponseCode))
	{
		UE_LOG(LogOnlineService, Error, TEXT("[%s] HTTP Response Code is not OK."), *APINameToLog);
		return false;
	}

	return true;
}

void UOnlineServiceSubsystem::RefreshTokens(const FString& RefreshToken)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::RefreshTokens_Response);

	const TMap<FString, FString> Params =
	{
		{ TEXT("refreshToken"), RefreshToken }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	UE_LOG(LogOnlineService, Log, TEXT("Requesting RefreshTokens with data: %s"), *Content);
	
	const FString APIUrl = PlayerSignAPIData->GetAPIEndpoint(OnlineServiceTags::PlayerSignAPI::SignIn);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::SignIn(const FString& Username, const FString& Password)
{
	SignInStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignIn::SignInPending, false);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SignIn_Response);

	LastUsername = Username;
	const TMap<FString, FString> Params =
	{
		{ TEXT("username"), Username },
		{ TEXT("password"), Password }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	UE_LOG(LogOnlineService, Log, TEXT("Requesting SignIn with data: %s"), *Content);
	
	const FString APIUrl = PlayerSignAPIData->GetAPIEndpoint(OnlineServiceTags::PlayerSignAPI::SignIn);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::SignUp(const FString& Username, const FString& Password, const FString& Email)
{
	SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::SignUpPending, false);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SignUp_Response, Username);

	LastUsername = Username;
	const TMap<FString, FString> Params =
	{
		{ TEXT("username"), Username },
		{ TEXT("password"), Password },
		{ TEXT("email"), Email }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	UE_LOG(LogOnlineService, Log, TEXT("Requesting SignUp with data: %s"), *Content);
	
	const FString APIUrl = PlayerSignAPIData->GetAPIEndpoint(OnlineServiceTags::PlayerSignAPI::SignUp);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::CheckUserAndResendConfirmCode(const FString& Username)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::CheckUserAndResendConfirmCode_Response);

	const TMap<FString, FString> Params =
	{
		{ TEXT("username"), Username },
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	UE_LOG(LogOnlineService, Log, TEXT("Requesting CheckUserAndResendConfirmCode with data: %s"), *Content);
	
	const FString APIUrl = PlayerSignAPIData->GetAPIEndpoint(OnlineServiceTags::PlayerSignAPI::CheckUserAndResendConfirmCode);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::ConfirmSignUp(const FString& ConfirmationCode)
{
	ConfirmSignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::ConfirmSignUp::CheckConfirmCodePending, false);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::ConfirmSignUp_Response);

	const TMap<FString, FString> Params =
	{
		{ TEXT("username"), LastUsername },
		{ TEXT("confirmationCode"), ConfirmationCode }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	UE_LOG(LogOnlineService, Log, TEXT("Requesting ConfirmSignUp with data: %s"), *Content);
	
	const FString APIUrl = PlayerSignAPIData->GetAPIEndpoint(OnlineServiceTags::PlayerSignAPI::ConfirmSignUp);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("PUT"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::SignOut(const FString& AccessToken)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SignOut_Response);

	const TMap<FString, FString> Params =
	{
		{ TEXT("accessToken"), AccessToken }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);

	UE_LOG(LogOnlineService, Log, TEXT("Requesting SignOut with data: %s"), *Content);
	
	const FString APIUrl = PlayerSignAPIData->GetAPIEndpoint(OnlineServiceTags::PlayerSignAPI::SignOut);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::RefreshTokens_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("RefreshTokens request failed."));
		return;
	}

	FInitiateAuthResponse InitiateAuthResponse;
	if (ParseAPIResponse(Response, TEXT("RefreshTokens"), InitiateAuthResponse, nullptr, nullptr))
	{
		InitiateAuthResponse.Dump();
		
		if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
		{
			PlayerAuthSubsystem->UpdateTokens(InitiateAuthResponse.AuthenticationResult.AccessToken, InitiateAuthResponse.AuthenticationResult.IdToken);
		}
	}
	else
	{
		UE_LOG(LogOnlineService, Error, TEXT("Failed to handle RefreshTokens response."));
	}
}

void UOnlineServiceSubsystem::SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("SignIn request failed."));
		SignInStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		return;
	}

	FString ExceptionName;
	FInitiateAuthResponse InitiateAuthResponse;
	if (ParseAPIResponse(Response, TEXT("SignIn"), InitiateAuthResponse, &ExceptionName, nullptr))
	{
		InitiateAuthResponse.Dump();
		
		if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
		{
			PlayerAuthSubsystem->InitializeTokens(LastUsername, InitiateAuthResponse.Email, InitiateAuthResponse.AuthenticationResult, this);
		}
		OnSignInSucceededDelegate.Broadcast();
	}
	else
	{
		if (ExceptionName.Contains(TEXT("NotAuthorizedException")))
		{
			UE_LOG(LogOnlineService, Warning, TEXT("SignIn Error: NotAuthorizedException (Wrong password or user not confirmed)"));
			SignInStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignIn::IncorrectCredentials, true);
		}
		else if (ExceptionName.Contains(TEXT("UserNotFoundException")))
		{
			UE_LOG(LogOnlineService, Warning, TEXT("SignIn Error: UserNotFoundException"));
			SignInStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignIn::UserNotFound, true);
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("Failed to handle SignIn response."));
			SignInStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		}
	}
}

void UOnlineServiceSubsystem::SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString Username)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("SignUp request failed."));
		SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		return;
	}

	FSignUpResponse SignUpResponse;
	FString ExceptionName;
	if (ParseAPIResponse(Response, TEXT("SignUp"), SignUpResponse, &ExceptionName, nullptr))
	{
		SignUpResponse.Dump();
		
		OnSignUpSucceededDelegate.Broadcast(SignUpResponse.CodeDeliveryDetails.Destination);
	}
	else
	{
		if (ExceptionName.Contains(TEXT("UsernameExistsException")))
		{
			// 겹치는 Username 중복 표시
			UE_LOG(LogOnlineService, Warning, TEXT("SignUp Error: UsernameExistsException"));
			SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::UsernameExists, true);
		}
		else if (ExceptionName.Contains(TEXT("UserLambdaValidationException")))
		{
			// CognitoPreSignUp_UniqueEmail Lambda 함수에 의해 오류 발생
			// UNCONFIRMED 유저라면, Confirmation Code 재전송
			CheckUserAndResendConfirmCode(Username);
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("Failed to handle SignUp response."));
			SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		}
	}
}

void UOnlineServiceSubsystem::CheckUserAndResendConfirmCode_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("CheckUserAndResendConfirmCode request failed."));
		SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		return;
	}

	FString ExceptionName;
	FCodeDeliveryDetails CodeDeliveryDetails;
	if (ParseAPIResponse(Response, TEXT("CheckUserAndResendConfirmCode"), CodeDeliveryDetails, &ExceptionName, nullptr))
	{
		CodeDeliveryDetails.Dump();

		if (!CodeDeliveryDetails.Destination.IsEmpty())
		{
			// 동일한 Username, Email을 사용하는 UNCONFIRMED 유저이므로, Confirmation Code 재전송됨
			OnSignUpSucceededDelegate.Broadcast(CodeDeliveryDetails.Destination);
		}
		else
		{
			// 동일한 Username, Email을 사용하는 이미 CONFIRMED된 유저가 존재하므로 가입된 유저가 있다고 표시
			UE_LOG(LogOnlineService, Warning, TEXT("SignUp Error: UserLambdaValidationException (from Pre-signup trigger)"));
			SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::UserAlreadyRegistered, true);
		}
	}
	else
	{
		if (ExceptionName.Contains(TEXT("UserNotFoundException")))
		{
			// AdminGetUser 결과 Username에 해당하는 유저 못찾는 경우
			// 다른 Username, 동일한 Email을 사용하므로 Email 중복 표시
			UE_LOG(LogOnlineService, Warning, TEXT("SignUp Error: UserLambdaValidationException (from Pre-signup trigger)"));
			SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::EmailAlreadyInUse, true);
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("Failed to handle CheckUserAndResendConfirmCode response."));
			SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		}
	}
}

void UOnlineServiceSubsystem::ConfirmSignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("ConfirmSignUp request failed."));
		ConfirmSignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		return;
	}

	FString ExceptionName;
	if (ParseAPIResponse(Response, TEXT("ConfirmSignUp"), &ExceptionName, nullptr))
	{
		OnConfirmSignUpSucceededDelegate.Broadcast();
	}
	else
	{
		if (ExceptionName.Contains(TEXT("CodeMismatchException")))
		{
			UE_LOG(LogOnlineService, Warning, TEXT("ConfirmSignUp Error: CodeMismatchException"));
			ConfirmSignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::ConfirmSignUp::IncorrectConfirmationCode, true);
		}
		else if (ExceptionName.Contains(TEXT("ExpiredCodeException")))
		{
			UE_LOG(LogOnlineService, Warning, TEXT("ConfirmSignUp Error: ExpiredCodeException"));
			ConfirmSignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::ConfirmSignUp::ConfirmationCodeExpired, true);
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("Failed to handle ConfirmSignUp response."));
			ConfirmSignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		}
	}
}

void UOnlineServiceSubsystem::SignOut_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("SignOut request failed."));
		return;
	}
	
	if (ParseAPIResponse(Response, TEXT("SignOut"), nullptr, nullptr))
	{
		OnSignOutSucceededDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogOnlineService, Error, TEXT("Failed to handle SignOut response."));
	}
}

bool UOnlineServiceSubsystem::ValidateSignInInput(const FString& Username, const FString& Password) const
{
	const bool bIsUsernameValid = !Username.IsEmpty();
	if (!bIsUsernameValid)
	{
		SignInStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::InvalidUsername, false);
		return false;
	}
	
	const bool bIsPasswordLongEnough = Password.Len() >= MinimumPasswordLength;
	if (!bIsPasswordLongEnough)
	{
		SignInStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::PasswordTooShort, false);
		return false;
	}

	FString FailedPasswordStatusMessage;
	const bool bIsValidPassword = IsValidPassword(Password, FailedPasswordStatusMessage);
	if (!bIsValidPassword)
	{
		SignInStatusMessageDelegate.Broadcast(FailedPasswordStatusMessage, false);
		return false;
	}

	// Validated
	SignInStatusMessageDelegate.Broadcast(FString(), false);
	return true;
}

bool UOnlineServiceSubsystem::ValidateSignUpInput(const FString& Username, const FString& Password, const FString& ConfirmPassword, const FString& Email) const
{
	const bool bIsUsernameValid = !Username.IsEmpty();
	if (!bIsUsernameValid)
	{
		SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::InvalidUsername, false);
		return false;
	}
	
	const bool bIsPasswordLongEnough = Password.Len() >= MinimumPasswordLength;
	if (!bIsPasswordLongEnough)
	{
		SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::PasswordTooShort, false);
		return false;
	}

	FString FailedPasswordStatusMessage;
	const bool bIsValidPassword = IsValidPassword(Password, FailedPasswordStatusMessage);
	if (!bIsValidPassword)
	{
		SignUpStatusMessageDelegate.Broadcast(FailedPasswordStatusMessage, false);
		return false;
	}
	
	const bool bArePasswordEqual = Password.Equals(ConfirmPassword);
	if (!bArePasswordEqual)
	{
		SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::PasswordsMismatch, false);
		return false;
	}
	
	const bool bIsValidEmail = IsValidEmail(Email);
	if (!bIsValidEmail)
	{
		SignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SignUp::InvalidEmail, false);
		return false;
	}

	// Validated
	SignUpStatusMessageDelegate.Broadcast(FString(), false);
	return true;
}

bool UOnlineServiceSubsystem::IsValidEmail(const FString& Email)
{
	/**
	 * R : Raw String으로, Escape Sequence(\n, \t, \" 등)을 해석하지 않고 그대로 문자열로 처리함. 줄바꿈도 가능
	 *
	 * Regex
	 * ^			: 문자열의 시작을 의미
	 * [^\s@]+		: 공백(\s) 또는 @을 포함하지 않는(^는 부정) 문자들의 연속된 그룹 (즉, 로컬 파트)
	 * @			: @ 기호가 정확히 한 개 포함되어야 함
	 * [^\s@]+		: 공백(\s) 또는 @을 포함하지 않는 문자들의 연속된 그룹 (즉, 도메인 네임)
	 * \.			: . 문자 그대로 (정규식에서 .는 "아무 문자"를 의미하는 기능)
	 * [^\s@]{2,}	: 공백(\s) 또는 @을 포함하지 않는 최소 2자 이상의 문자 그룹 (즉, .com, .net 같은 TLD)
	 * $			: 문자열의 끝을 의미
	 */
	
	const FRegexPattern EmailPattern(TEXT(R"(^[^\s@]+@[^\s@]+\.[^\s@]{2,}$)"));
	
	FRegexMatcher Matcher(EmailPattern, Email);
	return Matcher.FindNext();	// 일치하면 true 반환
}

bool UOnlineServiceSubsystem::IsValidPassword(const FString& Password, FString& OutStatusMessage)
{
	// contains at least one number
	const FRegexPattern NumberPattern(TEXT(R"(\d)"));
	
	FRegexMatcher NumberMatcher(NumberPattern, Password);
	if (!NumberMatcher.FindNext())
	{
		OutStatusMessage = OnlineServiceStatusMessage::SignUp::PasswordMissingNumber;
		return false;
	}

	// contains at least one special character
	// \w : a~z(both upper, lower case), 0~9, _
	// \s : 공백
	const FRegexPattern SpecialCharPattern(TEXT(R"([^\w\s])"));
	
	FRegexMatcher SpecialCharMatcher(SpecialCharPattern, Password);
	if (!SpecialCharMatcher.FindNext())
	{
		OutStatusMessage = OnlineServiceStatusMessage::SignUp::PasswordMissingSpecialChar;
		return false;
	}

	// contains at least one upper case character
	const FRegexPattern UpperCasePattern(TEXT(R"([A-Z])"));
	
	FRegexMatcher UpperCaseMatcher(UpperCasePattern, Password);
	if (!UpperCaseMatcher.FindNext())
	{
		OutStatusMessage = OnlineServiceStatusMessage::SignUp::PasswordMissingUppercase;
		return false;
	}

	// contains at least one lower case character
	const FRegexPattern LowerCasePattern(TEXT(R"([a-z])"));
	
	FRegexMatcher LowerCaseMatcher(LowerCasePattern, Password);
	if (!LowerCaseMatcher.FindNext())
	{
		OutStatusMessage = OnlineServiceStatusMessage::SignUp::PasswordMissingLowercase;
		return false;
	}

	// Validated
	return true;
}

bool UOnlineServiceSubsystem::ValidateConfirmationCode(const FString& Code) const
{
	const FRegexPattern SixDigitsPattern(TEXT(R"(^\d{6}$)"));
	FRegexMatcher Matcher(SixDigitsPattern, Code);
	
	const bool bValidConfirmationCode = Matcher.FindNext();
	if (bValidConfirmationCode)
	{
		ConfirmSignUpStatusMessageDelegate.Broadcast(FString(), false);
	}
	else
	{
		ConfirmSignUpStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::ConfirmSignUp::InvalidConfirmationCode, false);
	}

	return bValidConfirmationCode;
}

void UOnlineServiceSubsystem::InitGameLift()
{
#if WITH_GAMELIFT
	if (bGameLiftInitialized)
	{
		return;
	}

	// Getting the module first.
	FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	// Define the server parameters for a GameLift Anywhere fleet. These are not needed for a GameLift managed EC2 fleet.
	// 이때 fleet은 이 서버 프로그램을 가동하는 hosting resources를 나타낸다. AWS EC2 fleet이 될 수도 있고, 직접 서버를 가동하면 개인 하드웨어가 될 수도 있다.
	FServerParameters ServerParameters;

	SetServerParameters(ServerParameters);

	// InitSDK establishes a local connection with GameLift's agent to enable further communication.
	// Use InitSDK(serverParameters) for a GameLift Anywhere fleet. 
	// Use InitSDK() for a GameLift managed EC2 fleet.
	FGameLiftGenericOutcome InitSdkOutcome;
	if (bIsAnywhereActive)
	{
		InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParameters);
	}
	else
	{
		InitSdkOutcome = GameLiftSdkModule->InitSDK(); 
	}

	if (!InitSdkOutcome.IsSuccess())
	{
		UE_LOG(LogOnlineService, SetColor, TEXT("%s"), COLOR_RED);
		UE_LOG(LogOnlineService, Log, TEXT("ERROR: InitSDK failed : ("));
		FGameLiftError GameLiftError = InitSdkOutcome.GetError();
		UE_LOG(LogOnlineService, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
		UE_LOG(LogOnlineService, SetColor, TEXT("%s"), COLOR_NONE);
		return;
	}

	ProcessParameters = MakeShared<FProcessParameters>();

	auto OnGameSession = [this, GameLiftSdkModule](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		FString GameSessionId = FString(gameSession.GetGameSessionId());
		UE_LOG(LogOnlineService, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);

		FGameSessionInfo GameSessionInfo;
		GameSessionInfo.GameSessionId = gameSession.GetGameSessionId();
		GameSessionInfo.CreatorId = gameSession.GetGameSessionData();	// CreatorId는 GameSessionData 필드에 저장하도록 설정됨
		GameSessionInfo.MaximumPlayerSessionCount = gameSession.GetMaximumPlayerSessionCount();
		GameSessionInfo.DnsName = gameSession.GetDnsName();
		GameSessionInfo.FleetId = gameSession.GetFleetId();
		GameSessionInfo.IpAddress = gameSession.GetIpAddress();
		GameSessionInfo.Port = gameSession.GetPort();
		GameSessionInfo.Status = Aws::GameLift::Server::Model::GameSessionStatusMapper::GetNameForGameSessionStatus(gameSession.GetStatus());
		GameSessionInfo.Name = gameSession.GetName();
		GameSessionInfo.MatchmakerData = gameSession.GetMatchmakerData();
		
		int32 GamePropertiesCount = 0;
		const Aws::GameLift::Server::Model::GameProperty* GamePropertyArray = gameSession.GetGameProperties(GamePropertiesCount);
		if (GamePropertyArray && GamePropertiesCount)
		{
			GameSessionInfo.GameProperties.Reserve(GamePropertiesCount);
			
			for (int32 Index = 0; Index < GamePropertiesCount; ++Index)
			{
				const Aws::GameLift::Server::Model::GameProperty& CurrentProp = GamePropertyArray[Index];
				FKeyValueStruct NewProp;
				NewProp.Key = FString(CurrentProp.GetKey());
				NewProp.Value = FString(CurrentProp.GetValue());
				GameSessionInfo.GameProperties.Add(NewProp);
			}
		}
		
		OnServerStartGameSessionDelegate.Broadcast(GameSessionInfo);
		
		GameLiftSdkModule->ActivateGameSession();
	};
	ProcessParameters->OnStartGameSession.BindLambda(OnGameSession);

	// ProcessParameters.OnTerminate : 이 게임 서버의 호스팅 인스턴스가 종료되기 전에 추가 작업을 수행하기 위해 호출된다.
	auto OnProcessTerminate = [this, GameLiftSdkModule]()
	{
		UE_LOG(LogOnlineService, Log, TEXT("Game Server process is terminating."));

		OnProcessTerminateDelegate.Broadcast();
		
		GameLiftSdkModule->ProcessEnding();
	};
	ProcessParameters->OnTerminate.BindLambda(OnProcessTerminate);

	// GameLift invokes this callback approximately every 60 seconds.
	// false를 반환하면 서버가 종료된다.
	auto OnHealthCheck = [this]() 
	{
		UE_LOG(LogOnlineService, Log, TEXT("Performing Health Check"));

		OnHealthCheckDelegate.Broadcast();
		
		return true;
	};
	ProcessParameters->OnHealthCheck.BindLambda(OnHealthCheck);

	// 플레이어가 연결할 Port 설정
	// Command Line Arguments로 넘길 경우 해당 Port를 사용하고, 아니라면 UE의 기본 포트인 7777 사용 
	int32 Port = FURL::UrlConfig.DefaultPort;
	ParseCommandLinePort(Port);
	ProcessParameters->port = Port;

	// GameSession Log를 저장할 위치 저장
	TArray<FString> LogFiles;
	LogFiles.Add(TEXT("ProjectF/Saved/Logs/ProjectF.log"));
	ProcessParameters->logParameters = LogFiles;

	// The game server calls ProcessReady() to tell GameLift it's ready to host game sessions.
	UE_LOG(LogOnlineService, Log, TEXT("Calling Process Ready."));
	FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParameters);

    if (!ProcessReadyOutcome.IsSuccess())
    {
    	UE_LOG(LogOnlineService, SetColor, TEXT("%s"), COLOR_RED);
    	UE_LOG(LogOnlineService, Log, TEXT("ERROR: Process Ready Failed!"));
    	FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
    	UE_LOG(LogOnlineService, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
    	UE_LOG(LogOnlineService, SetColor, TEXT("%s"), COLOR_NONE);
    	return;
    }

	UE_LOG(LogOnlineService, Log, TEXT("InitGameLift completed!"));

	bGameLiftInitialized = true;
#endif
}

void UOnlineServiceSubsystem::TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& OutErrorMessage)
{
#if WITH_GAMELIFT
	if (PlayerSessionId.IsEmpty() || Username.IsEmpty())
	{
		OutErrorMessage = TEXT("PlayerSessionId and/or Username invalid.");
		return;
	}
	
	Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
	DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId)); // const TCHAR* -> const char* ; char = ANSICHAR

	const auto& DescribePlayerSessionOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
	if (!DescribePlayerSessionOutcome.IsSuccess())
	{
		OutErrorMessage = TEXT("DescribePlayerSessions failed.");
		return;
	}

	const auto& DescribePlayerSessionsResult = DescribePlayerSessionOutcome.GetResult();
	int32 Count = 0;
	const Aws::GameLift::Server::Model::PlayerSession* PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
	if (PlayerSessions == nullptr || Count == 0)
	{
		OutErrorMessage = TEXT("GetPlayerSessions failed.");
		return;
	}

	for (int32 i = 0; i < Count; ++i)
	{
		const Aws::GameLift::Server::Model::PlayerSession& PlayerSession = PlayerSessions[i];
		if (!Username.Equals(PlayerSession.GetPlayerId()))
		{
			continue;
		}

		if (PlayerSession.GetStatus() != Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED)
		{
			OutErrorMessage = FString::Printf(TEXT("Session for %s not RESERVED; fail PreLogin."), *Username);
			return;
		}

		const auto& AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
		if (!AcceptPlayerSessionOutcome.IsSuccess())
		{
			OutErrorMessage = FString::Printf(TEXT("Failed to accept player session for %s."), *Username);
			return;
		}
	}
#endif
}

void UOnlineServiceSubsystem::RemovePlayerSession(const FString& PlayerSessionId)
{
#if WITH_GAMELIFT
	if (!PlayerSessionId.IsEmpty())
	{
		Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
	}
#endif
}

void UOnlineServiceSubsystem::SetServerParameters(FServerParameters& OutServerParameters)
{
#if WITH_GAMELIFT
	// Anywhere Fleet을 사용하는지 체크
	FString AnywhereFleet;
	if (!FParse::Value(FCommandLine::Get(), TEXT("-anywherefleet"), AnywhereFleet))
	{
		bIsAnywhereActive = false;
		return;
	}

	bIsAnywhereActive = true;
	
	// AuthToken returned from the "aws gamelift get-compute-auth-token" API. Note this will expire and require a new call to the API after 15 minutes.
	// 서버를 Command Line으로 돌릴 때, Command Line Argument를 이용해 AuthToken 정보를 저장한다.
	if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), OutServerParameters.m_authToken))
	{
		UE_LOG(LogOnlineService, Log, TEXT("AUTH_TOKEN: %s"), *OutServerParameters.m_authToken)
	}
	
	// The Host/compute-name of the GameLift Anywhere instance.
	if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), OutServerParameters.m_hostId))
	{
		UE_LOG(LogOnlineService, Log, TEXT("HOST_ID: %s"), *OutServerParameters.m_hostId)
	}
	
	// The Anywhere Fleet ID.
	if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), OutServerParameters.m_fleetId))
	{
		UE_LOG(LogOnlineService, Log, TEXT("FLEET_ID: %s"), *OutServerParameters.m_fleetId)
	}
	
	// The WebSocket URL (GameLiftServiceSdkEndpoint).
	if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), OutServerParameters.m_webSocketUrl))
	{
		UE_LOG(LogOnlineService, Log, TEXT("WEBSOCKET_URL: %s"), *OutServerParameters.m_webSocketUrl)
	}
	
	// The PID of the running process
	OutServerParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
	UE_LOG(LogOnlineService, Log, TEXT("PID: %s"), *OutServerParameters.m_processId);
#endif
}

void UOnlineServiceSubsystem::ParseCommandLinePort(int32& OutPort)
{
#if WITH_GAMELIFT
	// Command Line에서 Token과 switch로 나눈다.
	TArray<FString> CommandLineTokens;
	TArray<FString> CommandLineSwitches;
	FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

	// port=???? 꼴의 Switch를 찾아 OutPort를 해당 Value로 설정한다.
	for (const FString& Switch : CommandLineSwitches)
	{
		FString Key;
		FString Value;
		if (Switch.Split("=", &Key, &Value))
		{
			if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase))
			{
				OutPort = FCString::Atoi(*Value);
				return;
			}
		}
	}
#endif
}

void UOnlineServiceSubsystem::CreateGameSession(const FString& RoomName, const FString& MatchMode, const FString& MaxPlayers, const FString& CreatorId)
{
	CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::CreateGameSession::CreateMatchPending, false);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::CreateGameSession_Response);

	const TMap<FString, FString> Params =
	{
		{ TEXT("roomName"), RoomName },
		{ TEXT("matchMode"), MatchMode },
		{ TEXT("maxPlayers"), MaxPlayers },
		{ TEXT("creatorId"), CreatorId }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	UE_LOG(LogOnlineService, Log, TEXT("Requesting CreateGameSession with data: %s"), *Content);

	const FString APIUrl = GameSessionsAPIData->GetAPIEndpoint(OnlineServiceTags::GameSessionsAPI::CreateGameSession);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
	{
		Request->SetHeader(TEXT("Authorization"), PlayerAuthSubsystem->GetAuthResult().AccessToken);
		UE_LOG(LogOnlineService, Log, TEXT("  with AccessToken %s"), *PlayerAuthSubsystem->GetAuthResult().AccessToken);
	}

	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::DescribeGameSessionStatus(const FString& GameSessionId)
{
	UE_LOG(LogOnlineService, Log, TEXT("Requesting DescribeGameSessionStatus..."));
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::DescribeGameSessionStatus_Response);
	
	FString APIUrl = GameSessionsAPIData->GetAPIEndpoint(OnlineServiceTags::GameSessionsAPI::DescribeGameSessionStatus);
	APIUrl.Append(FString::Printf(TEXT("?gameSessionId=%s"), *GameSessionId));
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::DescribeGameSessions()
{
	DescribeGameSessionsStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::DescribeGameSessions::FindMatchPending, false);

	UE_LOG(LogOnlineService, Log, TEXT("Requesting DescribeGameSessions..."));
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::DescribeGameSessions_Response);
	
	const FString APIUrl = GameSessionsAPIData->GetAPIEndpoint(OnlineServiceTags::GameSessionsAPI::DescribeGameSessions);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::CreatePlayerSession(const FString& Username, const FString& GameSessionId)
{
	CreatePlayerSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::CreatePlayerSession::JoinMatchPending, false);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::CreatePlayerSession_Response);

	const TMap<FString, FString> Params =
	{
		{ TEXT("playerId"), Username },
		{ TEXT("gameSessionId"), GameSessionId }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	UE_LOG(LogOnlineService, Log, TEXT("Requesting CreatePlayerSession with data: %s"), *Content);
	
	const FString APIUrl = GameSessionsAPIData->GetAPIEndpoint(OnlineServiceTags::GameSessionsAPI::CreatePlayerSession);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
	{
		Request->SetHeader(TEXT("Authorization"), PlayerAuthSubsystem->GetAuthResult().AccessToken);
		UE_LOG(LogOnlineService, Log, TEXT("  with AccessToken %s"), *PlayerAuthSubsystem->GetAuthResult().AccessToken);
	}
	
	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::TerminateGameSession(const FString& GameSessionId)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::TerminateGameSession_Response, GameSessionId);

	const TMap<FString, FString> Params =
	{
		{ TEXT("gameSessionId"), GameSessionId }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);

	UE_LOG(LogOnlineService, Log, TEXT("Requesting TerminateGameSession with data: %s"), *Content);
	
	const FString APIUrl = GameSessionsAPIData->GetAPIEndpoint(OnlineServiceTags::GameSessionsAPI::TerminateGameSession);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->ProcessRequest();
}

void UOnlineServiceSubsystem::CreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("CreateGameSession request failed."));
		CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		return;
	}

	FString ExceptionName;
	if (ParseAPIResponse(Response, TEXT("CreateGameSession"), CreatedGameSession, &ExceptionName, nullptr))
	{
		UE_LOG(LogOnlineService, Log, TEXT("CreateGameSession Succeeded"));
		CreatedGameSession.Dump();
			
		PollCreatedGameSessionStatus();
	}
	else
	{
		if (ExceptionName.Contains(TEXT("FleetCapacityExceededException")))
		{
			UE_LOG(LogOnlineService, Warning, TEXT("CreateGameSession Error: FleetCapacityExceededException. Fleet capacity is full."));
			CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::CreateGameSession::ServerCapacityFull, true);
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("Failed to handle CreateGameSession response."));
			CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		}
	}
}

void UOnlineServiceSubsystem::DescribeGameSessionStatus_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		CreatedGameSessionStatusPollAttemptCount = 0;
		
		UE_LOG(LogOnlineService, Error, TEXT("DescribeGameSessionStatus request failed."));
		CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	FString ExceptionName;
	if (ParseAPIResponse(Response, TEXT("DescribeGameSessionStatus"), JsonObject, &ExceptionName, nullptr))
	{
		FString GameSessionStatus;
		if (JsonObject->TryGetStringField(TEXT("Status"), GameSessionStatus))
		{
			if (GameSessionStatus == TEXT("ACTIVE"))
			{
				CreatedGameSessionStatusPollAttemptCount = 0;
				CreatePlayerSession(LastUsername, CreatedGameSession.GameSessionId);
			}
			else if (GameSessionStatus == TEXT("ACTIVATING"))
			{
				PollCreatedGameSessionStatus();
			}
			else
			{
				CreatedGameSessionStatusPollAttemptCount = 0;

				UE_LOG(LogOnlineService, Warning, TEXT("Polling stopped. GameSession entered a final state: %s"), *GameSessionStatus);
				CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::CreateGameSession::GameSessionStartFailed, true);
			}
		}
		else
		{
			CreatedGameSessionStatusPollAttemptCount = 0;
			
			UE_LOG(LogOnlineService, Error, TEXT("DescribeGameSessionStatus Error: 'Status' field was not found in the response."));
			CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		}
	}
	else
	{
		CreatedGameSessionStatusPollAttemptCount = 0;
		
		if (ExceptionName.Contains(TEXT("NotFoundException")))
		{
			UE_LOG(LogOnlineService, Warning, TEXT("DescribeGameSessionStatus Error: NotFoundException. The session may have been terminated."));
			CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::CreateGameSession::GameSessionStartFailed, true);
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("Failed to handle DescribeGameSessionStatus response."));
			CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		}
	}
}

void UOnlineServiceSubsystem::PollCreatedGameSessionStatus()
{
	if (CreatedGameSessionStatusPollAttemptCount < MaxCreatedGameSessionStatusPollAttempts)
	{
		++CreatedGameSessionStatusPollAttemptCount;

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(DescribeGameSessionStatusPollTimer, FTimerDelegate::CreateLambda([this]()
			{
				DescribeGameSessionStatus(CreatedGameSession.GameSessionId);
			}), CreatedGameSessionStatusPollInterval, false);
		}
	}
	else
	{
		UE_LOG(LogOnlineService, Error, TEXT("Created Game Session Status Polling Timed Out, Id %s"), *CreatedGameSession.GameSessionId);
		CreateGameSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		CreatedGameSessionStatusPollAttemptCount = 0;
	}
}

void UOnlineServiceSubsystem::DescribeGameSessions_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("DescribeGameSessions request failed."));
		DescribeGameSessionsStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		return;
	}

	FDescribeGameSessionsResponse DescribeResponse;
	FString ExceptionName;
	if (ParseAPIResponse(Response, TEXT("DescribeGameSessions"), DescribeResponse, &ExceptionName, nullptr))
	{
		DescribeResponse.Dump();
		
		OnDescribeGameSessionsSucceededDelegate.Broadcast(DescribeResponse.GameSessions);
		DescribeGameSessionsStatusMessageDelegate.Broadcast(TEXT(""), true); 
	}
	else
	{
		if (!ExceptionName.IsEmpty())
		{
			UE_LOG(LogOnlineService, Warning, TEXT("DescribeGameSessions Error: %s"), *ExceptionName);
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("Failed to handle DescribeGameSessions response."));
		}
		DescribeGameSessionsStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::DescribeGameSessions::FindMatchesFailed, true);
	}
}

void UOnlineServiceSubsystem::CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("CreatePlayerSession request failed."));
		CreatePlayerSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		return;
	}

	FPlayerSessionInfo PlayerSessionInfo;
	FString ExceptionName;
	if (ParseAPIResponse(Response, TEXT("CreatePlayerSession"), PlayerSessionInfo, &ExceptionName, nullptr))
	{
		PlayerSessionInfo.Dump();
		
		const FString Options = FString::Printf(TEXT("?PlayerSessionId=%s?Username=%s"), *PlayerSessionInfo.PlayerSessionId, *PlayerSessionInfo.PlayerId);
		const FString Ip = bIsAnywhereActive ? TEXT("127.0.0.1") : PlayerSessionInfo.IpAddress;
		const FString IpAndPort = FString::Printf(TEXT("%s:%d"), *Ip, PlayerSessionInfo.Port);
		const FString URL = IpAndPort + Options;

		OnCreatePlayerSessionSucceededDelegate.Broadcast(URL);
	}
	else
	{
		if (ExceptionName.Contains(TEXT("GameSessionFullException")))
		{
			UE_LOG(LogOnlineService, Warning, TEXT("CreatePlayerSession Error: GameSessionFullException"));
			CreatePlayerSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::CreatePlayerSession::MatchIsFull, true);
		}
		else
		{
			UE_LOG(LogOnlineService, Error, TEXT("Failed to handle CreatePlayerSession response."));
			CreatePlayerSessionStatusMessageDelegate.Broadcast(OnlineServiceStatusMessage::SomethingWentWrong, true);
		}
	}
}

void UOnlineServiceSubsystem::TerminateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString GameSessionId)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnlineService, Error, TEXT("TerminateGameSession request failed for %s. Attempting retry..."), *GameSessionId);
		SessionIdToTerminate = GameSessionId;
		PollTerminateGameSession();
		return;
	}

	FGameSessionInfo TerminatedGameSessionInfo;
	FString ExceptionName;
	if (ParseAPIResponse(Response, TEXT("TerminateGameSession"), TerminatedGameSessionInfo, &ExceptionName, nullptr))
	{
		UE_LOG(LogOnlineService, Log, TEXT("TerminateGameSession Succeeded for SessionId: %s"), *TerminatedGameSessionInfo.GameSessionId);
		TerminateGameSessionPollCount = 0;
		SessionIdToTerminate.Empty();
		CreatedGameSession = FGameSessionInfo();
		return;
	}

	int32 ResponseCode = Response->GetResponseCode();
	
	// 재시도할 수 있는 서버 오류
	if ((ResponseCode >= 500 && ResponseCode <= 599) || ExceptionName.Contains(TEXT("InternalServiceException")) || ExceptionName.Contains(TEXT("NotReadyException")))
	{
		UE_LOG(LogOnlineService, Error, TEXT("TerminateGameSession failed for %s with a RETRYABLE error (HTTP %d, Exception: %s). Attempting retry..."), *GameSessionId, ResponseCode, *ExceptionName);
		SessionIdToTerminate = GameSessionId;
		PollTerminateGameSession();
	}
	// 재시도할 수 없는 클라이언트 오류
	else 
	{
		if (ExceptionName.Contains(TEXT("NotFoundException")))
		{
			UE_LOG(LogOnlineService, Warning, TEXT("TerminateGameSession for %s is considered successful: NotFoundException."), *GameSessionId);
		}
		else // InvalidRequest, Unauthorized 등 그 외 모든 4xx 에러 포함
		{
			UE_LOG(LogOnlineService, Error, TEXT("TerminateGameSession failed for %s with a NON-RETRYABLE client-side error (HTTP %d, Exception: %s)."), *GameSessionId, ResponseCode, *ExceptionName);
		}
		TerminateGameSessionPollCount = 0;
		SessionIdToTerminate.Empty();
	}
}

void UOnlineServiceSubsystem::PollTerminateGameSession()
{
	if (TerminateGameSessionPollCount < MaxTerminateGameSessionPollAttempts)
	{
		++TerminateGameSessionPollCount;
        
		GetWorld()->GetTimerManager().SetTimer(TerminateGameSessionPollTimer, FTimerDelegate::CreateLambda([this]()
		{
			if (!SessionIdToTerminate.IsEmpty())
			{
				TerminateGameSession(SessionIdToTerminate);
			}
		}), TerminateGameSessionPollInterval, false);
	}
	else
	{
		UE_LOG(LogOnlineService, Error, TEXT("TerminateGameSession Polling Timed Out, Id %s"), *SessionIdToTerminate);
		TerminateGameSessionPollCount = 0;
		SessionIdToTerminate.Empty();
	}
}
