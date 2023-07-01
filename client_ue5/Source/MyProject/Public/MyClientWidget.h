#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "MyClientWidget.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API UMyClientWidget : public UUserWidget
{
private:
	void OpenBrowserForAuthorization();
	void UpdateScore();
	bool HandleCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	void ConnectToChatBot();
	void StartHttpServer();
	void StopHttpServer();

	void GetTwitchUser();
	void SendHelloMessage();
	
private:
	TSharedPtr<IWebSocket> Socket;
	FString ClientId;
	FString BaseWsServerUrl;
	bool _isServerStarted = false;
	FString BroadcasterId;
	FString AccessToken;

public:
	GENERATED_BODY()
		virtual bool Initialize();


	UPROPERTY(meta = (BindWidget))
		class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* ChatConnectButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* HelloButton;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
		class UCircularThrobber* LoadingThrobber;

	UPROPERTY(EditAnywhere)
		int32 Score;


	UFUNCTION()
		void HelloButtonClicked();

	UFUNCTION()
		void ChatConnectButtonClicked();

	UFUNCTION()
		void LoginButtonClicked();
};
