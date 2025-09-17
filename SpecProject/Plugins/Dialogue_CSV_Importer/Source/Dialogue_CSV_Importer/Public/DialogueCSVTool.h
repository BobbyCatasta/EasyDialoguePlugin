// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "DialogueCSVTool.generated.h"


struct FDialogueRow;

UCLASS(BlueprintType,Blueprintable)
class DIALOGUE_CSV_IMPORTER_API UDialogueCSVTool : public UObject
{	
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueCSVTool();

private:

	//
	UFUNCTION(BlueprintCallable,CallInEditor)
	void DownloadCSVFromURL(const FString& URL);

	//
	void OnCSVDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//
	TArray<FDialogueRow> ParseCSV(const FString Content);
};
