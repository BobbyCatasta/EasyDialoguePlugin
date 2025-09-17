// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueCSVTool.h"
#include "HttpModule.h"
#include "Http.h"
#include "Interfaces/IHttpResponse.h"
#include "Structs/FDialogueRow.h"

// Sets default values for this component's properties
UDialogueCSVTool::UDialogueCSVTool() {}


void UDialogueCSVTool::DownloadCSVFromURL(const FString& URL)
{
	// Ottieni modulo HTTP
	FHttpModule* Http = &FHttpModule::Get();

	// Crea richiesta
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb("GET");

	// Callback alla fine del download
	Request->OnProcessRequestComplete().BindUObject(this, &UDialogueCSVTool::OnCSVDownloaded);

	// Invia richiesta
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
	UE_LOG(LogTemp, Log, TEXT("CSV downloaded correctly."));
	//UE_LOG(LogTemp, Log, TEXT("%s"), *CSVContent);

	TArray<FDialogueRow> DialogueRows = ParseCSV(CSVContent);
	
	for (FDialogueRow Row : DialogueRows)
	{
		UE_LOG(LogTemp, Log, TEXT("%s"),*Row.Scene);
	}

	// Esempio: popola Scenes / Character
	// Tables  (fare cosa simile con le datatable, tabella per scene, tabella per dialoghi personaggi etc...)
	//PopulateDialogMaps(DialogRows);
}

TArray<FDialogueRow> UDialogueCSVTool::ParseCSV(const FString Content)
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

		// Split gestendo le virgolette
		TArray<FString> Cells;
		Line.ParseIntoArray(Cells, TEXT(","), false);

		FDialogueRow Row;
		
		Row.Scene = Cells[0];
		Row.Id = Cells[1];
		Row.Speaker = Cells[2];
		
		ParsedRows.Add(Row);
	}

	return ParsedRows;
}

