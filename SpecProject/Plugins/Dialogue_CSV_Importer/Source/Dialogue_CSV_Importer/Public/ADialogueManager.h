// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/FDialogueRow.h"
#include "ADialogueManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueLine,const FDialogueRow&,DialogueLine);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueEvent,FName,EventName);

class UDataTable;

UCLASS()
class DIALOGUE_CSV_IMPORTER_API AADialogueManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AADialogueManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Start a dialogue sequence at the given scene
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(FName SceneName);

	// Advance the dialogue to the next scene/line
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();

	
protected:

	// Keeps track of whether a dialogue sequence is currently active
	bool bIsDialogueActive = false;

	// Internal function to find and process a dialogue row by scene name
	void FindDialogue(FName SceneName);
	
	// List of DataTables containing dialogue rows that can be searched
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TArray<UDataTable*> DialogueDataTables;

	// Pointer to the currently active dialogue row
	FDialogueRow* CurrentDialogueRow;
	
	// Event triggered whenever a dialogue line needs to be displayed
	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FOnDialogueLine OnDialogueLine;

	// Event triggered when a dialogue sequence ends
	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FOnDialogueEnd OnDialogueEnd;

	// Event triggered when a dialogue row requests a gameplay-related event
	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FDialogueEvent DialogueEvent;
};
