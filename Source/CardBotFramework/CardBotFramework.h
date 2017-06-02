#pragma once

#include "Engine.h"

//CONFIG
#define DEBUG_MODE TRUE


//MACROS
#if(DEBUG_MODE == TRUE)
    #define INFO(message) Info(message);
    #define WARNING(message) Warning(message);
    #define ERROR(message) Error(message);
#else
    #define INFO(message)
    #define WARNING(message)
    #define ERROR(message)
#endif

void Info(const FString &message);
void Warning(const FString &message);
void Error(const FString &message);


