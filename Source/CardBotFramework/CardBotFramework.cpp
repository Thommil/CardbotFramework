#include "CardBotFramework.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, CardBotFramework, "CardBotFramework" );

void ConsoleInfo(const FString &message)
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *message);
}

void ConsoleWarning(const FString &message)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *message);
}

void ConsoleError(const FString &message)
{
    UE_LOG(LogTemp, Error, TEXT("%s"), *message);
}
