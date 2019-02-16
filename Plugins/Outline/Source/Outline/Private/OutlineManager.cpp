// Fill out your copyright notice in the Description page of Project Settings.

#include "OutlineManager.h"
#include "FileManager.h"
#include "OutlineComponent.h"

#include <fstream>
#include <sstream>
#include <string>

// Sets default values
AOutlineManager::AOutlineManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
}

// Called when the game starts or when spawned
void AOutlineManager::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}


void AOutlineManager::Init()
{
	TArray<FString> ObjFiles;
	IFileManager::Get().FindFiles(ObjFiles, *ObjFilePath, TEXT("obj"));
	if (ObjFiles.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No obj files found!"));
		return;
	}

	for (int32 i = 0; i < Outlines.Num(); ++i)
	{
		if (Outlines[i].RefActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Outlines[%d] has no RefActor!"), i);
			continue;
		}
		if (Outlines[i].OutlineMaterial == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Outlines[%d] has no Material!"), i);
			continue;
		}
		if (Outlines[i].ObjFilename.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Outlines[%d] has no ObjFilename!"), i);
			continue;
		}

		FString RealObjName;
		for (const FString& ObjName : ObjFiles)
		{
			if (ObjName == Outlines[i].ObjFilename || ObjName == Outlines[i].ObjFilename + ".obj")
			{
				RealObjName = ObjName;
				break;
			}
		}
		if (RealObjName.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Outlines[%d] has wrong ObjFilename!"), i);
			continue;
		}


		TArray<FVector> VertexArray;
		TArray<FVector4> IndexArray;

		std::ifstream InFile(TCHAR_TO_ANSI(*(ObjFilePath + "/" + RealObjName)));

		for (std::string Line; std::getline(InFile, Line); )
		{
			std::istringstream LineStream(Line);
			std::string VF;
			LineStream >> VF;
			if (VF == "v")
			{
				float x, y, z;
				LineStream >> x >> y >> z;
				VertexArray.Add(Outlines[i].RefActor->GetActorTransform().TransformPosition(FVector(x, z, y)));
			}
			else if (VF == "f")
			{
				int32 x, y, z, w = 0;
				LineStream >> x >> y >> z >> w;
				IndexArray.Add(FVector4(x - 1, y - 1, z - 1, w - 1));
			}
		}

		UOutlineComponent* OutlineComp = NewObject<UOutlineComponent>(this);
		OutlineComp->bDraw = Outlines[i].bDraw;
		OutlineComp->LineThickness = Outlines[i].LineThickness;
		OutlineComp->LineMaterial = Outlines[i].OutlineMaterial;
		OutlineComp->RegisterComponent();
		Outlines[i].OutlineComp = OutlineComp;

		for (const auto& elem : IndexArray)
		{
			OutlineComp->Lines.Add(FSimpleLine(VertexArray[int32(elem.X)], VertexArray[int32(elem.Y)]));
			OutlineComp->Lines.Add(FSimpleLine(VertexArray[int32(elem.Y)], VertexArray[int32(elem.Z)]));

			if (elem.W >= 0)
			{
				OutlineComp->Lines.Add(FSimpleLine(VertexArray[int32(elem.Z)], VertexArray[int32(elem.W)]));
				OutlineComp->Lines.Add(FSimpleLine(VertexArray[int32(elem.W)], VertexArray[int32(elem.X)]));
			}
			else
			{
				OutlineComp->Lines.Add(FSimpleLine(VertexArray[int32(elem.Z)], VertexArray[int32(elem.X)]));
			}
		}

		OutlineComp->MarkRenderStateDirty();
	}
}