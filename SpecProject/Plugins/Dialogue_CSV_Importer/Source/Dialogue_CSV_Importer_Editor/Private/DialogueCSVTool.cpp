// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueCSVTool.h"
#include "HttpModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataTable.h"
#include "Interfaces/IHttpResponse.h"
#include "Structs/FDialogueRow.h"
#include "EditorAssetLibrary.h"
#include "UObject/SavePackage.h"


// Sets default values for this component's properties
UDialogueCSVTool::UDialogueCSVTool() {}

void UDialogueCSVTool::PostInitProperties()
{
	UObject::PostInitProperties();

	// Set the Data Table to create FDialogueRow Tables
	// DataTableFactory = NewObject<UDataTableFactory>();
	// DataTableFactory->Struct = FDialogueRow::StaticStruct();
}

void UDialogueCSVTool::DownloadCSVAndCreateDataTable(const FString& DataTablePath, const FString& AssetName, const FString& URL)
{
	PackagePath = DataTablePath;
	DataTableName = AssetName;
	
	FHttpModule* Http = &FHttpModule::Get();

	// HTTP Request
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb("GET");

	// Add Callback at the end of the request
	Request->OnProcessRequestComplete().BindUObject(this, &UDialogueCSVTool::OnCSVDownloaded);

	// Send Request
	Request->ProcessRequest();
}

void UDialogueCSVTool::OnCSVDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR : Could not download CSV."));
		return;
	}

	FString CSVContent = UTF8_TO_TCHAR(Response->GetContent().GetData());

	// You can use "END TABLE" in the CSV for separating the Data Table part for design text and other useful text
	int32 FoundIndex = CSVContent.Find(TEXT("END TABLE"), ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (FoundIndex != INDEX_NONE)
		CSVContent = CSVContent.Left(FoundIndex);
	
	UE_LOG(LogTemp, Log, TEXT("CSV downloaded correctly. Now Creating Data Table..."));

	TArray<FDialogueRow> DialogueRows = ParseCSV(CSVContent);
	UDataTable* DataTable;
	
	// This may change, create a Table Named based on GID ,not based on AssetName
	CreateDialogueDataTableAsset(DataTable, DialogueRows);
	
}

TArray<FDialogueRow> UDialogueCSVTool::ParseCSV(const FString& Content)
{
	TArray<FDialogueRow> ParsedRows;
	TArray<FString> Lines;
	
	Content.ParseIntoArrayLines(Lines);
	if (Lines.Num() <= 1)
		return ParsedRows;
	
	// 
	for (int32 i = 1; i < Lines.Num(); i++)
	{
		FString Line = Lines[i];

		// Cell splitting
		TArray<FString> Cells = ParseCSVLine(Lines[i]);

		// Fill Dialogue Row
		FDialogueRow DialogueRow;
		
		DialogueRow.Scene = FName(Cells[0]);
		DialogueRow.Speaker = Cells[1];
		DialogueRow.Dialogue = FText::FromString(Cells[2]);
		DialogueRow.TextSpeed = FCString::Atof(*Cells[3]);
		DialogueRow.NextScene = FName(Cells[4]);
		
		ParsedRows.Add(DialogueRow);
	}

	return ParsedRows;
}

TArray<FString> UDialogueCSVTool::ParseCSVLine(const FString& Line)
{
	TArray<FString> Cells;
	FString Current;
	bool bInQuotes = false;
	
	for (int32 i = 0; i < Line.Len(); i++)
	{
		TCHAR Char = Line[i];

		if (Char == '\"')
		{
			if (bInQuotes && i + 1 < Line.Len() && Line[i + 1] == '\"')
			{
				Current.AppendChar('\"'); // Quotation marks exception
				i++; // Skip second mark
			}
			else
			{
				bInQuotes = !bInQuotes; // Has quotation marks
			}
		}
		else if (Char == ',' && !bInQuotes)
		{
			Cells.Add(Current);
			Current.Empty();
		}
		else
		{
			Current.AppendChar(Char);
		}
	}
	Cells.Add(Current);
	return Cells;
}


void UDialogueCSVTool::CreateDialogueDataTableAsset(UDataTable*& OutTable, TArray<FDialogueRow> DialogueRows)
{
	// Check path and name of the asset
	if (!PackagePath.StartsWith(TEXT("/Game")))
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: The package path must start with /Game"));
		return;
	}
	if (DataTableName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Insert a name for the Data Table"));
		return;
	}

	FString PackageName = PackagePath + TEXT("/") + DataTableName;

	// Replace existing asset
	if (UEditorAssetLibrary::DoesAssetExist(PackageName))
	{
		UObject* LoadedObj = UEditorAssetLibrary::LoadAsset(PackageName);
		OutTable = Cast<UDataTable>(LoadedObj);
	}

	// Create package
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Could not create package"));
		return;
	}

	OutTable = NewObject<UDataTable>(Package, UDataTable::StaticClass(), *DataTableName, RF_Public | RF_Standalone);
	OutTable->RowStruct = FDialogueRow::StaticStruct();
	OutTable->MarkPackageDirty();

	// Fill Table Rows
	for (const FDialogueRow& Row : DialogueRows)
	{
		OutTable->AddRow(FName(Row.Scene), Row);
	}
	FAssetRegistryModule::AssetCreated(OutTable);

	// Save package
	FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;

	bool bSaved = UPackage::SavePackage(Package, OutTable, *FilePath, SaveArgs);
	if (bSaved)
	{
		UE_LOG(LogTemp, Log, TEXT("DataTable saved as asset in: %s"), *PackageName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save DataTable asset: %s"), *PackageName);
	}
}