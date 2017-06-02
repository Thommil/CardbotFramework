#include "CardBotFramework.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, CardBotFramework, "CardBotFramework" );

void Info(const FString &message)
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *message);
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *message);
    }
}

void Warning(const FString &message)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *message);
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *message);
    }
}

void Error(const FString &message)
{
    UE_LOG(LogTemp, Error, TEXT("%s"), *message);
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *message);
    }
}
