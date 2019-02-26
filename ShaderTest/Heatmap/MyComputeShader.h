#pragma once

#include "GlobalShader.h"
#include "ShaderParameterUtils.h"


BEGIN_UNIFORM_BUFFER_STRUCT(FMyUniformBufferStruct, )
UNIFORM_MEMBER(float, a)
UNIFORM_MEMBER(float, b)
END_UNIFORM_BUFFER_STRUCT(FMyUniformBufferStruct)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FMyUniformBufferStruct, TEXT("MyStruct"))

class FMyComputeShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FMyComputeShader, Global)

public:
	FMyComputeShader(){}

	FMyComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		:FGlobalShader(Initializer)
	{
		OutputSurface.Bind(Initializer.ParameterMap, TEXT("OutputSurface"));
		TextureParameter.Bind(Initializer.ParameterMap, TEXT("TextureParameter"));
	}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	}

	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);
		Ar << OutputSurface;
		Ar << TextureParameter;
		return bShaderHasOutdatedParams;
	}

	void SetParameters(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef OutputSurfaceUAV, FShaderResourceViewRHIRef TextureParameterSRV, FMyUniformBufferStruct& MyUniformBufferStruct)
	{
		FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
		if (OutputSurface.IsBound())
		{
			RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputSurface.GetBaseIndex(), OutputSurfaceUAV);
		}
		if (TextureParameter.IsBound())
		{
			RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, TextureParameter.GetBaseIndex(), TextureParameterSRV);
		}
		SetUniformBufferParameterImmediate(RHICmdList, ComputeShaderRHI, GetUniformBufferParameter<FMyUniformBufferStruct>(), MyUniformBufferStruct);
	}

	void UnbindBuffers(FRHICommandList& RHICmdList)
	{
		FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
		if (OutputSurface.IsBound())
		{
			RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputSurface.GetBaseIndex(), FUnorderedAccessViewRHIParamRef());
		}
		if (TextureParameter.IsBound())
		{
			RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, TextureParameter.GetBaseIndex(), FShaderResourceViewRHIParamRef());
		}
	}

private:
	FShaderResourceParameter OutputSurface;
	FShaderResourceParameter TextureParameter;
};

IMPLEMENT_SHADER_TYPE(, FMyComputeShader, TEXT("/Project/MyShader.usf"), TEXT("MainCS"), SF_Compute);