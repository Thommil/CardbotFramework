#pragma once

#include "Engine.h"

#define NO_DEBUG 0
#define CONSOLE_DEBUG 1
#define VIEWPORT_DEBUG 2

#define DEBUG_MODE VIEWPORT_DEBUG

#if (DEBUG_MODE == VIEWPORT_DEBUG)
    #define INFO(message) GEngine->AddOnScreenDebugMessage(FMath::RandRange(0,TNumericLimits<int32>::Max()), 2.0f, FColor::Green, message);
    #define WARNING(message) GEngine->AddOnScreenDebugMessage(FMath::RandRange(0,TNumericLimits<int32>::Max()), 2.0f, FColor::Yellow, message);
    #define ERROR(message) GEngine->AddOnScreenDebugMessage(FMath::RandRange(0,TNumericLimits<int32>::Max()), 2.0f, FColor::Red, message);
#elif (DEBUG_MODE == CONSOLE_DEBUG)
    #define INFO(message) UE_LOG(LogTemp, Log, message);
    #define WARNING(message) UE_LOG(LogTemp, Warning, message);
    #define ERROR(message) UE_LOG(LogTemp, Error, message);
#else
    #define INFO(message)
    #define WARNING(message)
    #define ERROR(message)
#endif
