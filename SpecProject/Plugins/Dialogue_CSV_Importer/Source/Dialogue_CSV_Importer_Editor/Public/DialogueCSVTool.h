// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "DialogueCSVTool.generated.h"


struct FDialogueRow;

UCLASS(BlueprintType,Blueprintable)
class DIALOGUE_CSV_IMPORTER_EDITOR_API UDialogueCSVTool : public UObject
{	
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueCSVTool();

	virtual void PostInitProperties() override;

private:

	//
	FString DataTableName;

	//
	FString PackagePath;

#if WITH_EDITOR
	//
	UFUNCTION(BlueprintCallable,CallInEditor, Category = "DataTable Creation")
	void DownloadCSVAndCreateDataTable(const FString& DataTablePath, const FString& AssetName, const FString& URL);

	//
	void OnCSVDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//
	TArray<FDialogueRow> ParseCSV(const FString& Content);

	//
	TArray<FString> ParseCSVLine(const FString& Line);

	//
	void CreateDialogueDataTableAsset(UDataTable*& OutTable,TArray<FDialogueRow> DialogueRows);
#endif
};
