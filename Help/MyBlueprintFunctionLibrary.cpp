// Fill out your copyright notice in the Description page of Project Settings.

#include "MyBlueprintFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UnrealClient.h"


bool UMyBlueprintFunctionLibrary::FindBestFocusLocation(AActor* TargetActor, FVector& Location, TArray<FVector> DesiredViewDirections, 
		float MinDistance, float BestDistance, float LineStepDistance, float HorizontalStepAngle, int VerticalStepsTotal)
{
	if (!TargetActor)
	{
		Location = UGameplayStatics::GetPlayerCameraManager(TargetActor->GetWorld(), 0)->GetCameraLocation();
		return false;
	}

	if(DesiredViewDirections.Num() == 0)
		DesiredViewDirections = { FVector(1.0, 1.0, 1.0), FVector(1.0, -1.0, 1.0), FVector(-1.0, 1.0, 1.0), FVector(-1.0, -1.0, 1.0) };
	VerticalStepsTotal = FMath::Clamp(VerticalStepsTotal, 1, 10);

	FVector TargetOrigin;
	FVector BoxExtent;
	TargetActor->GetActorBounds(false, TargetOrigin, BoxExtent);

	FVector OriginToCamera = UGameplayStatics::GetPlayerCameraManager(TargetActor->GetWorld(), 0)->GetCameraLocation() - TargetOrigin;

	FVector InitViewDirection;

	float MaxDot = 0.0;
	for (FVector v : DesiredViewDirections)
	{
		float NewDot = FVector::DotProduct(OriginToCamera, v);
		if (NewDot > MaxDot)
		{
			MaxDot = NewDot;
			InitViewDirection = v;
		}
	}
	InitViewDirection.Normalize();
	
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, TargetActor);

	for (int i = 0; i < VerticalStepsTotal; ++i)
	{
		FVector ViewDirection = FMath::Lerp(InitViewDirection, FVector(0, 0, 1), float(i) / float(VerticalStepsTotal));

		for (float Angle = 0.f; Angle <= 180.f; Angle += HorizontalStepAngle)
		{
			FVector NewViewDirection = ViewDirection.RotateAngleAxis(Angle, FVector(0.0, 0.0, 1.0));

			for (float Distance = BestDistance; Distance >= MinDistance; Distance -= LineStepDistance)
			{
				FVector TraceEnd = TargetOrigin + NewViewDirection * (BoxExtent.Size() + Distance);
				bool bBlocked = TargetActor->GetWorld()->LineTraceSingleByChannel(HitResult, TargetOrigin, TraceEnd, ECollisionChannel::ECC_Visibility, Params);

				if (!bBlocked)
				{
					Location = TraceEnd;
					return true;
				}
			}
			if (Angle > 0.f && Angle < 180.f)
			{
				NewViewDirection = ViewDirection.RotateAngleAxis(Angle * (-1.0), FVector(0.0, 0.0, 1.0));

				for (float Distance = BestDistance; Distance >= MinDistance; Distance -= LineStepDistance)
				{
					FVector TraceEnd = TargetOrigin + NewViewDirection * (BoxExtent.Size() + Distance);
					bool bBlocked = TargetActor->GetWorld()->LineTraceSingleByChannel(HitResult, TargetOrigin, TraceEnd, ECollisionChannel::ECC_Visibility, Params);

					if (!bBlocked)
					{
						Location = TraceEnd;
						return true;
					}
				}
			}
		}
	}

	FVector ViewDirection(0.f, 0.f, 1.f);

	for (float Distance = BestDistance; Distance >= MinDistance; Distance -= LineStepDistance)
	{
		FVector TraceEnd = TargetOrigin + ViewDirection * (BoxExtent.Size() + Distance);
		bool bBlocked = TargetActor->GetWorld()->LineTraceSingleByChannel(HitResult, TargetOrigin, TraceEnd, ECollisionChannel::ECC_Visibility, Params);

		if (!bBlocked)
		{
			Location = TraceEnd;
			return true;
		}
	}

	Location = UGameplayStatics::GetPlayerCameraManager(TargetActor->GetWorld(), 0)->GetCameraLocation();
	return false;
}

void UMyBlueprintFunctionLibrary::TakeScreenShot(FString Filename, bool bShowUI)
{
	FScreenshotRequest::RequestScreenshot(Filename, bShowUI, true);
}