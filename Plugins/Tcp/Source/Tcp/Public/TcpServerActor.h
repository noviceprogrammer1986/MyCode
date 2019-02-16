// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "IImageWrapperModule.h"
#include "TcpServerActor.generated.h"


class FTcpReceiver;
class UTexture2D;
class UPhotoListWidget;

UCLASS()
class TCP_API ATcpServerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATcpServerActor();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Tcp")
		void StartTcpListener();
	UFUNCTION()
		void OnTextureReady(UTexture2D* InTexture);

	UFUNCTION()
		void OnTextureReadyFor(UTexture2D* InTexture, int32 CurrentPoint);

	UPROPERTY(EditAnywhere, Category = "Tcp")
		int32 Port;
	UPROPERTY(EditAnywhere, Category = "Save")
		FString SaveDirectory;
	UPROPERTY(EditAnywhere, Category = "Save")
		FString FileExtension;
	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UPhotoListWidget> PhotoListWidgetClass;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TSharedPtr<FTcpListener> TcpListener;
	TArray<TSharedPtr<FTcpReceiver>> TcpReceivers;
	bool OnConnectionAccepted(FSocket* Socket, const FIPv4Endpoint& Endpoint);

	UPROPERTY()
		TArray<UTexture2D*> Textures;
	UPROPERTY()
		UPhotoListWidget* PhotoListWidget;
};
