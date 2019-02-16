// Fill out your copyright notice in the Description page of Project Settings.

#include "TransformWidget.h"
#include "EditableTextBox.h"
#include "MyGameViewportClient.h"
#include "MyWidget.h"


void UTransformWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MyGameViewport = Cast<UMyGameViewportClient>(GetWorld()->GetGameViewport());
	if (!MyGameViewport)
	{
		UE_LOG(LogTemp, Error, TEXT("wrong gameviewportclient"));
		return;
	}
	MyGameViewport->OnActorSelected.AddLambda([&](AActor* SelectedActor)
	{
		if (SelectedActor)
		{
			FVector Location = SelectedActor->GetActorLocation();
			FRotator Rotation = SelectedActor->GetActorRotation();
			FVector Scale = SelectedActor->GetActorScale();
			LocationX->SetText(FText::FromString(FString::SanitizeFloat(Location.X)));
			LocationY->SetText(FText::FromString(FString::SanitizeFloat(Location.Y)));
			LocationZ->SetText(FText::FromString(FString::SanitizeFloat(Location.Z)));
			RotationX->SetText(FText::FromString(FString::SanitizeFloat(Rotation.Roll)));
			RotationY->SetText(FText::FromString(FString::SanitizeFloat(Rotation.Pitch)));
			RotationZ->SetText(FText::FromString(FString::SanitizeFloat(Rotation.Yaw)));
			ScaleX->SetText(FText::FromString(FString::SanitizeFloat(Scale.X)));
			ScaleY->SetText(FText::FromString(FString::SanitizeFloat(Scale.Y)));
			ScaleZ->SetText(FText::FromString(FString::SanitizeFloat(Scale.Z)));
		}
	});
	MyGameViewport->OnLocationChanged.AddLambda([&] 
	{
		FVector Location = MyGameViewport->SelectedActor->GetActorLocation();
		LocationX->SetText(FText::FromString(FString::SanitizeFloat(Location.X)));
		LocationY->SetText(FText::FromString(FString::SanitizeFloat(Location.Y)));
		LocationZ->SetText(FText::FromString(FString::SanitizeFloat(Location.Z)));
	});
	MyGameViewport->OnRotationChanged.AddLambda([&] 
	{
		FRotator Rotation = MyGameViewport->SelectedActor->GetActorRotation();
		RotationX->SetText(FText::FromString(FString::SanitizeFloat(Rotation.Roll)));
		RotationY->SetText(FText::FromString(FString::SanitizeFloat(Rotation.Pitch)));
		RotationZ->SetText(FText::FromString(FString::SanitizeFloat(Rotation.Yaw)));
	});
	MyGameViewport->OnScaleChanged.AddLambda([&] 
	{
		FVector Scale = MyGameViewport->SelectedActor->GetActorScale();
		ScaleX->SetText(FText::FromString(FString::SanitizeFloat(Scale.X)));
		ScaleY->SetText(FText::FromString(FString::SanitizeFloat(Scale.Y)));
		ScaleZ->SetText(FText::FromString(FString::SanitizeFloat(Scale.Z)));
	});
	LocationX->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnLocationXCommitted);
	LocationY->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnLocationYCommitted);
	LocationZ->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnLocationZCommitted);
	RotationX->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnRotationXCommitted);
	RotationY->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnRotationYCommitted);
	RotationZ->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnRotationZCommitted);
	ScaleX->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnScaleXCommitted);
	ScaleY->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnScaleYCommitted);
	ScaleZ->OnTextCommitted.AddDynamic(this, &UTransformWidget::OnScaleZCommitted);
}

void UTransformWidget::OnLocationXCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float X = FCString::Atof(*Text.ToString());
	LocationX->SetText(FText::FromString(FString::SanitizeFloat(X)));
	if (MyGameViewport && MyGameViewport->SelectedActor && MyGameViewport->MyWidget)
	{
		FVector Location = MyGameViewport->SelectedActor->GetActorLocation();
		Location = FVector(X, Location.Y, Location.Z);
		MyGameViewport->SelectedActor->SetActorLocation(Location);
		MyGameViewport->MyWidget->WidgetLocation = Location;
	}
}
void UTransformWidget::OnLocationYCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float Y = FCString::Atof(*Text.ToString());
	LocationY->SetText(FText::FromString(FString::SanitizeFloat(Y)));
	if (MyGameViewport && MyGameViewport->SelectedActor && MyGameViewport->MyWidget)
	{
		FVector Location = MyGameViewport->SelectedActor->GetActorLocation();
		Location = FVector(Location.X, Y, Location.Z);
		MyGameViewport->SelectedActor->SetActorLocation(Location);
		MyGameViewport->MyWidget->WidgetLocation = Location;
	}
}
void UTransformWidget::OnLocationZCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float Z = FCString::Atof(*Text.ToString());
	LocationZ->SetText(FText::FromString(FString::SanitizeFloat(Z)));
	if (MyGameViewport && MyGameViewport->SelectedActor && MyGameViewport->MyWidget)
	{
		FVector Location = MyGameViewport->SelectedActor->GetActorLocation();
		Location = FVector(Location.X, Location.Y, Z);
		MyGameViewport->SelectedActor->SetActorLocation(Location);
		MyGameViewport->MyWidget->WidgetLocation = Location;
	}
}
void UTransformWidget::OnRotationXCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float X = FCString::Atof(*Text.ToString());
	if ((X <= -360.f) || (X >= 360.f))
	{
		X = FRotator::ClampAxis(X);
	}
	RotationX->SetText(FText::FromString(FString::SanitizeFloat(X)));
	if (MyGameViewport && MyGameViewport->SelectedActor)
	{
		FRotator Rotation = MyGameViewport->SelectedActor->GetActorRotation();
		Rotation = FRotator(Rotation.Pitch, Rotation.Yaw, X);
		MyGameViewport->SelectedActor->SetActorRotation(Rotation);
	}
}
void UTransformWidget::OnRotationYCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float Y = FCString::Atof(*Text.ToString());
	if ((Y <= -360.f) || (Y >= 360.f))
	{
		Y = FRotator::ClampAxis(Y);
	}
	RotationY->SetText(FText::FromString(FString::SanitizeFloat(Y)));
	if (MyGameViewport && MyGameViewport->SelectedActor)
	{
		FRotator Rotation = MyGameViewport->SelectedActor->GetActorRotation();
		Rotation = FRotator(Y, Rotation.Yaw, Rotation.Roll);
		MyGameViewport->SelectedActor->SetActorRotation(Rotation);
	}
}
void UTransformWidget::OnRotationZCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float Z = FCString::Atof(*Text.ToString());
	if ((Z <= -360.f) || (Z >= 360.f))
	{
		Z = FRotator::ClampAxis(Z);
	}
	RotationZ->SetText(FText::FromString(FString::SanitizeFloat(Z)));
	if (MyGameViewport && MyGameViewport->SelectedActor)
	{
		FRotator Rotation = MyGameViewport->SelectedActor->GetActorRotation();
		Rotation = FRotator(Rotation.Pitch, Z, Rotation.Roll);
		MyGameViewport->SelectedActor->SetActorRotation(Rotation);
	}
}
void UTransformWidget::OnScaleXCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float X = FCString::Atof(*Text.ToString());
	ScaleX->SetText(FText::FromString(FString::SanitizeFloat(X)));
	if (MyGameViewport && MyGameViewport->SelectedActor)
	{
		FVector Scale = MyGameViewport->SelectedActor->GetActorScale();
		Scale = FVector(X, Scale.Y, Scale.Z);
		MyGameViewport->SelectedActor->SetActorScale3D(Scale);
	}
}
void UTransformWidget::OnScaleYCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float Y = FCString::Atof(*Text.ToString());
	ScaleY->SetText(FText::FromString(FString::SanitizeFloat(Y)));
	if (MyGameViewport && MyGameViewport->SelectedActor)
	{
		FVector Scale = MyGameViewport->SelectedActor->GetActorScale();
		Scale = FVector(Scale.X, Y, Scale.Z);
		MyGameViewport->SelectedActor->SetActorScale3D(Scale);
	}
}
void UTransformWidget::OnScaleZCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float Z = FCString::Atof(*Text.ToString());
	ScaleZ->SetText(FText::FromString(FString::SanitizeFloat(Z)));
	if (MyGameViewport && MyGameViewport->SelectedActor)
	{
		FVector Scale = MyGameViewport->SelectedActor->GetActorScale();
		Scale = FVector(Scale.X, Scale.Y, Z);
		MyGameViewport->SelectedActor->SetActorScale3D(Scale);
	}
}