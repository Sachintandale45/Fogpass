#include <iostream>
#include "CoreState.h"
#include "landmark.h"

void setupLandmarkLogic(CoreState *state)
{
    QObject::connect(state, &CoreState::fogModeChanged, [](bool foggy){
        if (foggy) {
            std::cout << "[LANDMARK] Logic triggered: FOGGY" << std::endl;
        } else {
            std::cout << "[LANDMARK] Logic triggered: CLEAR" << std::endl;
        }
    });
}