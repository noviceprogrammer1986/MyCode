// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IHttpRequest.h"
#include "JsonActor.generated.h"


USTRUCT(BlueprintType)
struct FSaleOrder
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FString SaleTime;
	UPROPERTY(BlueprintReadOnly)
		FString Name;
	UPROPERTY(BlueprintReadOnly)
		FString OrderID;
	UPROPERTY(BlueprintReadOnly)
		FString SaleMoney;
	UPROPERTY(BlueprintReadOnly)
		FString PayType;
	UPROPERTY(BlueprintReadOnly)
		FString Number;
	UPROPERTY(BlueprintReadOnly)
		FString Area;
	UPROPERTY(BlueprintReadOnly)
		FString ServiceAreaName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuery, const TArray<FSaleOrder>&, Sales);

UCLASS()
class TCP_API AJsonActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJsonActor();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
		void BeginQuery();
	UFUNCTION(BlueprintCallable)
		void EndQuery();

	UFUNCTION()
		void JsonQuery();

	UPROPERTY(EditAnywhere)
		FString Url = "http://isapy.sdhssam.com/supermarket/index_new_getdata?said=133&jyms=&begin2end=%s%%20-%%20%s&page=1&limit=%d";
	UPROPERTY(EditAnywhere)
		int32 Limit = 10;
	UPROPERTY(EditAnywhere)
		float Delay = 10.f;
	UPROPERTY(BlueprintAssignable)
		FOnQuery OnQuery;

private:
	void OnQueryResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	TArray<FSaleOrder> Sales;
	FTimerHandle QueryHandle;
};
