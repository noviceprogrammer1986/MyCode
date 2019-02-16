// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshGenerator.generated.h"


USTRUCT()
struct FOutline
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		AActor* RefActor;
	UPROPERTY(EditAnywhere)
		FString RefCompName;
	UPROPERTY(EditAnywhere)
		FString ObjFilename;
	UPROPERTY(EditAnywhere)
		bool bDraw = true;
	UPROPERTY(EditAnywhere)
		float LineThickness = 1.f;
	UPROPERTY(EditAnywhere)
		UMaterialInterface* OutlineMaterial;

	UPROPERTY()
		class UProceduralMeshComponent* OutlineComp;
};

USTRUCT()
struct FLine
{
	GENERATED_BODY()

	UPROPERTY()
		FVector Start;

	UPROPERTY()
		FVector End;

	FLine()
		: Start(ForceInit)
		, End(ForceInit)
	{}
	FLine(const FVector& InStart, const FVector& InEnd)
		: Start(InStart)
		, End(InEnd)
	{}
};

UCLASS()
class OUTLINE_API AProceduralMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMeshGenerator();

	UFUNCTION()
		void Init();

	UFUNCTION(BlueprintCallable, Category = "Outline")
		void ShowOutline(bool bVisible = true, AActor* InActor = nullptr);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void AutoFill();
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Outline")
		FString ObjFilePath;

	UPROPERTY(EditAnywhere, Category = "Outline")
		UMaterialInterface* DefaultOutlineMaterial;

	UPROPERTY(EditAnywhere, Category = "Outline")
		TArray<FOutline> Outlines;

	UPROPERTY(EditAnywhere, Category = "Search")
		FString SearchName;
};