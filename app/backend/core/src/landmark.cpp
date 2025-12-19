#include <iostream>
#include "CoreState.h"
#include "landmark.h"
#include <QTimer>

void setupLandmarkLogic(CoreState *state)
{
    QObject::connect(state, &CoreState::fogModeChanged, [](bool foggy){
        if (foggy) {
            std::cout << "[LANDMARK] Logic triggered: FOGGY" << std::endl;
        } else {
            std::cout << "[LANDMARK] Logic triggered: CLEAR" << std::endl;
        }
    });

    // Simulate setting landmark locations after a delay
    QTimer::singleShot(5000, [state]() {
        QStringList locations;
        locations << "Eiffel Tower";
        locations << "Louvre Museum";
        locations << "Notre-Dame Cathedral";

        std::cout << "[LANDMARK] Setting landmark locations..." << std::endl;
        state->setLandmarkLocations(locations);
    });
}