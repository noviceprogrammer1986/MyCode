// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonActor.h"
#include "Http.h"
#include "JsonUtilities.h"
#include "DateTime.h"
#include "TimerManager.h"


// Sets default values
AJsonActor::AJsonActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AJsonActor::BeginPlay()
{
	Super::BeginPlay();

	BeginQuery();
}

void AJsonActor::BeginQuery()
{
	GetWorldTimerManager().SetTimer(QueryHandle, this, &AJsonActor::JsonQuery, Delay, true, 0.f);
}

void AJsonActor::EndQuery()
{
	GetWorldTimerManager().ClearTimer(QueryHandle);
}

void AJsonActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndQuery();

	Super::EndPlay(EndPlayReason);
}

void AJsonActor::JsonQuery()
{
	FString Today = FDateTime::Today().ToIso8601().Left(10);
	FString RealUrl = FString::Printf(*Url, *Today, *Today, Limit);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetURL(RealUrl);
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnQueryResponseReceived);
	Request->ProcessRequest();
}

void AJsonActor::OnQueryResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Operation Query failed"));
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonData;
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonData) || !JsonData.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to parse json=[%s]"), *JsonString);
		return;
	}

	Sales.Empty();

	for (auto& ValueElem : JsonData->GetArrayField("data"))
	{
		TSharedPtr<FJsonObject> JsonObject = ValueElem->AsObject();
		FSaleOrder Order;
		Order.SaleTime = JsonObject->GetStringField("sale_time");
		Order.Name = JsonObject->GetStringField("name").LeftChop(1);
		Order.OrderID = JsonObject->GetStringField("orderid");
		Order.SaleMoney = JsonObject->GetStringField("sale_money");
		Order.PayType = JsonObject->GetStringField("paytype");
		Order.Number = JsonObject->GetStringField("number");
		Order.Area = JsonObject->GetStringField("area");
		Order.ServiceAreaName = JsonObject->GetStringField("service_area_name");
		Sales.Add(Order);
	}

	OnQuery.Broadcast(Sales);
}