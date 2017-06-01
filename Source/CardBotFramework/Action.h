#pragma once

#include "Action.generated.h"


/**
 * Action types to define routing policies by "channels"
 */
UENUM(BlueprintType)
enum class EActionType : uint8
{
    Move        = 0x01  ,
    Attack      = 0x02  ,
    Morph       = 0x04  ,
    Special     = 0x08  ,
    System      = 0x10
};


/**
 * Action types to define routing capabilities by "channels"
 */
UENUM(BlueprintType, meta=(Bitflags))
enum class EActionCapability : uint8
{
    Move        = 0x01  ,
    Attack      = 0x02  ,
    Morph       = 0x04  ,
    Special     = 0x08  ,
    System      = 0x10  
};
ENUM_CLASS_FLAGS(EActionCapability)
