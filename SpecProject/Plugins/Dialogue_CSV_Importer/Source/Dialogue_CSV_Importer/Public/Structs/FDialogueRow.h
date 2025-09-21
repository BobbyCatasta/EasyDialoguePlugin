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

	// Id used for recognizing the unique dialogue
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Dialogue_CSV_DataTable")
	FString Scene = TEXT("");

	// May be removed later
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Dialogue_CSV_DataTable")
	FString Id = TEXT("");

	// Name of the Speaker of the dialogue
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Dialogue_CSV_DataTable")
	FString Speaker = TEXT("");

	// Text of the dialogue
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Dialogue_CSV_DataTable")
	FText Dialogue = FText::GetEmpty();

	// Speed at which the text is shown, in word per second
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Dialogue_CSV_DataTable")
	float TextSpeed = 0.1f;
};
