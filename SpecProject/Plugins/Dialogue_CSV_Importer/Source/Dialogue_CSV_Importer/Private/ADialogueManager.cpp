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
	bIsDialogueActive = true;
	FindDialogue(SceneName);
}

void AADialogueManager::AdvanceDialogue()
{
	if (!CurrentDialogueRow && !bIsDialogueActive)
		return;
	FindDialogue(CurrentDialogueRow->NextScene);
}

void AADialogueManager::FindDialogue(FName SceneName)
{
	FDialogueRow* DialogueRow = nullptr;
	
	for (UDataTable* DataTable : DialogueDataTables)
	{
		if (!DataTable)
		{
			UE_LOG(LogTemp,Warning,TEXT("The current DataTable is null!"))	
			continue;
		}
		if (SceneName == TEXT("") || SceneName == TEXT("END"))
		{
			OnDialogueEnd.Broadcast();
			bIsDialogueActive = false;
			return;
		}
		DialogueRow = DataTable->FindRow<FDialogueRow>(SceneName,"Dialogue Row Finder",true);
		if (!DialogueRow)
			continue;
		DialogueEvent.Broadcast(DialogueRow->EventName);
		OnDialogueLine.Broadcast(*DialogueRow);
		CurrentDialogueRow = DialogueRow;
		return;
	}
	if (!DialogueRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dialogue Row %s can't be found in any of the DataTable!"), *SceneName.ToString());
		bIsDialogueActive = false;
	}
}

