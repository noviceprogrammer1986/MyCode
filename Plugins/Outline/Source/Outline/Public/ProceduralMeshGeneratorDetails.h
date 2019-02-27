#pragma once

#include "CoreMinimal.h"
#include "PropertyEditor/Public/IDetailCustomization.h"


class FProceduralMeshGeneratorDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	FReply ClickMe();

private:
	bool bIsConverting = false;
	TWeakObjectPtr<class AProceduralMeshGenerator> Generator;
};