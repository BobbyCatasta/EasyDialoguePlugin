// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueCSVTool.h"
#include "HttpModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/DataTableFactory.h"
#include "EditorAssetLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Structs/FDialogueRow.h"

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
	int32 FoundIndex = CSVContent.Find(TEXT("END TABLE"), ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (FoundIndex != INDEX_NONE)
		CSVContent = CSVContent.Left(FoundIndex);
	
	UE_LOG(LogTemp, Log, TEXT("CSV downloaded correctly. Now Creating Data Table..."));

	TArray<FDialogueRow> DialogueRows = ParseCSV(CSVContent);
	UDataTable* DataTable;
	// To change, not based by AssetName but based on GID of the Table, create a Table Named based on gid
	CreateDialogueDataTableAsset(DataTable, DialogueRows);
	
}

TArray<FDialogueRow> UDialogueCSVTool::ParseCSV(const FString& Content)
{
	TArray<FDialogueRow> ParsedRows;
	TArray<FString> Lines;
	Content.ParseIntoArrayLines(Lines); // divide per riga

	if (Lines.Num() <= 1)
		return ParsedRows;

	// 
	for (int32 i = 1; i < Lines.Num(); i++)
	{
		FString Line = Lines[i];

		// Split delle Celle
		TArray<FString> Cells;
		Line.ParseIntoArray(Cells, TEXT(","), false);

		// Riempimento Riga di Dialogo
		FDialogueRow DialogueRow;
		
		DialogueRow.Scene = Cells[0];
		DialogueRow.Id = Cells[1];
		DialogueRow.Speaker = Cells[2];
		DialogueRow.Dialogue = FText::FromString(Cells[3]);
		DialogueRow.TextSpeed = FCString::Atof(*Cells[4]);
		
		ParsedRows.Add(DialogueRow);
	}

	return ParsedRows;
}

void UDialogueCSVTool::CreateDialogueDataTableAsset(UDataTable*& OutTable, TArray<FDialogueRow> DialogueRows)
{
	if (!PackagePath.StartsWith(TEXT("/Game")))
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR : The package path must start with /Game"));
		return;
	}
	if (DataTableName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR : Insert a name for the Data Table"));
		return;
	}
	FString PackageName = PackagePath + TEXT("/") + DataTableName;
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR : Error creating the package, try again"));
	}

	OutTable = NewObject<UDataTable>(Package, *DataTableName, RF_Public | RF_Standalone);
	OutTable->RowStruct = FDialogueRow::StaticStruct();

	for (FDialogueRow Row : DialogueRows)
	{
		OutTable->AddRow(FName(Row.Scene),Row);
	}
	// Registra l’asset nel Content Browser
	FAssetRegistryModule::AssetCreated(OutTable);
	
	OutTable->MarkPackageDirty();

	FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

	// Check if the asset already exists in the project
	if (UEditorAssetLibrary::DoesAssetExist(FilePath))
		UEditorAssetLibrary::DeleteAsset(FilePath);
	
	bool bSaved = UPackage::SavePackage(Package, OutTable, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *FilePath);

	if (bSaved)
		UE_LOG(LogTemp, Log, TEXT("DataTable saved as asset in : %s"), *PackagePath);
}

