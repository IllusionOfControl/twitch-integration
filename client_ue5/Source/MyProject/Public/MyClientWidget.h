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
	bool HandleCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	int32 GetFreePort();

private:
	TSharedPtr<IWebSocket> Socket;
	FString ClientId;
	FString BaseWsServerUrl;
	bool _isServerStarted = false;

public:
	GENERATED_BODY()
		virtual bool Initialize();


	UPROPERTY(meta = (BindWidget))
		class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* HelloButton;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
		class UCircularThrobber* LoadingThrobber;

	UPROPERTY()
		int32 Score;

	UPROPERTY()
		FString BroadcasterId;

	UPROPERTY()
		FString AccessToken;


	UFUNCTION()
		void HelloButtonClicked();

	UFUNCTION()
		void LoginButtonClicked();

	UFUNCTION()
		void UpdateScore();

	UFUNCTION()
		void SendHelloMessage();

	UFUNCTION()
		void GetTwitchUser();

	UFUNCTION()
		void ConnectWebSocket();

	UFUNCTION()
		void StartHttpServer();

	UFUNCTION()
		void StopHttpServer();
};
