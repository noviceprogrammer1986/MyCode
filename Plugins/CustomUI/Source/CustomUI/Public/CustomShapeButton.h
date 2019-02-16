#pragma once

#include "Button.h"
#include "SCustomShapeButton.h"
#include "CustomShapeButton.generated.h"


 /**
  * Classic UMG Button with Advanced Hit implementation.
  * Use SetAdvancedHitTexture to set unique button's geometry, determined by it's alpha channel.
  * Use SetAdvancedHitAlpha to set an integer in range 0-255. If pixel's alpha is lower than this value, it will be treated as an empty space.
  */
UCLASS()
class CUSTOMUI_API UCustomShapeButton : public UButton
{
	GENERATED_BODY()
public:

	/**
	 *	For this to work, the texture asset must have the following settings:
	 *	MipGenSettings: NoMipmaps
	 *	TextureGroup: UI
	 *	CompressionSettings: UserInterface2D(RGBA)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AdvancedHitTest")
		UTexture2D* AdvancedHitTexture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AdvancedHitTest", meta = (ClampMin = "0.0", ClampMax = "255.0", UIMin = "0.0", UIMax = "255.0"))
		int32 AdvancedHitAlpha;

	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest")
	void SetAdvancedHitTexture(UTexture2D* InTexture)
	{
		AdvancedHitTexture = InTexture;
		if (MyButton.IsValid()) (static_cast<SCustomShapeButton*>(MyButton.Get()))->SetAdvancedHitTexture(AdvancedHitTexture);
	}

	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest")
	void SetAdvancedHitAlpha(int32 InAlpha)
	{
		AdvancedHitAlpha = InAlpha;
		if (MyButton.IsValid()) (static_cast<SCustomShapeButton*>(MyButton.Get()))->SetAdvancedHitAlpha(AdvancedHitAlpha);
	}

	UCustomShapeButton() : AdvancedHitTexture(NULL), AdvancedHitAlpha(0) {}

	virtual void SynchronizeProperties() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
