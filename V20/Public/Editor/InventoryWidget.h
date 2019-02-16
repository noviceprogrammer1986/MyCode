// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"


class UTexture2D;
class UScrollBox;
class UBorderButton;
class UMyGameViewportClient;
class UMyGameInstance;

USTRUCT(BlueprintType)
struct FItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ItemClass;
	UPROPERTY(EditAnywhere)
		FText ItemText;
};

UCLASS()
class V20_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	UPROPERTY(meta = (BindWidget))
		UScrollBox* MyScrollBox;

	UPROPERTY(EditAnywhere)
		TArray<FItem> Items;
	UPROPERTY(EditAnywhere)
		int32 NumItemsPerRow = 1;
	UPROPERTY(EditAnywhere)
		FVector2D IconSize = FVector2D(128, 128);
	UPROPERTY(EditAnywhere)
		FVector2D DragIconSize = FVector2D(128, 128);
	UPROPERTY(EditAnywhere)
		FMargin IconPadding = FMargin(10);
	UPROPERTY(EditAnywhere)
		FMargin TextPadding = FMargin(0, 10, 0, 0);
	UPROPERTY(EditAnywhere)
		int32 FontSize = 24;
	UPROPERTY(EditAnywhere)
		FSlateColor FontColor = FSlateColor(FLinearColor(1.0, 1.0, 1.0, 1.0));

	UPROPERTY()
		UBorderButton* CurrentButton;

public:
	UFUNCTION()
		void OnRelease(UDragDropOperation* Operation);

	UFUNCTION()
		void OnDragging(UDragDropOperation* Operation, const FPointerEvent& PointerEvent);

private:
	UPROPERTY()
		UMyGameViewportClient* MyGameViewport;
	UPROPERTY()
		UMyGameInstance* MyGameInstance;
};
