#pragma once

class CoreState; // Forward declaration
class LandmarkEngine; // Forward declaration

// Wires up the landmark business logic to the core state
void setupLandmarkLogic(CoreState *state, LandmarkEngine *engine);