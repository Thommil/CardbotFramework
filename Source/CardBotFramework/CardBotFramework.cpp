#include "CardBotFramework.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, CardBotFramework, "CardBotFramework" );

void ConsoleInfo(const FString &message)
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *message);
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *message);
    }
}

void ConsoleWarning(const FString &message)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *message);
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *message);
    }
}

void ConsoleError(const FString &message)
{
    UE_LOG(LogTemp, Error, TEXT("%s"), *message);
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *message);
    }
}
