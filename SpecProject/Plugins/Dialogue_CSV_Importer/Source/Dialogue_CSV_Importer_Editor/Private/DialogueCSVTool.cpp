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


void UDialogueCSVTool::DownloadCSVAndCreateDataTable(const FString& DataTablePath, const FString& AssetName, const FString& URL)
{
	PackagePath = DataTablePath;
	DataTableName = AssetName;
	
	FHttpModule* Http = &FHttpModule::Get();

	// HTTP Request
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	if (URL.IsEmpty())
	{
		Request->SetURL(TEXT("https://docs.google.com/spreadsheets/d/e/2PACX-1vRmH43TxcGmiz4NA44X67RGiCfEb9B553j-qocvUzPU"
			 "0NwCizhIRZoJFYMt-UuRj9-QhCOhLtLtxBKT/pub?gid=2120350590&single=true&output=csv"));
		UE_LOG(LogTemp, Display, TEXT("Using example table from Google Docs..."));
	}
	else
	{
		Request->SetURL(URL);
	}
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
		DialogueLogging.Broadcast(TEXT("ERROR : Could not download CSV."),EDialogueLogType::ERROR);
		UE_LOG(LogTemp, Error, TEXT("ERROR : Could not download CSV."));
		return;
	}

	FString CSVContent = UTF8_TO_TCHAR(Response->GetContent().GetData());

	// You can use "END TABLE" in the CSV for separating the Data Table part for design text and other useful text
	int32 FoundIndex = CSVContent.Find(TEXT("END TABLE"), ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (FoundIndex != INDEX_NONE)
		CSVContent = CSVContent.Left(FoundIndex);

	DialogueLogging.Broadcast(TEXT("CSV downloaded correctly. Now Creating Data Table..."),EDialogueLogType::LOG);
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
		if (!Cells[3].IsNumeric())
		{
			UE_LOG(LogTemp, Warning, TEXT("The Dialogue %s has a non valid float. Using default value..."),*DialogueRow.Scene.ToString());
		}
		else
		{
			DialogueRow.TextSpeed = FCString::Atof(*Cells[3]);
		}
		DialogueRow.NextScene = FName(Cells[4]);
		DialogueRow.EventName = FName(Cells[5]);
		
		ParsedRows.Add(DialogueRow);
	}

	return ParsedRows;
}

TArray<FString> UDialogueCSVTool::ParseCSVLine(const FString& Line)
{
	TArray<FString> Cells;
	FString Current;
	bool bInQuotes = false; // Tracks whether we are inside quotation marks

	for (int32 i = 0; i < Line.Len(); i++)
	{
		TCHAR Char = Line[i];

		if (Char == '\"')
		{
			// Handle quotes
			if (bInQuotes && i + 1 < Line.Len() && Line[i + 1] == '\"')
			{
				// Escaped quotes inside a quoted field
				Current.AppendChar('\"');
				i++; // Skip the second quote
			}
			else
			{
				// Entering or exiting quoted section
				bInQuotes = !bInQuotes;
			}
		}
		else if (Char == ',' && !bInQuotes)
		{
			// Comma found outside quotes
			Cells.Add(Current);
			Current.Empty();
		}
		else
		{
			// Normal character 
			Current.AppendChar(Char);
		}
	}

	// Add the last cell
	Cells.Add(Current);

	return Cells;
}


void UDialogueCSVTool::CreateDialogueDataTableAsset(UDataTable*& OutTable, TArray<FDialogueRow> DialogueRows)
{
	// Check path and name of the asset
	if (PackagePath.IsEmpty())
	{
	    // If no path is provided, use a default path inside /Game
	    PackagePath = TEXT("/Game/DialogueDataTables");
	    UE_LOG(LogTemp, Log, TEXT("Using default path for saving data table..."));
	}
	if (DataTableName.IsEmpty())
	{
	    // If no name is provided, use a default name for the asset
	    DataTableName = TEXT("NewDialogueDataTable");
	    UE_LOG(LogTemp, Log, TEXT("Using default name for asset..."));
	}

	FString PackageName = PackagePath + TEXT("/") + DataTableName;

	// Replace existing asset (if it already exists in the Content Browser)
	if (UEditorAssetLibrary::DoesAssetExist(PackageName))
	{
	    UObject* LoadedObj = UEditorAssetLibrary::LoadAsset(PackageName);
	    OutTable = Cast<UDataTable>(LoadedObj);
	}

	// Create a new package for the DataTable
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
	    DialogueLogging.Broadcast(TEXT("ERROR: Could not create package. Please retry."), EDialogueLogType::ERROR);
	    UE_LOG(LogTemp, Error, TEXT("ERROR: Could not create package. Please retry."));
	    return;
	}

	// Create the DataTable object inside the package
	OutTable = NewObject<UDataTable>(Package, UDataTable::StaticClass(), *DataTableName, RF_Public | RF_Standalone);
	OutTable->RowStruct = FDialogueRow::StaticStruct(); // Bind our custom row struct
	OutTable->MarkPackageDirty(); // Mark package as modified (so Unreal knows it needs saving)

	// Fill the DataTable with parsed rows from the CSV
	for (const FDialogueRow& Row : DialogueRows)
	{
	    OutTable->AddRow(FName(Row.Scene), Row);
	}
	// Notify the AssetRegistry so the new asset appears in the Content Browser
	FAssetRegistryModule::AssetCreated(OutTable);

	// Convert package name into a physical file path
	FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	FString FolderPath = FPaths::GetPath(FilePath);

	// Ensure that the folder exists on disk, otherwise create it
	if (!IFileManager::Get().DirectoryExists(*FolderPath))
	{
	    if (!IFileManager::Get().MakeDirectory(*FolderPath, true))
	    {
	        DialogueLogging.Broadcast(TEXT("ERROR : Failed to create directory on disk"), EDialogueLogType::ERROR);
	        UE_LOG(LogTemp, Error, TEXT("ERROR : Failed to create directory on disk: %s"), *FolderPath);
	        return;
	    }
	    UE_LOG(LogTemp, Log, TEXT("Created directory: %s"), *FolderPath);
	}

	// Prepare save arguments for UPackage::SavePackage
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;

	// Save the package to disk as a .uasset file
	bool bSaved = UPackage::SavePackage(Package, OutTable, *FilePath, SaveArgs);
	if (bSaved)
	{
	    DialogueLogging.Broadcast(FString::Printf(TEXT("DataTable saved as asset in : \n\n%s"),*PackageName), EDialogueLogType::DONE);
	    UE_LOG(LogTemp, Log, TEXT("DataTable saved as asset in: %s"), *PackageName);
	}
	else
	{
	    DialogueLogging.Broadcast(TEXT("Failed to create and save DataTable asset"), EDialogueLogType::ERROR);
	    UE_LOG(LogTemp, Error, TEXT("Failed to save DataTable asset: %s"), *PackageName);
	}
}