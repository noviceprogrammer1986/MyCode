// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWidget.h"
#include "BorderButton.h"
#include "MyDragDropOperation.h"
#include "ScrollBox.h"
#include "HorizontalBox.h"
#include "HorizontalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "WidgetLayoutLibrary.h"
#include "MyGameViewportClient.h"
#include "MyGameInstance.h"
#include "Image.h"
#include "TextBlock.h"
#include "VerticalBox.h"
#include "VerticalBoxSlot.h"


void UInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!MyScrollBox)
	{
		return;
	}
	MyScrollBox->ClearChildren();

	int32 Remains = Items.Num() % NumItemsPerRow;
	int32 NumRows = Remains == 0 ? Items.Num() / NumItemsPerRow : Items.Num() / NumItemsPerRow + 1;
	for (int32 i = 0; i < NumRows; ++i)
	{
		UHorizontalBox* NewHorizontalBox = NewObject<UHorizontalBox>(this);
		for (int32 j = 0; j < NumItemsPerRow; ++j)
		{
			int32 CurrentIndex = i * NumItemsPerRow + j;

			UBorderButton* NewBorderButton = NewObject<UBorderButton>(this);
			NewBorderButton->Init(this, CurrentIndex);
			UVerticalBox* NewVerticalBox = NewObject<UVerticalBox>(this);
			UVerticalBoxSlot* VSlot = NewVerticalBox->AddChildToVerticalBox(NewBorderButton);
			VSlot->SetHorizontalAlignment(HAlign_Center);
			VSlot->SetVerticalAlignment(VAlign_Center);

			UTextBlock* NewTextBlock = NewObject<UTextBlock>(this);
			FText NewText = FText::FromString("Hello");
			NewTextBlock->SetVisibility(ESlateVisibility::Hidden);
			if (CurrentIndex < Items.Num() && !Items[CurrentIndex].ItemText.IsEmpty())
			{
				NewText = Items[CurrentIndex].ItemText;
				NewTextBlock->SetVisibility(ESlateVisibility::Visible);
			}
			NewTextBlock->SetText(NewText);
			NewTextBlock->SetJustification(ETextJustify::Center);
			NewTextBlock->SetColorAndOpacity(FontColor);
			NewTextBlock->Font.Size = FontSize;
			NewVerticalBox->AddChildToVerticalBox(NewTextBlock)->SetPadding(TextPadding);

			UHorizontalBoxSlot* MySlot = NewHorizontalBox->AddChildToHorizontalBox(NewVerticalBox);
			MySlot->SetHorizontalAlignment(HAlign_Center);
			MySlot->SetVerticalAlignment(VAlign_Center);
			MySlot->SetSize(ESlateSizeRule::Fill);
			MySlot->SetPadding(IconPadding);
		}
		MyScrollBox->AddChild(NewHorizontalBox);
	}
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MyGameViewport = Cast<UMyGameViewportClient>(GetWorld()->GetGameViewport());
	if (!MyGameViewport)
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong GameViewportClient!"));
	}
	MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (!MyGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong GameInstance!"));
	}
}

void UInventoryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UMyDragDropOperation* MyOperation = NewObject<UMyDragDropOperation>(this);
	MyOperation->OnDragCancelled.AddDynamic(this, &UInventoryWidget::OnRelease);
	MyOperation->OnDragging.BindUObject(this, &UInventoryWidget::OnDragging);

	if (CurrentButton)
	{
		CurrentButton->SetBrushColor(FLinearColor::White);
		UImage* Icon = NewObject<UImage>(this);
		Icon->SetBrushFromTexture(Items[CurrentButton->Index].Icon);
		Icon->Brush.ImageSize = DragIconSize;
		MyOperation->DefaultDragVisual = Icon;
	}

	OutOperation = MyOperation;
}

void UInventoryWidget::OnRelease(UDragDropOperation* Operation)
{
	if (Operation->Payload)
	{
		AActor* ItemActor = Cast<AActor>(Operation->Payload);
		if (ItemActor)
		{
			if (ItemActor->bHidden)
				ItemActor->Destroy();
			else
			{
				if (MyGameViewport)
				{
					MyGameViewport->SetSelectedActor(ItemActor);
					MyGameViewport->EnterEditMode();
				}
				if (MyGameInstance)
				{
					MyGameInstance->SaveItems.Add(ItemActor, FSaveItem(ItemActor->GetClass(), ItemActor->GetActorTransform()));
				}
			}
		}
	}
	CurrentButton = nullptr;
}

void UInventoryWidget::OnDragging(UDragDropOperation* Operation, const FPointerEvent& PointerEvent)
{
	if (Operation->Payload)
	{
		AActor* ItemActor = Cast<AActor>(Operation->Payload);
		if (ItemActor && !ItemActor->bHidden)
		{
			FVector WorldOrigin;
			FVector WorldDirection;
			FVector2D MousePosition = (UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) * UWidgetLayoutLibrary::GetViewportScale(GetWorld())).IntPoint();

			if (UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), MousePosition, WorldOrigin, WorldDirection))
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, true, ItemActor);
				if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, WorldOrigin + WorldDirection * 100000.f, ECollisionChannel::ECC_Visibility, Params))
				{
					ItemActor->SetActorLocation(HitResult.Location);
				}
				else
				{
					ItemActor->SetActorLocation(WorldOrigin + WorldDirection * 800.f);
				}
			}
		}
	}
}

void UInventoryWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (InOperation->Payload)
	{
		AActor* ItemActor = Cast<AActor>(InOperation->Payload);
		if (ItemActor)
		{
			ItemActor->SetActorHiddenInGame(true);
		}
	}

	if (InOperation->DefaultDragVisual)
	{
		InOperation->DefaultDragVisual->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UInventoryWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (InOperation->DefaultDragVisual)
	{
		InOperation->DefaultDragVisual->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InOperation->Payload)
	{
		AActor* ItemActor = Cast<AActor>(InOperation->Payload);
		if (ItemActor)
		{
			ItemActor->SetActorHiddenInGame(false);
		}
	}
	else if (CurrentButton)
	{
		UClass* ActorClass = Items[CurrentButton->Index].ItemClass;
		if (ActorClass)
		{
			AActor* ItemActor = GetWorld()->SpawnActor(ActorClass);
			InOperation->Payload = ItemActor;
		}
	}
}

FReply UInventoryWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (CurrentButton)
	{
		CurrentButton->SetBrushColor(FLinearColor::White);
		CurrentButton = nullptr;
	}
	return FReply::Handled();
}