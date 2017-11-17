// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DolphinResponseCallbackStructs.generated.h"

USTRUCT(BlueprintType)
struct DOLPHIN_API FDolphinResponseCallback
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dolphin", meta = (DisplayName = "Protocol"))
		uint8 SpecificProtocol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dolphin", meta = (DisplayName = "Callback"))
		FName CallbackName;
};

USTRUCT(BlueprintType)
struct DOLPHIN_API FDolphinResponseCallbackBinder
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dolphin", meta = (DisplayName = "Protocol Module"))
		uint8 ProtocolModule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dolphin", meta = (DisplayName = "Specific Callback"))
		TArray<FDolphinResponseCallback> SpecificCallback;
};