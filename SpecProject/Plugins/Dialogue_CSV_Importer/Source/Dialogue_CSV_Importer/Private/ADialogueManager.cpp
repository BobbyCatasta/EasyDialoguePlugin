// Fill out your copyright notice in the Description page of Project Settings.


#include "ADialogueManager.h"

// Sets default values
AADialogueManager::AADialogueManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AADialogueManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AADialogueManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AADialogueManager::StartDialogue(FName SceneName)
{
	// Mark dialogue as active and search for the starting scene
	bIsDialogueActive = true;
	FindDialogue(SceneName);
}

void AADialogueManager::AdvanceDialogue()
{
	// If there is no current dialogue or dialogue is not active, do nothing
	if (!CurrentDialogueRow && !bIsDialogueActive)
		return;

	// Move to the next dialogue row using the "NextScene" value
	FindDialogue(CurrentDialogueRow->NextScene);
}

void AADialogueManager::FindDialogue(FName SceneName)
{
	FDialogueRow* DialogueRow = nullptr;
    
	for (UDataTable* DataTable : DialogueDataTables)
	{
		if (!DataTable)
		{
			// Warn if a DataTable reference is invalid
			UE_LOG(LogTemp, Warning, TEXT("The current DataTable is null!"))	
			continue;
		}

		// If the scene name is empty or "END", stop the dialogue
		if (SceneName == TEXT("") || SceneName == TEXT("END"))
		{
			OnDialogueEnd.Broadcast();
			bIsDialogueActive = false;
			return;
		}

		// Look up the dialogue row in the current DataTable
		DialogueRow = DataTable->FindRow<FDialogueRow>(SceneName, "Dialogue Row Finder", true);
		if (!DialogueRow)
			continue;

		// Broadcast any associated gameplay event
		DialogueEvent.Broadcast(DialogueRow->EventName);

		// Broadcast the dialogue line for UI or gameplay to consume
		OnDialogueLine.Broadcast(*DialogueRow);

		// Store current row for advancing later
		CurrentDialogueRow = DialogueRow;
		return;
	}

	// If no matching row was found in any DataTable, log a warning and stop
	if (!DialogueRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dialogue Row %s can't be found in any of the DataTable!"), *SceneName.ToString());
		bIsDialogueActive = false;
	}
}

