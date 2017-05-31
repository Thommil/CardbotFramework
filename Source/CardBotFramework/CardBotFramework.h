#pragma once

#include "Engine.h"

//CONFIG
#define DEBUG_MODE TRUE


//MACROS
#if(DEBUG_MODE == TRUE)
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


