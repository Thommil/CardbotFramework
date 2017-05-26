#pragma once

#include "Engine.h"

#define NO_DEBUG 0
#define CONSOLE_DEBUG 1
#define VIEWPORT_DEBUG 2

#define DEBUG_MODE CONSOLE_DEBUG

#if (DEBUG_MODE == VIEWPORT_DEBUG)
    #define INFO(message) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, message);
    #define WARNING(message) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, message);
    #define ERROR(message) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, message);
#elif (DEBUG_MODE == CONSOLE_DEBUG)
    #define INFO(message) ConsoleInfo(message);
    #define WARNING(message) ConsoleWarning(message);
    #define ERROR(message) ConsoleError(message);
#else
    #define INFO(message)
    #define WARNING(message)
    #define ERROR(message)
#endif

void ConsoleInfo(const FString &message);
void ConsoleWarning(const FString &message);
void ConsoleError(const FString &message);


