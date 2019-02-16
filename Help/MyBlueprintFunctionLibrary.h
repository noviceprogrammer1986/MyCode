// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HELP_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "CPP", meta=(AdvancedDisplay = "LineStepDistance, HorizontalStepAngle, VerticalStepsTotal"))
		static bool FindBestFocusLocation(AActor* TargetActor, FVector& Location, TArray<FVector> DesiredViewDirections,
			float MinDistance = 500.f, float BestDistance = 2000.f,
			float LineStepDistance = 200.f, float HorizontalStepAngle = 30.f, int VerticalStepsTotal = 2);

	UFUNCTION(BlueprintCallable, Category = "CPP")
		static void TakeScreenShot(FString Filename = "ScreenShot", bool bShowUI = true);

};
