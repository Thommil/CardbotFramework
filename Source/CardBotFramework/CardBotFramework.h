#pragma once

#include "Engine.h"

//CONFIG
#define DEBUG_MODE TRUE

#define DEFAULT_OBJECT_POOL_SIZE 8

#define DEFAULT_COLLISIONEVENTDATA_POOL_SIZE 4
#define DEFAULT_DAMAGEEVENTDATA_POOL_SIZE 4

//MACROS
#define ERROR(message) Error(message);

#if(DEBUG_MODE == TRUE)
    #define INFO(message) Info(message);
    #define WARNING(message) Warning(message);
#else
    #define INFO(message)
    #define WARNING(message)
#endif

void Info(const FString &message);
void Warning(const FString &message);
void Error(const FString &message);


