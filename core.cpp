// -- core file -- //

// include SFML
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// include standard libraries
#include <cstring>
#include <vector>
#include <queue>
#include <stack>
#include <cmath>

// include project
#include "data.h"
#include "base.h"
#include "load.h"
#include "sfml.h"
#include "render.h"
#include "sign.h"
#include "light.h"
#include "tile.h"
#include "car.h"
#include "world.h"
#include "gui.h"
#include "debug.h"
#include "game.h"

// main function
int main(int argc, char** argv) {
    // reset RNG
    srand(time(NULL));

    // load stuff
    int err = resource.import();
    if (err) return err;

    // load map
    FILE* fp = fopen("test.dat", "rb");
    world.load(fp);
    fclose(fp);

    // add test car
    /*cars.objs.push_back(Car(Cars::Smart, sf::Vector2i(9, 9), Direction::South, Cars::Taxi, 5));
    cars.objs.push_back(Car(Cars::Smart, sf::Vector2i(16, 6), Direction::South, Cars::Van, 4));
    cars.objs.push_back(Car(Cars::Smart, sf::Vector2i(19, 12), Direction::South, Cars::Luxury, 7));
    world.getTile(cars.objs[0].tp)->car = &cars.objs[0];
    world.getTile(cars.objs[1].tp)->car = &cars.objs[1];
    world.getTile(cars.objs[2].tp)->car = &cars.objs[2];

    Car& fcar = cars.objs[0];

    // add test path
    cars.objs[0].pathfindService(Vector2s(cars.objs[0].tp), tileFetch, Service::Parking, 1);
    cars.objs[1].pathfindService(Vector2s(cars.objs[1].tp), tileFetch, Service::Shop, 1);
    cars.objs[2].pathfindService(Vector2s(cars.objs[2].tp), tileFetch, Service::Shop, 1);*/

    // create window
    sfml.createWindow(false);

    // create FPS clock
    sf::Uint8 clockUpdateTimer = 0;
    sf::Int64 clockBuffer = 0;
    sf::Clock clock;

    // main loop
    while (sfml.app.isOpen()) {
        // update state
        game.updateGame();

        // clear window
        sfml.app.clear(sf::Color(34, 39, 41));

        // render window
        sf::Int64 FPS = clock.restart().asMicroseconds();
        if (clockUpdateTimer-- <= 0) {
            clockUpdateTimer = 15;
            clockBuffer = FPS;
        };
        game.renderGame(clockBuffer);

        // display window
        sfml.app.display();

        // handle events
        sf::Event evt;
        while (sfml.app.pollEvent(evt)) {
            if (sfml.handleEvent(evt))
                game.handleEvent(evt);
        };
    };

    // success
    return 0x0;
};
