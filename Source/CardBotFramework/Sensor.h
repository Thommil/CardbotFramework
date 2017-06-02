#pragma once

#include "Sensor.generated.h"


/**
 * Sensors types to define routing policies by "channels"
 */
UENUM(BlueprintType, meta=(Bitflags) )
enum class ESensorType : uint8
{
    Damage        = 1  ,
    Collision     = 2  ,
    Sonar         = 3
};
