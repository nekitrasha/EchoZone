// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZBaseGameMode.h"
#include "EchoZone/Character/AEZCharacter.h"
#include "AEZPLayerController.h"

AEZBaseGameMode::AEZBaseGameMode()
{
	DefaultPawnClass = AEZCharacter::StaticClass();
	PlayerControllerClass = AEZPlayerController::StaticClass();
}
