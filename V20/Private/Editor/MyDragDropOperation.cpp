// Fill out your copyright notice in the Description page of Project Settings.

#include "MyDragDropOperation.h"


void UMyDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	OnDragging.ExecuteIfBound(this, PointerEvent);
}