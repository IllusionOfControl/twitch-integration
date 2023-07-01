// Fill out your copyright notice in the Description page of Project Settings.

#include "MyClientWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CircularThrobber.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "WebSocketsModule.h"
#include "JsonObjectConverter.h"
#include "Math/UnrealMathUtility.h"
#include "HttpServerModule.h"
#include "HttpPath.h"
#include "IHttpRouter.h"
#include "HttpServerHttpVersion.h"
#include "HttpServerResponse.h"


bool UMyClientWidget::Initialize()
{
    Super::Initialize();

    HelloButton->OnClicked.AddDynamic(this, &UMyClientWidget::HelloButtonClicked);
    LoginButton->OnClicked.AddDynamic(this, &UMyClientWidget::LoginButtonClicked);

    BroadcasterId = TEXT("214253716");
    ClientId = TEXT("xy4lgciskz02qdr32iey77rsaqk1u7");
    BaseWsServerUrl = TEXT("ws://127.0.0.1:8000/ws");

    return true;
}


void UMyClientWidget::HelloButtonClicked()
{
    UE_LOG(LogTemp, Display, TEXT("Hello button is clicked!"));

    SendHelloMessage();
}


void UMyClientWidget::LoginButtonClicked() 
{
    UE_LOG(LogTemp, Display, TEXT("Login button is clicked!"));

    LoadingThrobber->SetVisibility(ESlateVisibility::Visible);

    UE_LOG(LogTemp, Display, TEXT("Launching http server for OAauth callback"));
    StartHttpServer();

    UE_LOG(LogTemp, Display, TEXT("Opening browser for OAuth authorize"));
    OpenBrowserForAuthorization();
}


void UMyClientWidget::OpenBrowserForAuthorization() 
{
    FString AuthorizationEndpoint = TEXT("https://id.twitch.tv/oauth2/authorize");
    FString RedirectURI = TEXT("http://localhost:12345/callback");
    FString ResponseType = TEXT("token");
    FString Scope = TEXT("user:read:email chat:read moderator:manage:announcements");

    FString AuthURL = FString::Printf(
        TEXT("%s?client_id=%s&redirect_uri=%s&response_type=%s&scope=%s"),
        *AuthorizationEndpoint,
        *ClientId,
        *RedirectURI,
        *ResponseType,
        *Scope
    );

    FPlatformProcess::LaunchURL(*AuthURL, nullptr, nullptr);
}


bool UMyClientWidget::HandleCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) 
{
    if (Request.QueryParams.Contains(TEXT("access_token")))
    {
        AccessToken = Request.QueryParams[TEXT("access_token")];

        LoadingThrobber->SetVisibility(ESlateVisibility::Hidden);
        HelloButton->SetVisibility(ESlateVisibility::Visible);

        ConnectWebSocket();

        TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("");
    }
    else if (Request.QueryParams.Contains(TEXT("error")))
    {
        UE_LOG(LogTemp, Error, TEXT("OAuth error: %s"), Request.QueryParams["error_description"]);

        TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("OAuthError"), TEXT("text/html"));
    }

    return true;
}


void UMyClientWidget::StartHttpServer() {
    FHttpServerModule& httpServerModule = FHttpServerModule::Get();
    TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(12345, true);

    bool isValid = (FHttpPath(TEXT("/callback"))).IsValidPath();

    UE_LOG(LogTemp, Display, TEXT("Path valid %d"), isValid? 0 : 1);

    if (httpRouter != nullptr)
    {
        httpRouter->BindRoute(FHttpPath(TEXT("/callback")), EHttpServerRequestVerbs::VERB_GET,
            [this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { 
                for (const auto& queryParam : Request.QueryParams)
                {
                    UE_LOG(LogTemp, Log, TEXT("QueryParam = '%s' : '%s'"), *queryParam.Key, *queryParam.Value);
                }

                TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample GET"), TEXT("text/html"));
                OnComplete(MoveTemp(response));
                return true;
            });

        httpServerModule.StartAllListeners();

        _isServerStarted = true;
        UE_LOG(LogTemp, Display, TEXT("Web server started on port = %d"), 123456);
    }
    else
    {
        _isServerStarted = false;
        UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = %d"), 123456);
    }
}


void UMyClientWidget::StopHttpServer() {
    FHttpServerModule& httpServerModule = FHttpServerModule::Get();
    httpServerModule.StopAllListeners();
}


void UMyClientWidget::ConnectWebSocket() {
    const FString ServerURL = FString::Printf(TEXT("%s?access_token=%s"), *BaseWsServerUrl, *AccessToken);
    const FString ServerProtocol = TEXT("ws");

    Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);

    Socket->OnConnected().AddLambda([]() -> void {
        UE_LOG(LogTemp, Display, TEXT("Socket connected."));
        });

    Socket->OnMessage().AddLambda([&](const FString& Message) -> void {
        if (Message == TEXT("plus")) {
            Score = FMath::Clamp(Score++, -10, 10);
            UpdateScore();
        }

        if (Message == TEXT("minus")) {
            Score = FMath::Clamp(Score--, -10, 10);
            UpdateScore();
        }
        });

    Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
        UE_LOG(LogTemp, Display, TEXT("Socket closed."));
        });

    Socket->OnConnectionError().AddLambda([](const FString& Error) -> void {
        UE_LOG(LogTemp, Error, TEXT("Socket error: %s"), *Error);
        });

    UE_LOG(LogTemp, Display, TEXT("Socket connecting...."));
    Socket->Connect();
}

void UMyClientWidget::UpdateScore() {
    FString MyString = FString::Printf(TEXT("%d"), Score);
    ScoreText->SetText(FText::FromString(MyString));
}



void UMyClientWidget::GetTwitchUser()
{
    //  TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    //  TSharedPtr<FJsonObject> JsonObject;

    //  UE_LOG(LogTemp, Display, TEXT("Preparing get user request"));
    //  
    //  FString Url = TEXT("https://api.twitch.tv/helix/users");

    //  HttpRequest->SetURL(Url);
    //  HttpRequest->SetHeader(TEXT("Client-ID"), TEXT("xy4lgciskz02qdr32iey77rsaqk1u7"));
    //  HttpRequest->SetHeader(TEXT("Authorization"), TEXT("Bearer "));
    //  HttpRequest->SetVerb(TEXT("Get"));

    //  TSharedPtr<FJsonObject> RequestPayload = MakeShareable(new FJsonObject);
    //  RequestPayload->SetStringField(TEXT("message"), TEXT("Hello, chat!"));
    //  RequestPayload->SetStringField(TEXT("color"), TEXT("orange"));

    //  FString JsonString;
    //  TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonString);
    //  FJsonSerializer::Serialize(RequestPayload.ToSharedRef(), Writer);

    //  UE_LOG(LogTemp, Display, TEXT("Serialized JSON: %s"), *JsonString);

    //  TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    //  HttpRequest->SetContentAsString(TEXT("{}"));

    //  HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess) {
    //      if (bSuccess && Response.IsValid())
    //      {
    //          int32 code = Response->GetResponseCode();
    //          UE_LOG(LogTemp, Display, TEXT("HTTP Response code: %d"), code);

    //          //FString ResponseString = Response->GetContentAsString();
    //          //TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);

    ///*          if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid()) {
    //              FString Value;

    //              if (JsonObject->TryGetStringField(TEXT("key"), Value));
    //          }*/
    //      }
    //      else
    //      {
    //          // Handle request failure
    //          UE_LOG(LogTemp, Error, TEXT("HTTP Request failed!"));
    //      }
    //      });

    //  // Execute the request

    //  UE_LOG(LogTemp, Display, TEXT("Send Request"));

    //  HttpRequest->ProcessRequest();
}

void UMyClientWidget::SendHelloMessage()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    TSharedPtr<FJsonObject> JsonObject;

    UE_LOG(LogTemp, Display, TEXT("Preparing announcement request to chat: %s"), *BroadcasterId);

    FString BaseUrl = TEXT("https://api.twitch.tv/helix/chat/announcements");
    FString UrlWithParams = FString::Printf(TEXT("%s?broadcaster_id=%s&moderator_id=%s"), *BaseUrl, *BroadcasterId, *BroadcasterId);
    FString AuthorizationHeader = FString::Printf(TEXT("Bearer %s"), *AccessToken);

    HttpRequest->SetURL(UrlWithParams);
    HttpRequest->SetHeader(TEXT("Client-ID"), *ClientId);
    HttpRequest->SetHeader(TEXT("Authorization"), *AuthorizationHeader);
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetVerb(TEXT("POST"));

    TSharedPtr<FJsonObject> RequestPayload = MakeShareable(new FJsonObject);
    RequestPayload->SetStringField(TEXT("message"), TEXT("Hello, chat!"));
    RequestPayload->SetStringField(TEXT("color"), TEXT("orange"));

    FString JsonString;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(RequestPayload.ToSharedRef(), Writer);

    UE_LOG(LogTemp, Display, TEXT("Serialized JSON: %s"), *JsonString);

    HttpRequest->SetContentAsString(JsonString);

    HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess) {
        if (bSuccess && Response.IsValid())
        {
            int32 code = Response->GetResponseCode();
            FString message = Response->GetContentAsString();
            UE_LOG(LogTemp, Display, TEXT("HTTP Response code: %d"), code);
            UE_LOG(LogTemp, Display, TEXT("HTTP Response content: %s"), *message);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HTTP Request failed!"));
        }
        });

    UE_LOG(LogTemp, Display, TEXT("Send Request"));

    HttpRequest->ProcessRequest();
}