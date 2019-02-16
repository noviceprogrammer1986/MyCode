
#include "CustomShapeButton.h"
#include "ButtonSlot.h"


void UCustomShapeButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	(static_cast<SCustomShapeButton*>(MyButton.Get()))->SetAdvancedHitTexture(AdvancedHitTexture);
	(static_cast<SCustomShapeButton*>(MyButton.Get()))->SetAdvancedHitAlpha(AdvancedHitAlpha);
}

TSharedRef<SWidget> UCustomShapeButton::RebuildWidget()
{
	TSharedPtr<SCustomShapeButton> CSButton = SNew(SCustomShapeButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.IsFocusable(IsFocusable)
		;

	CSButton->SetAdvancedHitTexture(AdvancedHitTexture);
	CSButton->SetAdvancedHitAlpha(AdvancedHitAlpha);

	MyButton = CSButton;

	if (GetChildrenCount() > 0) Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());

	return MyButton.ToSharedRef();
}