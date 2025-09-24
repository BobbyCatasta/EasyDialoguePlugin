// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "DialogueCSVTool.generated.h"


struct FDialogueRow;


UENUM(BlueprintType)
enum class EDialogueLogType : uint8
{
	LOG			UMETA(DisplayName="LOG"),
	DONE		UMETA(DisplayName="DONE"),
	ERROR		UMETA(DisplayName="ERROR"),
	WARNING		UMETA(DisplayName="WARNING"),
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDialogueLog,FString,MessageLog,EDialogueLogType,DialogueLogType);

UCLASS(BlueprintType,Blueprintable)
class DIALOGUE_CSV_IMPORTER_EDITOR_API UDialogueCSVTool : public UObject
{	
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueCSVTool();

	UPROPERTY(BlueprintAssignable,Category="Dialogue")
	FDialogueLog DialogueLogging;

private:

	// Name of the DataTable asset to be created
	FString DataTableName;

	// Target folder path (inside the Unreal project) where the DataTable will be stored
	FString PackagePath;

#if WITH_EDITOR
	// Downloads a CSV from a given URL and creates a DataTable asset
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "DataTable Creation")
	void DownloadCSVAndCreateDataTable(const FString& DataTablePath, const FString& AssetName, const FString& URL);

	// Callback triggered once the CSV file has been downloaded
	void OnCSVDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Parses the entire CSV file content into an array of dialogue rows
	TArray<FDialogueRow> ParseCSV(const FString& Content);

	// Parses a single CSV line into an array of string cells
	TArray<FString> ParseCSVLine(const FString& Line);

	// Creates a DataTable asset in the Content Browser from parsed dialogue rows
	void CreateDialogueDataTableAsset(UDataTable*& OutTable, TArray<FDialogueRow> DialogueRows);
#endif
};
