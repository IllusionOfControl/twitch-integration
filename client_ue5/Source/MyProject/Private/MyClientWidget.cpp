
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


FString HtmlOAuthSuccess = TEXT(R"(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <title>OAuth Callback</title>
    </head>
    <body>
        <script>
            if (window.location.hash) {
                const currentUrl = window.location.href;
                const newUrl = currentUrl.replace('#', '?');
                window.location.href = newUrl;
            } else { window.close(); }
        </script>
        Close this page and open the application.
    </body>
    </html>
)");


bool UMyClientWidget::Initialize()
{
    Super::Initialize();

    HelloButton->OnClicked.AddDynamic(this, &UMyClientWidget::HelloButtonClicked);
    LoginButton->OnClicked.AddDynamic(this, &UMyClientWidget::LoginButtonClicked);
    ChatConnectButton->OnClicked.AddDynamic(this, &UMyClientWidget::ChatConnectButtonClicked);

    ClientId = TEXT("xy4lgciskz02qdr32iey77rsaqk1u7");
    BaseWsServerUrl = TEXT("ws://127.0.0.1:8000/ws");

    return true;
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

void UMyClientWidget::ChatConnectButtonClicked()
{
    UE_LOG(LogTemp, Display, TEXT("Chat connect button is clicked!"));

    ChatConnectButton->SetIsEnabled(false);
    ConnectToChatBot();
}

void UMyClientWidget::HelloButtonClicked()
{
    UE_LOG(LogTemp, Display, TEXT("Hello button is clicked!"));

    if (BroadcasterId.IsEmpty()) {
        FetchTwitchUser();
    }

    SendHelloMessage();
}


void UMyClientWidget::OpenBrowserForAuthorization() 
{
    FString AuthorizationEndpoint = TEXT("http://127.0.0.1:5000/callback");
    //FString AuthorizationEndpoint = TEXT("https://id.twitch.tv/oauth2/authorize");
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

        UE_LOG(LogTemp, Display, TEXT("Retrived access token -> %s"), *AccessToken);

        UE_LOG(LogTemp, Error, TEXT("OAuth success"));

        LoadingThrobber->SetVisibility(ESlateVisibility::Hidden);
        LoginButton->SetIsEnabled(false);
        ChatConnectButton->SetIsEnabled(true);
        HelloButton->SetIsEnabled(true);

        TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(HtmlOAuthSuccess, TEXT("text/html"));
        OnComplete(MoveTemp(response));
    }
    else if (Request.QueryParams.Contains(TEXT("error")))
    {
        FString ErrorMessage = Request.QueryParams[TEXT("error_description")];
        UE_LOG(LogTemp, Error, TEXT("OAuth error: %s"), *ErrorMessage);

        TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("OAuthError"), TEXT("text/html"));
        OnComplete(MoveTemp(response));
    }
    else 
    {
        TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(HtmlOAuthSuccess, TEXT("text/html"));
        OnComplete(MoveTemp(response));
    }

    return true;
}


void UMyClientWidget::StartHttpServer() {
    FHttpServerModule& httpServerModule = FHttpServerModule::Get();
    TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(12345, true);

    bool isValid = (FHttpPath(TEXT("/callback"))).IsValidPath();

    if (httpRouter != nullptr)
    {
        httpRouter->BindRoute(FHttpPath(TEXT("/callback")), EHttpServerRequestVerbs::VERB_GET,
            [this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { 
                return HandleCallback(Request, OnComplete);
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


void UMyClientWidget::ConnectToChatBot() {
    //const FString ServerURL = FString::Printf(TEXT("%s?access_token=%s"), *BaseWsServerUrl, *AccessToken);
    const FString ServerURL = FString::Printf(TEXT("%s?access_token=%s"), *BaseWsServerUrl, *AccessToken);
    const FString ServerProtocol = TEXT("ws");

    Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);

    Socket->OnConnected().AddLambda([]() -> void {
        UE_LOG(LogTemp, Display, TEXT("Socket connected."));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "WebSocket connected");
        });

    Socket->OnMessage().AddLambda([&](const FString& Message) -> void {
        UE_LOG(LogTemp, Display, TEXT("WebSocket -> %s"), *Message);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("WebSocket retrive -> %s"), *Message));

        if (Message.Compare(TEXT("plus")) == 0) {
            Score = FMath::Min(++Score, 10);
        }

        if (Message.Compare(TEXT("minus")) == 0) {
            Score = FMath::Max(--Score, -10);
        }

        UpdateScore();
        });

    Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
        UE_LOG(LogTemp, Display, TEXT("Socket closed."));
        });

    Socket->OnConnectionError().AddLambda([](const FString& Error) -> void {
        UE_LOG(LogTemp, Error, TEXT("Socket error: %s"), *Error);
        if (!Error.IsEmpty())
            UE_LOG(LogTemp, Error, TEXT("Socket error: %s"), *Error);
        });

    UE_LOG(LogTemp, Display, TEXT("Socket connecting...."));
    UE_LOG(LogTemp, Display, TEXT("Socket addr -> %s"), *ServerURL);
    Socket->Connect();
}

void UMyClientWidget::UpdateScore() {
    FString MyString = FString::Printf(TEXT("%d"), Score);
    ScoreText->SetText(FText::FromString(MyString));
}


void UMyClientWidget::FetchTwitchUser()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    TSharedPtr<FJsonObject> JsonObject;
    
    UE_LOG(LogTemp, Display, TEXT("Preparing get user request"));
      
    FString Url = TEXT("https://api.twitch.tv/helix/users");

    HttpRequest->SetURL(Url);
    HttpRequest->SetHeader(TEXT("Client-ID"), TEXT("xy4lgciskz02qdr32iey77rsaqk1u7"));
    HttpRequest->SetHeader(TEXT("Authorization"), TEXT("Bearer "));
    HttpRequest->SetVerb(TEXT("Get"));

    HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess) {
        if (bSuccess && Response.IsValid())
        {
            FString ResponseData = Response->GetContentAsString();

            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseData);
            if (FJsonSerializer::Deserialize(Reader, JsonObject))
            {
                if (JsonObject->TryGetStringField(TEXT("broadcaster_id"), BroadcasterId))
                {
                    UE_LOG(LogTemp, Error, TEXT("HTTP Request Twitch::GetUsers successed!"));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HTTP Request failed!"));
            BroadcasterId = FString();
            UE_LOG(LogTemp, Error, TEXT("HTTP Request Twitch::GetUsers failed!"));
        }
        });

    UE_LOG(LogTemp, Display, TEXT("Send Request"));

    HttpRequest->ProcessRequest();

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Request to Twitch::GetUsers");
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
    HttpRequest->SetVerb(TEXT("Post"));

    TSharedPtr<FJsonObject> RequestPayload = MakeShareable(new FJsonObject);
    RequestPayload->SetStringField(TEXT("message"), TEXT("Hello, chat!"));
    RequestPayload->SetStringField(TEXT("color"), TEXT("orange"));

    FString JsonString;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(RequestPayload.ToSharedRef(), Writer);

    HttpRequest->SetContentAsString(JsonString);

    HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess) {
        if (bSuccess && Response.IsValid())
        {
            int32 code = Response->GetResponseCode();
            FString message = Response->GetContentAsString();
            UE_LOG(LogTemp, Error, TEXT("HTTP Request Twitch::SendChatAnnouncement successed!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HTTP Request Twitch::SendChatAnnouncement failed!"));
        }
        });

    UE_LOG(LogTemp, Display, TEXT("Send Request"));

    HttpRequest->ProcessRequest();
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Request to Twitch::SendChatAnnouncement");
}