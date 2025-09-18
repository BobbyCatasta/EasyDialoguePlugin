// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FDialogueRow.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType,Blueprintable)
struct DIALOGUE_CSV_IMPORTER_API FDialogueRow : public FTableRowBase
{
	GENERATED_BODY()

	//
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString Scene;

	//
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString Id;

	//
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString Speaker;

	//
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FText Dialogue;

	// in word per second
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float TextSpeed;
};
