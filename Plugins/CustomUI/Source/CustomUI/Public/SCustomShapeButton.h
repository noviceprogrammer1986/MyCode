#pragma once

#include "SButton.h"
/**
 * Classic Slate Button with Advanced Hit implementation.
 * Use SetAdvancedHitTexture to set unique button's geometry, determined by it's alpha channel.
 * Use SetAdvancedHitAlpha to set an integer in range 0-255. If pixel's alpha is lower than this value, it will be treated as an empty space.
 */
class CUSTOMUI_API SCustomShapeButton : public SButton
{
public:
	/**
	 *	For this to work, the texture asset must have the following settings:
	 *	MipGenSettings: NoMipmaps
	 *	TextureGroup: UI
	 *	CompressionSettings: UserInterface2D(RGBA)
	 */
	UTexture2D* AdvancedHitTexture;
	int32 AdvancedHitAlpha;

	SCustomShapeButton() : AdvancedHitTexture(NULL), AdvancedHitAlpha(0) {}

	void SetAdvancedHitTexture(UTexture2D* InTexture)
	{
		AdvancedHitTexture = InTexture;
	}
	void SetAdvancedHitAlpha(int32 InAlpha)
	{
		AdvancedHitAlpha = InAlpha;
	}

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		if (!bIsHovered) return FReply::Unhandled();
		return SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
	}

	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override
	{
		if (!bIsHovered) return FReply::Unhandled();
		return SButton::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	}

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		return SButton::OnMouseButtonUp(MyGeometry, MouseEvent);
	}

	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		if (AdvancedHitTexture) return;
		return SButton::OnMouseEnter(MyGeometry, MouseEvent);
	}

	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
	{
		return SButton::OnMouseLeave(MouseEvent);
	}

	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual TSharedPtr<IToolTip> GetToolTip() override
	{
		return (bIsHovered ? SWidget::GetToolTip() : NULL);
	}
};
