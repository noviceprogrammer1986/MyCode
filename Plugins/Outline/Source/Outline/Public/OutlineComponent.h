// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "OutlineComponent.generated.h"

/**
 * 
 */

USTRUCT()
struct FSimpleLine
{
	GENERATED_BODY()

	UPROPERTY()
		FVector Start;

	UPROPERTY()
		FVector End;

	FSimpleLine()
		: Start(ForceInit)
		, End(ForceInit)
	{}
	FSimpleLine(const FVector& InStart, const FVector& InEnd)
		: Start(InStart)
		, End(InEnd)
	{}
};

UCLASS()
class OUTLINE_API UOutlineComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
	
public:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Draw")
		bool bDraw = true;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Draw")
	//	FLinearColor LineColor = FLinearColor(0.f, 1.f, 0.f);

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Draw")
	//	bool bLineScreenSpace = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Draw")
		float LineThickness = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Draw")
		UMaterialInterface* LineMaterial;

	UPROPERTY()
		TArray<FSimpleLine> Lines;
};
