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
	
	//
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(FName SceneName);

	//
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();

	
private:

	//
	bool bIsDialogueActive = false;

	//
	void FindDialogue(FName SceneName);
	
	//
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TArray<UDataTable*> DialogueDataTables;

	//
	FDialogueRow* CurrentDialogueRow;
	
	//
	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FOnDialogueLine OnDialogueLine;

	//
	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FOnDialogueEnd OnDialogueEnd;

	//
	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FDialogueEvent DialogueEvent;
};
