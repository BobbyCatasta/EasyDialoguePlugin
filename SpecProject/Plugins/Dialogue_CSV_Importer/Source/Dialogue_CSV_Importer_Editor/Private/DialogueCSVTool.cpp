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
			UE_LOG(LogTemp, Warning, TEXT("The Dialogue %s has been skipped because of a non valid float."),*DialogueRow.Scene.ToString());
			continue;
		}
		DialogueRow.TextSpeed = FCString::Atof(*Cells[3]);
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
	if (PackagePath.IsEmpty())
	{
	    PackagePath = TEXT("/Game/DialogueDataTables");
	    UE_LOG(LogTemp, Log, TEXT("Using default path for saving data table..."));
	}
	if (DataTableName.IsEmpty())
	{
	    DataTableName = TEXT("NewDialogueDataTable");
	    UE_LOG(LogTemp, Log, TEXT("Using default name for asset..."));
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
	    DialogueLogging.Broadcast(TEXT("ERROR: Could not create package. Please retry."), EDialogueLogType::ERROR);
	    UE_LOG(LogTemp, Error, TEXT("ERROR: Could not create package. Please retry."));
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
	FString FolderPath = FPaths::GetPath(FilePath);

	// Create folder if it doesn't exist
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

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;

	bool bSaved = UPackage::SavePackage(Package, OutTable, *FilePath, SaveArgs);
	if (bSaved)
	{
	    DialogueLogging.Broadcast(FString::Printf(TEXT("DataTable saved as asset in : \n%s"),*PackageName), EDialogueLogType::DONE);
	    UE_LOG(LogTemp, Log, TEXT("DataTable saved as asset in: %s"), *PackageName);
	}
	else
	{
	    DialogueLogging.Broadcast(TEXT("Failed to create and save DataTable asset"), EDialogueLogType::ERROR);
	    UE_LOG(LogTemp, Error, TEXT("Failed to save DataTable asset: %s"), *PackageName);
	}
}