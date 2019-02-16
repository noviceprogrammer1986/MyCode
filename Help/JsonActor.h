// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IHttpRequest.h"
#include "JsonActor.generated.h"


class AMyActor;
class FSocket;
class FInternetAddr;

USTRUCT(BlueprintType)
struct FSubStepInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubStepStruct")
		FString SubStepDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubStepStruct")
		FString FileName;
};

USTRUCT(BlueprintType)
struct FStepInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StepStruct")
		FString StepName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StepStruct")
		FString StepDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StepStruct")
		TArray<FSubStepInfo> SubSteps;
};

USTRUCT(BlueprintType)
struct FPlan
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StepStruct")
		TArray<FStepInfo> StepInfos;
};


UCLASS()
class RJY_API AJsonActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJsonActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
		void JsonCreate(AMyActor* Actor);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "JsonDelete"))
		void JsonTryDelete(AMyActor* Actor);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "JsonUpdate"))
		void JsonPreUpdate(AMyActor* Actor);

	UFUNCTION(BlueprintCallable)
		void JsonQuery(AMyActor* Actor);

	UFUNCTION(BlueprintCallable)
		void LoadMyActorsByBuildingCode(TSubclassOf<AMyActor> ActorClass, const FString& BuildingCode);

	UPROPERTY()
		FString BaseURL = "http://xiaofang.******.com:8888/api/";

	UPROPERTY()
		FString PlanURL = "http://xiaofang.******.com:8888/api/download_plan_basic";

	UPROPERTY()
		FString Cookie = "test_token=MWRkYzAzNGQtODFjNi00ZTRhLWJjMjgtNTNkNmVmNTlmZmY1.01f960cda97b9f46b48302a55506d0ab332a5f55";

	void TCPConnect(const FString& SocketName = "SenderSocket", const FString& IP = "127.0.0.1", const int32 Port = 3000);

	void ShowUI(FVector2D Position, FVector2D Size, const FString& URL_UI);

	UFUNCTION(BlueprintCallable)
		void HideUI();

	UFUNCTION(BlueprintCallable)
		void UploadFiles();

	UFUNCTION(BlueprintCallable)
		void TakeScreenShot(FString Filename = "ScreenShot", bool bShowUI = true);

	UFUNCTION(BlueprintCallable)
		void TestCase();

private:

	TMap<FHttpRequestPtr, AMyActor*> RAMap;
	TMap<FHttpRequestPtr, TSubclassOf<AMyActor>> RCMap;
	FTimerHandle Handle_ClearAll, Handle_CheckCode;

	FSocket* SenderSocket = nullptr;
	TSharedPtr<FInternetAddr> IPAddr;
	FVector2D DesktopResolution;

	void JsonUpdate(const FString& URL, const FString& UpdatedContent);
	void OnUpdateResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	void OnCreateResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	void JsonDelete(AMyActor* Actor);
	void OnDeleteResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	void OnPreUpdateResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	void OnQueryResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	void OnLoadResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	void OnUploadResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

	void CheckActorCode(AMyActor* Actor);

public:

	FSubStepInfo MakeSubStep(FString SubStepDescription, FString ScreenShotFileName);

	FStepInfo MakeStep(FString StepName, FString StepDescription, TArray<FSubStepInfo>& InSubSteps);

	UFUNCTION(BlueprintCallable, Category = "JsonActor")
		int32 AddStep(FString StepName, FString StepDescription);

	UFUNCTION(BlueprintCallable, Category = "JsonActor")
		void AddSubStep(FString InStepName, FString InSubStepText, FString InFileName);

	UFUNCTION(BlueprintCallable, Category = "JsonActor")
		void ClearPlan();

protected:

	FPlan PlanStruct;
};
