// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDialogueRow.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType,Blueprintable)
struct DIALOGUE_CSV_IMPORTER_API FDialogueRow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString Scene;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString Id;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString Speaker;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FText Dialogue;
};
