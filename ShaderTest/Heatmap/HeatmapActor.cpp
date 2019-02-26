// Fill out your copyright notice in the Description page of Project Settings.

#include "HeatmapActor.h"
#include "MyComputeShader.h"
#include "Engine/TextureRenderTarget2D.h"


// Sets default values
AHeatmapActor::AHeatmapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHeatmapActor::BeginPlay()
{
	Super::BeginPlay();

	FMyShaderParameters MyStruct;
	MyStruct.a = FMath::FRand();
	MyStruct.b = 1.0;
	//ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
	//	FComputeShaderRunner,
	//	AHeatmapActor*, MyHeatmapActor, this,
	//	UTextureRenderTarget2D*, MyRenderTarget, RenderTarget,
	//	{ 
	//		MyHeatmapActor->ExecuteComputerShader_RenderThread(MyRenderTarget); 
	//	}
	//);
	ENQUEUE_RENDER_COMMAND(MyCommand)(
		[this, MyStruct](FRHICommandListImmediate& RHICmdList)
	{
		ExecuteComputerShader_RenderThread(RenderTarget, MyStruct);
	}
		);
}

// Called every frame
void AHeatmapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHeatmapActor::ExecuteComputerShader_RenderThread(UTextureRenderTarget2D* RenderTarget, FMyShaderParameters MyShaderParameters)
{
	check(IsInRenderingThread());

	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
	TShaderMapRef<FMyComputeShader> MyComputeShader(GetGlobalShaderMap(ERHIFeatureLevel::SM5));
	RHICmdList.SetComputeShader(MyComputeShader->GetComputeShader());

	FRHIResourceCreateInfo CreateInfoSRV;
	FTexture2DRHIRef RHITextureSRV = RHICreateTexture2D(10000, 1, PF_G32R32F, 1, 1, TexCreate_ShaderResource, CreateInfoSRV);

	uint32 DestStride = 0;
	float* TextureDataPtr = (float*)RHICmdList.LockTexture2D(RHITextureSRV, 0, EResourceLockMode::RLM_WriteOnly, DestStride, false, false);
	for (int32 i = 0; i < 10000; ++i)
	{
		*TextureDataPtr = FMath::FRand();
		++TextureDataPtr;
		*TextureDataPtr = FMath::FRand();
		++TextureDataPtr;
	}
	RHICmdList.UnlockTexture2D(RHITextureSRV, 0, false, false);

	FShaderResourceViewRHIRef TextureSRV = RHICreateShaderResourceView(RHITextureSRV, 0);

	FRHIResourceCreateInfo CreateInfoUAV;
	FTexture2DRHIRef RHITextureUAV = RHICreateTexture2D(RenderTarget->SizeX, RenderTarget->SizeY, PF_R32_FLOAT, 1, 1, TexCreate_UAV, CreateInfoUAV);
	FUnorderedAccessViewRHIRef TextureUAV = RHICreateUnorderedAccessView(RHITextureUAV);

	FMyUniformBufferStruct MyUniformBufferStruct;
	MyUniformBufferStruct.a = MyShaderParameters.a;
	MyUniformBufferStruct.b = MyShaderParameters.b;

	MyComputeShader->SetParameters(RHICmdList, TextureUAV, TextureSRV, MyUniformBufferStruct);
	DispatchComputeShader(RHICmdList, *MyComputeShader, RenderTarget->SizeX / 32, RenderTarget->SizeY / 32, 1);
	MyComputeShader->UnbindBuffers(RHICmdList);

	RHICmdList.CopyToResolveTarget(RHITextureUAV, RenderTarget->GetRenderTargetResource()->TextureRHI, FResolveParams());
}