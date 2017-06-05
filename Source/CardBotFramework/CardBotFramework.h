#pragma once

#include "Engine.h"

//CONFIG
#define DEBUG_MODE TRUE

#define DEFAULT_COLLISION_EVENTDATA_POOL_SIZE 16
#define DEFAULT_DAMAGE_EVENTDATA_POOL_SIZE 8

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


