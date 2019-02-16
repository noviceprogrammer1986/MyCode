// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OutlineManager.generated.h"


USTRUCT()
struct FOutlineData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		AActor* RefActor;
	UPROPERTY(EditAnywhere)
		FString ObjFilename;
	UPROPERTY(EditAnywhere)
		bool bDraw = true;
	UPROPERTY(EditAnywhere)
		float LineThickness = 1.f;
	UPROPERTY(EditAnywhere)
		UMaterialInterface* OutlineMaterial;

	UPROPERTY()
		UOutlineComponent* OutlineComp;
};

UCLASS()
class OUTLINE_API AOutlineManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOutlineManager();

	UFUNCTION()
		void Init();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Outline")
		FString ObjFilePath;

	UPROPERTY(EditAnywhere, Category = "Outline")
		TArray<FOutlineData> Outlines;
};
