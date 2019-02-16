
#include "SCustomShapeButton.h"
#include "Engine/Texture2D.h"


FReply SCustomShapeButton::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!AdvancedHitTexture) return SButton::OnMouseMove(MyGeometry, MouseEvent);

	bool bHitTexture = true;
	FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	if (!LocalPosition.ContainsNaN())
	{
		LocalPosition /= MyGeometry.GetLocalSize();
		if (LocalPosition.X < 0.f || LocalPosition.Y < 0.f || LocalPosition.X > 1.f || LocalPosition.Y > 1.f)
		{
			// This happens if we press the button but move outside of it with the cursor.
			return SButton::OnMouseMove(MyGeometry, MouseEvent);
		}

		int32 ImageWidth = AdvancedHitTexture->PlatformData->SizeX;
		LocalPosition.X *= ImageWidth;
		LocalPosition.Y *= AdvancedHitTexture->PlatformData->SizeY;

		int32 BufferPosition = (FMath::FloorToInt(LocalPosition.Y) * ImageWidth) + FMath::FloorToInt(LocalPosition.X);
		FColor* ImageData = static_cast<FColor*>((AdvancedHitTexture->PlatformData->Mips[0]).BulkData.Lock(LOCK_READ_ONLY));
		if (!ImageData)
		{
			bHitTexture = false;
			UE_LOG(LogTemp, Warning, TEXT("The AdvancesHitTexture may not be properly setup. See tooltip of AdvancedHitTexture. Texture in use: %s"), *AdvancedHitTexture->GetName());
		}
		else
		{
			if (ImageData[BufferPosition].A <= AdvancedHitAlpha)
			{
				bHitTexture = false;
			}
		}
		AdvancedHitTexture->PlatformData->Mips[0].BulkData.Unlock();

		if (bHitTexture != bIsHovered)
		{
			if (bHitTexture) SButton::OnMouseEnter(MyGeometry, MouseEvent);
			else SButton::OnMouseLeave(MouseEvent);
		}
	}
	return SButton::OnMouseMove(MyGeometry, MouseEvent);
}

FCursorReply SCustomShapeButton::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	if (!bIsHovered) return FCursorReply::Unhandled();
	TOptional<EMouseCursor::Type> TheCursor = Cursor.Get();
	return (TheCursor.IsSet()) ? FCursorReply::Cursor(TheCursor.GetValue()) : FCursorReply::Unhandled();
}