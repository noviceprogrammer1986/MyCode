// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "MyDragDropOperation.generated.h"


DECLARE_DELEGATE_TwoParams(FOnDragged, UDragDropOperation*, const FPointerEvent&);

UCLASS()
class V20_API UMyDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	void Dragged_Implementation(const FPointerEvent& PointerEvent);

	FOnDragged OnDragging;
};
