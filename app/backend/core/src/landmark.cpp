#include <iostream>
#include "CoreState.h"
#include "landmark.h"
#include "LandmarkEngine.h"

void setupLandmarkLogic(CoreState *state, LandmarkEngine *engine)
{
    // The old logic is now handled by the LandmarkEngine.
    // We could connect other state logic here if needed.
    (void)state; // Mark as unused to prevent compiler warnings
    engine->start();
}