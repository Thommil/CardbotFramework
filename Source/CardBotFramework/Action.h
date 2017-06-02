#pragma once

#include "Action.generated.h"


/**
 * Action types to define routing policies by "channels"
 */
UENUM(BlueprintType, meta=(Bitflags) )
enum class EActionType : uint8
{
    Move        = 1  ,
    Attack      = 2  ,
    Morph       = 3  ,
    Special     = 4  ,
    System      = 5
};
