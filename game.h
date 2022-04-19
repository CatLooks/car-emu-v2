// -- game controller -- //

// finder function
Tile* tileFetch(Vector2s pos) {
    return world.getTileS(pos);
};

// encapsulate into <game>
struct {

    // variables
    sf::Vector2i mouseBuf = sf::Vector2i(-1, -1);
    States::Game state = States::View;
    States::Game bufst = States::Menu;

    // flags
    bool enterCmd = false;
    bool enterFst = false;
    bool eraser = false;
    bool showService = false;
    bool showRoutes = false;
    bool showDebug = true;      // CHANGE IN RELEASE TO FALSE
    bool placeCar = false;

    // handle game events
    void handleEvent(sf::Event evt) {
        // get window size
        sf::Vector2i windowSize = (sf::Vector2i)sfml.app.getSize();

        // key press event
        if (evt.type == sf::Event::KeyPressed) {
            // toggle game state
            if (evt.key.code == sf::Keyboard::Tab && !enterCmd) {
                switch (state) {
                    case States::Menu: break;
                    case States::View:
                    state = States::Edit;
                    world.camera.x += paletteWidth / 2;
                    break;
                    case States::Edit:
                    state = States::View;
                    world.camera.x -= paletteWidth / 2;
                    break;
                };
                return;
            };

            // save game
            if (evt.key.code == sf::Keyboard::F5 && !enterCmd) {
                FILE* fp = fopen("test.dat", "wb");
                world.dump(fp, state == States::Edit);
                fclose(fp);
            };

            if (enterCmd) {
                // exit command input
                if (evt.key.code == sf::Keyboard::Escape)
                    enterCmd = false;

                // shift cursor
                if (evt.key.code == sf::Keyboard::Left)
                    gui.cmdcur = clamp(gui.cmdcur - 1, 0, gui.cmd.size());
                if (evt.key.code == sf::Keyboard::Right)
                    gui.cmdcur = clamp(gui.cmdcur + 1, 0, gui.cmd.size());
            } else {
                // tick game
                if (evt.key.code == sf::Keyboard::T) {
                    // update cars
                    world.update(tileFetch, (sf::Vector2u)windowSize);

                    // update light timer
                    lights.update();
                };

                // pause game
                if (evt.key.code == sf::Keyboard::P) {
                    world.speed = world.speed ? 0 : 1;
                };

                // menu / exit car
                if (evt.key.code == sf::Keyboard::Escape) {
                    if (world.control) {
                        // stop car control
                        world.control = NULL;
                    } else {
                        // switch room
                        States::Game temps = state;
                        state = bufst;
                        bufst = temps;
                    };
                };

                // new car
                if (evt.key.code == sf::Keyboard::N) {
                    placeCar = !placeCar;
                };

                // test
                if (evt.key.code == sf::Keyboard::M) {
                    printf("Dests:\n");
                    for (auto iter = cars.objs.front().dest.begin(); iter != cars.objs.front().dest.end(); iter++) {
                        printf("%d %d\n", (*iter).x, (*iter).y);
                    };
                };
            };

            // enter command input
            if (evt.key.code == sf::Keyboard::Slash && !enterCmd) {
                enterCmd = true;
                enterFst = true;
                gui.cmd.clear();
                gui.cmdcur = 0;
            };

            // switch level
            if (evt.key.code == sf::Keyboard::Space && !enterCmd) {
                world.underground = !world.underground;
                return;
            };

            // toggle shows
            if (!enterCmd) {
                switch (evt.key.code) {
                    case sf::Keyboard::Num0: showService = !showService; break;
                    case sf::Keyboard::Num9: showRoutes  = !showRoutes;  break;
                    default: break;
                };

                // quick mode
                if (state == States::Edit) {
                    switch (evt.key.code) {
                        case sf::Keyboard::Num1: gui.state = States::Ground;       break;
                        case sf::Keyboard::Num2: gui.state = States::Outline;      break;
                        case sf::Keyboard::Num3: gui.state = States::Deco;         break;
                        case sf::Keyboard::Num4: gui.state = States::Route;        break;
                        case sf::Keyboard::Num5: gui.state = States::Service;      break;
                        case sf::Keyboard::Num6: gui.state = States::RoadSign;     break;
                        case sf::Keyboard::Num7: gui.state = States::TrafficLight; break;
                        case sf::Keyboard::Num8: gui.state = States::MapInfo;      break;
                        default: break;
                    };
                };

                // toggle eraser
                if (evt.key.code == sf::Keyboard::E) {
                    eraser = !eraser;
                    gui.editorBuf = sf::Vector2i(-1, -1);
                    gui.originBuf = sf::Vector2i(-1, -1);
                    return;
                };

                // default zoom
                if (evt.key.code == sf::Keyboard::Home) {
                    int bz = world.zoom;
                    world.zoom = 64;

                    // remap camera
                    world.camera.x = (world.camera.x) * float(world.zoom) / bz;
                    world.camera.y = (world.camera.y) * float(world.zoom) / bz;
                    return;
                };
            };

            // debug mode
            if (evt.key.code == sf::Keyboard::F12 && evt.key.control) {
                showDebug = !showDebug;
                return;
            };
        }

        // mouse press event
        else if (evt.type == sf::Event::MouseButtonPressed) {
            // LMB
            if (evt.mouseButton.button == sf::Mouse::Left) {
                // get window size
                sf::Vector2i windowSize = (sf::Vector2i)sfml.app.getSize();

                // switch mode
                switch (state) {
                    case States::View:
                    clickView(windowSize, sf::Vector2i(evt.mouseButton.x, evt.mouseButton.y));
                    break;
                    case States::Edit:
                    clickEdit(windowSize, sf::Vector2i(evt.mouseButton.x, evt.mouseButton.y));
                    break;
                    case States::Menu:
                    clickMenu(windowSize, sf::Vector2i(evt.mouseButton.x, evt.mouseButton.y));
                    break;
                };
            };
        }

        // mouse scroll event
        else if (evt.type == sf::Event::MouseWheelMoved) {
            if (evt.mouseWheel.x < windowSize.x - paletteWidth) {
                // buffer initial zoom
                int bz = world.zoom;

                // resize zoom
                if (evt.mouseWheel.delta > 0)
                    world.zoom *= 1.0f + evt.mouseWheel.delta / 8.0f;
                else
                    world.zoom /= 1.0f - evt.mouseWheel.delta / 8.0f;

                // zoom bounds
                if (world.zoom % 2)
                    world.zoom++;
                if (world.zoom < 8)
                    world.zoom = 8;
                if (world.zoom > 1024)
                    world.zoom = 1024;

                // remap camera
                world.camera.x = (world.camera.x) * float(world.zoom) / bz;
                world.camera.y = (world.camera.y) * float(world.zoom) / bz;

            } else {
                if (state == States::Edit) {
                    if (gui.state == States::RoadSign) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(sfml.app);
                        int dx = windowSize.x - paletteWidth + 16;
                        int dy = paletteStart + 16;

                        // sign reference
                        Sign* sign = gui.sobj ? gui.sobj : &gui.sign;

                        // check for speed slider
                        if (sf::IntRect(dx + 100, dy + 196, 120, 40).contains(mousePos)) {
                            sign->speed = clamp(sign->speed + evt.mouseWheel.delta, 0, 250);
                        }

                        // check for position sliders
                        else if (gui.sobj) {
                            if (sf::IntRect(dx + 48, dy + 13 * 16 + buttonSize + 0 * 48, 176, 48).contains(mousePos)) {
                                world.removeSign(sign);
                                int buf = sign->pos.x;
                                int pos = sign->pos.x + evt.mouseWheel.delta;

                                while (true) {
                                    if (pos < 0 || pos >= world.size.x) {
                                        sign->pos.x = buf;
                                        world.placeSign(sign);
                                        msgBuffer.push_back(Triplet<char*, sf::Color, int>("Unable to place a sign", TEXT_INV, 120));
                                        break;
                                    };

                                    sign->pos.x = clamp(pos, 0, 65535);
                                    if (world.placeSign(sign)) break;
                                    pos += evt.mouseWheel.delta > 0 ? 1 : -1;
                                };
                            }
                            else if (sf::IntRect(dx + 48, dy + 13 * 16 + buttonSize + 1 * 48, 176, 48).contains(mousePos)) {
                                world.removeSign(sign);
                                int buf = sign->pos.y;
                                int pos = sign->pos.y + evt.mouseWheel.delta;

                                while (true) {
                                    if (pos < 0 || pos >= world.size.y) {
                                        sign->pos.y = buf;
                                        world.placeSign(sign);
                                        msgBuffer.push_back(Triplet<char*, sf::Color, int>("Unable to place a sign", TEXT_INV, 120));
                                        break;
                                    };

                                    sign->pos.y = clamp(pos, 0, 65535);
                                    if (world.placeSign(sign)) break;
                                    pos += evt.mouseWheel.delta > 0 ? 1 : -1;
                                };
                            };
                        };

                    } else if (gui.state == States::TrafficLight) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(sfml.app);
                        int dx = windowSize.x - paletteWidth + 16;
                        int dy = paletteStart + 16;

                        // light reference
                        Light* light = gui.lobj ? gui.lobj : &gui.sample;

                        // check for sliders
                        if (sf::IntRect(dx + 52,  dy + 0 * 48 + 4, 80, 40).contains(mousePos)) {
                            int val = clamp(light->red / 60 + evt.mouseWheel.delta, 0, 79);
                            int oth = light->red % 60;
                            light->red = val * 60 + oth;
                        }
                        else if (sf::IntRect(dx + 140, dy + 0 * 48 + 4, 80, 40).contains(mousePos)) {
                            int val = clamp(light->red % 60 + evt.mouseWheel.delta, 0, 59);
                            int oth = light->red / 60;
                            light->red = oth * 60 + val;
                        }
                        else if (sf::IntRect(dx + 52,  dy + 1 * 48 + 4, 80, 40).contains(mousePos)) {
                            int val = clamp(light->green / 60 + evt.mouseWheel.delta, 0, 79);
                            int oth = light->green % 60;
                            light->green = val * 60 + oth;
                        }
                        else if (sf::IntRect(dx + 140, dy + 1 * 48 + 4, 80, 40).contains(mousePos)) {
                            int val = clamp(light->green % 60 + evt.mouseWheel.delta, 0, 59);
                            int oth = light->green / 60;
                            light->green = oth * 60 + val;
                        }
                        else if (sf::IntRect(dx + 52,  dy + 2 * 48 + 4, 80, 40).contains(mousePos)) {
                            int val = clamp(light->yellow / 60 + evt.mouseWheel.delta, 0, 79);
                            int oth = light->yellow % 60;
                            light->yellow = val * 60 + oth;
                        }
                        else if (sf::IntRect(dx + 140, dy + 2 * 48 + 4, 80, 40).contains(mousePos)) {
                            int val = clamp(light->yellow % 60 + evt.mouseWheel.delta, 0, 59);
                            int oth = light->yellow / 60;
                            light->yellow = oth * 60 + val;
                        }
                        else if (sf::IntRect(dx + 52,  dy + 3 * 48 + 4, 80, 40).contains(mousePos)) {
                            int val = clamp(light->init / 60 + evt.mouseWheel.delta, 0, 79);
                            int oth = light->init % 60;
                            light->init = val * 60 + oth;
                        }
                        else if (sf::IntRect(dx + 140, dy + 3 * 48 + 4, 80, 40).contains(mousePos)) {
                            int val = clamp(light->init % 60 + evt.mouseWheel.delta, 0, 59);
                            int oth = light->init / 60;
                            light->init = oth * 60 + val;
                        }

                        // check for position sliders
                        else if (gui.lobj) {
                            if (sf::IntRect(dx + 48, dy + 10 * 32 + buttonSize + 0 * 48, 176, 48).contains(mousePos)) {
                                world.removeLight(light);
                                int buf = light->pos.x;
                                int pos = light->pos.x + evt.mouseWheel.delta;

                                while (true) {
                                    if (pos < 0 || pos >= world.size.x) {
                                        light->pos.x = buf;
                                        world.placeLight(light);
                                        msgBuffer.push_back(Triplet<char*, sf::Color, int>("Unable to place a traffic light", TEXT_INV, 120));
                                        break;
                                    };

                                    light->pos.x = clamp(pos, 0, 65535);
                                    if (world.placeLight(light)) break;
                                    pos += evt.mouseWheel.delta > 0 ? 1 : -1;
                                };
                            }
                            else if (sf::IntRect(dx + 48, dy + 10 * 32 + buttonSize + 1 * 48, 176, 48).contains(mousePos)) {
                                world.removeLight(light);
                                int buf = light->pos.y;
                                int pos = light->pos.y + evt.mouseWheel.delta;

                                while (true) {
                                    if (pos < 0 || pos >= world.size.y) {
                                        light->pos.y = buf;
                                        world.placeLight(light);
                                        msgBuffer.push_back(Triplet<char*, sf::Color, int>("Unable to place a traffic light", TEXT_INV, 120));
                                        break;
                                    };

                                    light->pos.y = clamp(pos, 0, 65535);
                                    if (world.placeLight(light)) break;
                                    pos += evt.mouseWheel.delta > 0 ? 1 : -1;
                                };
                            };
                        };
                    } else if (gui.state == States::MapInfo) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(sfml.app);
                        int dx = windowSize.x - paletteWidth + 16;
                        int dy = paletteStart + 16;

                        // check resize numbers
                        if (sf::IntRect(dx + 8,   dy + 112 + buttonSize * 1, buttonSize, buttonSize).contains(mousePos))
                            world.resize.left   += evt.mouseWheel.delta;
                        else if (sf::IntRect(dx + 152, dy + 112 + buttonSize * 1, buttonSize, buttonSize).contains(mousePos))
                            world.resize.width  += evt.mouseWheel.delta;
                        else if (sf::IntRect(dx + 80,  dy + 104 + buttonSize * 0, buttonSize, buttonSize).contains(mousePos))
                            world.resize.top    += evt.mouseWheel.delta;
                        else if (sf::IntRect(dx + 80,  dy + 120 + buttonSize * 2, buttonSize, buttonSize).contains(mousePos))
                            world.resize.height += evt.mouseWheel.delta;

                        // clamp resize numbers
                        world.resize.width  = clamp(world.resize.width,  1, 65535);
                        world.resize.height = clamp(world.resize.height, 1, 65535);
                    };
                };
            };
        }

        // text input event
        else if (enterCmd && evt.type == sf::Event::TextEntered && !enterFst) {
            char c = (sf::Uint8)evt.text.unicode;

            // check for backspace
            if (c == '\b') {
                if (!gui.cmd.empty())
                    gui.cmd.erase(gui.cmd.begin() + --gui.cmdcur);
            }

            // check for return
            else if (c == '\r' || c == '\n') {
                executeCommand(gui.cmd);
                gui.cmd.clear();
                enterCmd = false;
            }

            // add character to command
            else {
                gui.cmd.insert(gui.cmd.begin() + gui.cmdcur++, c);
            };
        };
    };

    // update game
    void updateGame() {
        // variables
        static sf::Vector2i bufferPos;
        const int cameraSpeed = lerp(4, 64, world.zoom / 1024.0f);

        // get window size
        sf::Vector2u windowSize = sfml.app.getSize();

        // update first frame after open
        enterFst = false;

        // get mouse position
        sf::Vector2i mousePos = sf::Mouse::getPosition(sfml.app);
        if (gui.state == States::Outline && gui.cursor == Outline::AutoMark) {
            world.cursor.x = floor(float(world.camera.x + mousePos.x - (int)windowSize.x / 2) / world.zoom - 0.5f);
            world.cursor.y = floor(float(world.camera.y + mousePos.y - (int)windowSize.y / 2) / world.zoom - 0.5f);
        } else {
            world.cursor.x = floor(float(world.camera.x + mousePos.x - (int)windowSize.x / 2) / world.zoom);
            world.cursor.y = floor(float(world.camera.y + mousePos.y - (int)windowSize.y / 2) / world.zoom);
        };

        // update flags
        if (!enterCmd) {
            // move camera by keyboard
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                world.camera.x -= cameraSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                world.camera.x += cameraSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                world.camera.y -= cameraSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                world.camera.y += cameraSpeed;
        };

        // move camera by mouse
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            // check for first click
            if (bufferPos.x == -1)
                bufferPos = mousePos;

            // shift camera
            world.camera += bufferPos - mousePos;
            bufferPos = mousePos;

        } else bufferPos.x = -1;

        // hold menu
        if (state == States::Edit && sf::Mouse::isButtonPressed(sf::Mouse::Left) && mousePos.x >= (int)windowSize.x - paletteWidth) {
            switch (gui.state) {
                case States::Ground: {

                };
                break;
                case States::Outline: {

                };
                break;
                case States::Deco: {

                };
                break;
                case States::Route: {

                };
                break;
                case States::Service: {

                };
                break;
                case States::RoadSign: {
                    int dx = windowSize.x - paletteWidth + 16;
                    int dy = paletteStart + 16;

                    // sign reference
                    Sign* sign = gui.sobj ? gui.sobj : &gui.sign;

                    // check for speed slider
                    if (sf::IntRect(dx + 100, dy + 196, 120, 40).contains(mousePos)) {
                        sign->speed = (mousePos.x - dx - 100) * 250 / 119;
                    };
                };
                break;
                case States::TrafficLight: {
                    int dx = windowSize.x - paletteWidth + 16;
                    int dy = paletteStart + 16;

                    // light reference
                    Light* light = gui.lobj ? gui.lobj : &gui.sample;

                    // check for timer sliders
                    if (sf::IntRect(dx + 52,  dy + 0 * 48 + 4, 80, 40).contains(mousePos)) {
                        int val = (mousePos.x - dx - 52);
                        int oth = light->red % 60;
                        light->red = val * 60 + oth;
                    }
                    else if (sf::IntRect(dx + 140, dy + 0 * 48 + 4, 80, 40).contains(mousePos)) {
                        int val = (mousePos.x - dx - 140) * 59 / 79;
                        int oth = light->red / 60;
                        light->red = oth * 60 + val;
                    }
                    else if (sf::IntRect(dx + 52,  dy + 1 * 48 + 4, 80, 40).contains(mousePos)) {
                        int val = (mousePos.x - dx - 52);
                        int oth = light->green % 60;
                        light->green = val * 60 + oth;
                    }
                    else if (sf::IntRect(dx + 140, dy + 1 * 48 + 4, 80, 40).contains(mousePos)) {
                        int val = (mousePos.x - dx - 140) * 59 / 79;
                        int oth = light->green / 60;
                        light->green = oth * 60 + val;
                    }
                    else if (sf::IntRect(dx + 52,  dy + 2 * 48 + 4, 80, 40).contains(mousePos)) {
                        int val = (mousePos.x - dx - 52);
                        int oth = light->yellow % 60;
                        light->yellow = val * 60 + oth;
                    }
                    else if (sf::IntRect(dx + 140, dy + 2 * 48 + 4, 80, 40).contains(mousePos)) {
                        int val = (mousePos.x - dx - 140) * 59 / 79;
                        int oth = light->yellow / 60;
                        light->yellow = oth * 60 + val;
                    }
                    else if (sf::IntRect(dx + 52,  dy + 3 * 48 + 4, 80, 40).contains(mousePos)) {
                        int val = (mousePos.x - dx - 52);
                        int oth = light->init % 60;
                        light->init = val * 60 + oth;
                    }
                    else if (sf::IntRect(dx + 140, dy + 3 * 48 + 4, 80, 40).contains(mousePos)) {
                        int val = (mousePos.x - dx - 140) * 59 / 79;
                        int oth = light->init / 60;
                        light->init = oth * 60 + val;
                    };
                };
                break;
                case States::MapInfo: {

                };
                break;
                default:
                break;
            };
        };

        // edit tile
        if (state == States::Edit && sf::Mouse::isButtonPressed(sf::Mouse::Left) && mousePos.x < (int)windowSize.x - paletteWidth) {
            // check for buttons
            if (sf::IntRect(windowSize.x - buttonSize * 5 - paletteThick, 0, buttonSize, buttonSize).contains(mousePos))
                return;

            // switch by state
            switch (gui.state) {
                case States::Ground: {
                    // fetch tile
                    Tile* tile = world.getTile(world.cursor);
                    if (tile == NULL) break;

                    // erase ground
                    if (eraser) {
                        tile->ground(world.underground) = Ground::Void;
                        tile->parts(world.underground) = 0;
                        break;
                    };

                    // fill ground
                    if (gui.cursor == -1) break;
                    tile->ground(world.underground) = static_cast<Ground::types>(gui.cursor);
                    if (gui.cursor == Ground::Void) tile->parts(world.underground) = 0;
                };
                break;
                case States::Outline: {
                    // fetch tile
                    Tile* tile = world.getTile(world.cursor);

                    // check for auto marks
                    if (gui.cursor == Outline::AutoMark) {
                        // process part NE
                        Tile* partNE = tile;
                        if (partNE) {
                            sf::Uint8& parts = partNE->parts(world.underground);
                            if (eraser) {
                                parts &= 0x9E;
                            } else {
                                parts |= ((parts >> 2 & 1) || (parts >> 7 & 1)) << 6;
                                parts |= ((parts >> 1 & 1) || (parts >> 4 & 1)) << 5;
                                parts |= 1 << 0;
                            };
                            partNE->outline(world.underground) = Outline::Mark;
                        };
                        // process part NW
                        Tile* partNW = world.getTile(world.cursor + sf::Vector2i(1, 0));
                        if (partNW) {
                            sf::Uint8& parts = partNW->parts(world.underground);
                            if (eraser) {
                                parts &= 0xCD;
                            } else {
                                parts |= ((parts >> 3 & 1) || (parts >> 7 & 1)) << 4;
                                parts |= ((parts >> 0 & 1) || (parts >> 6 & 1)) << 5;
                                parts |= 1 << 1;
                            };
                            partNW->outline(world.underground) = Outline::Mark;
                        };
                        // process part SE
                        Tile* partSE = world.getTile(world.cursor + sf::Vector2i(0, 1));
                        if (partSE) {
                            sf::Uint8& parts = partSE->parts(world.underground);
                            if (eraser) {
                                parts &= 0x3B;
                            } else {
                                parts |= ((parts >> 3 & 1) || (parts >> 4 & 1)) << 7;
                                parts |= ((parts >> 0 & 1) || (parts >> 5 & 1)) << 6;
                                parts |= 1 << 2;
                            };
                            partSE->outline(world.underground) = Outline::Mark;
                        };
                        // process part SW
                        Tile* partSW = world.getTile(world.cursor + sf::Vector2i(1, 1));
                        if (partSW) {
                            sf::Uint8& parts = partSW->parts(world.underground);
                            if (eraser) {
                                parts &= 0x67;
                            } else {
                                parts |= ((parts >> 2 & 1) || (parts >> 6 & 1)) << 7;
                                parts |= ((parts >> 1 & 1) || (parts >> 5 & 1)) << 4;
                                parts |= 1 << 3;
                            };
                            partSW->outline(world.underground) = Outline::Mark;
                        };
                        break;
                    };

                    // check tile
                    if (tile == NULL) break;

                    // check for auto outline
                    if (gui.cursor == Outline::Auto) {
                        // init
                        Ground::types& tlg = tile->ground(world.underground);
                        Outline::types outline = Outline::None;
                        tile->parts(world.underground) = 0;

                        for (int i = 0; i < 8; i++) {
                            // fetch neighbor
                            Tile* nb = world.getTile(world.cursor + neighborOffsets[i]);
                            if (nb == NULL) continue;

                            // ground types references
                            Ground::types& nbg = nb->ground(world.underground);

                            // get outline texture
                            if (outline == Outline::None && Outline::outlines[nbg] && !Ground::similar[tlg][nbg])
                                outline = Outline::outlines[nbg];

                            // update outline side
                            if (!Ground::similar[tlg][nbg])
                                tile->parts(world.underground) |= 1 << (i ^ 0x7);
                        };
                        tile->outline(world.underground) = outline;
                        break;
                    };

                    // check cursor
                    if (gui.cursor == -1) break;

                    // fill outline
                    if (eraser) {
                        tile->outline(world.underground) = Outline::None;
                        tile->parts(world.underground) = 0;
                    } else {
                        tile->outline(world.underground) = static_cast<Outline::types>(gui.cursor);
                        tile->parts(world.underground) = gui.cursor ? gui.parts : 0;
                    };
                };
                break;
                case States::Deco: {
                    // fetch tile
                    Tile* tile = world.getTile(world.cursor);
                    if (tile == NULL) break;

                    // erase decoration
                    if (eraser) {
                        tile->deco(world.underground) = Decoration::None;
                        break;
                    };

                    // check cursor
                    if (gui.cursor == -1) break;

                    // replace decoration
                    tile->deco(world.underground) = static_cast<Decoration::types>(gui.cursor);
                };
                break;
                case States::Route: {
                    // fetch tile
                    Tile* tile = world.getTile(world.cursor);
                    if (tile == NULL) break;

                    // erase route
                    if (eraser) {
                        // get route array
                        Way::types* routes = tile->routes(world.underground);
                        routes[0] = Way::None;
                        routes[1] = Way::None;
                        routes[2] = Way::None;
                        routes[3] = Way::None;
                        break;
                    };

                    // check for autofill
                    if (gui.autofill) {
                        // check mouse buffer
                        if (world.cursor != mouseBuf) {
                            mouseBuf = world.cursor;

                            // fetch edited tile
                            Tile* tile = world.getTile(gui.editorBuf);
                            if (tile == NULL) {
                                // shift buffers
                                gui.originBuf = gui.editorBuf;
                                gui.editorBuf = mouseBuf;
                                break;
                            };

                            // get out direction
                            Direction out = adjacentSide(gui.editorBuf, mouseBuf);
                            if (out == Direction::None) {
                                // shift buffers
                                gui.originBuf = gui.editorBuf;
                                gui.editorBuf = mouseBuf;
                                break;
                            };

                            // get in direction
                            Direction inp = adjacentSide(gui.editorBuf, gui.originBuf);
                            if (inp == Direction::None) {
                                // shift buffers
                                gui.originBuf = gui.editorBuf;
                                gui.editorBuf = mouseBuf;
                                break;
                            };

                            // create route
                            Way::types* routes = tile->routes(world.underground);
                            routes[convertDirection(inp)] = Way::In;
                            routes[convertDirection(out)] = Way::Out;

                            // shift buffers
                            gui.originBuf = gui.editorBuf;
                            gui.editorBuf = mouseBuf;
                        };
                        break;
                    };

                    // fill route
                    Way::types* routes = tile->routes(world.underground);
                    routes[0] = gui.route[0];
                    routes[1] = gui.route[1];
                    routes[2] = gui.route[2];
                    routes[3] = gui.route[3];
                };
                break;
                case States::Service: {
                    // check cursor
                    if (gui.cursor == -1) break;

                    // fetch tile
                    Tile* tile = world.getTile(world.cursor);
                    if (tile == NULL) break;

                    // replace service
                    tile->service = static_cast<Service::types>(gui.cursor);
                };
                break;
                case States::RoadSign: {

                };
                break;
                case States::TrafficLight: {

                };
                break;
                case States::MapInfo: {

                };
                break;
                default:
                break;
            };
        };

        // tick world
        if (state == States::View) {
            for (int i = 0; i < world.speed; i++) {
                // update cars
                world.update(tileFetch, windowSize);

                // update light timer
                lights.update();
            };
        } else {
            for (int i = 0; i < world.speed; i++) {
                // update light timer
                lights.update();
            };
        };
    };

    // render game
    void renderGame(sf::Int64 FPS) {
        // get window size
        sf::Vector2i windowSize = (sf::Vector2i)sfml.app.getSize();

        // create renderer
        Renderer renderer;

        // check for menu
        if (state == States::Menu) {


        } else {
            // render world tiles
            if (state == States::View)
                world.render(renderer, sfml.app.getSize(), false, showRoutes, showService, false, false, lights.timer, false);
            else
                world.render(renderer, sfml.app.getSize(), true, gui.state == States::Route || showRoutes, gui.state == States::Service || showService,
                             gui.state == States::RoadSign, gui.state == States::TrafficLight, lights.timer, gui.state == States::MapInfo);

            // render GUI
            if (state == States::Edit) {
                gui.renderEdit(renderer, windowSize, eraser);
            } else if (state == States::View) {
                if (placeCar)
                    renderer.push(6, sf::IntRect(world.cursor.x * world.zoom - world.camera.x + windowSize.x / 2, world.cursor.y * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 96, 32, 32));
                gui.renderCarGUI(renderer, windowSize);
            };

            // render buttons
            gui.renderCtrl(renderer, windowSize, state, enterCmd);

            // render debug messages
            debugMessage(renderer, windowSize);

            // render debug info
            Tile* tile = world.getTile(world.cursor);
            debugTile(renderer, tile, world.cursor, showDebug);
        };

        // render rect buffer
        Pair<int, int> vc = renderer.render();

        // render vertex count
        if (showDebug) {
            char text[256];
            sprintf(text, "FPS: %d\nDisplay: %dQ + %dT", FPS ? int(1000000 / FPS) : 0, vc.a / 4, vc.b + 1);
            resource.text.setString(sf::String(text));
            resource.text.setCharacterSize(16);
            resource.text.setFillColor(TEXT_USE);
            resource.text.setPosition(8, windowSize.y - (enterCmd ? 108 : 48));
            sfml.app.draw(resource.text);
        };

        // update animation
        for (int i = 0; i < world.speed; i++) {
            if (animTimer++ >= animSpeed) {
                animTimer = 0;
                animFrame++;
            };
        };
    };

    // menu mouse click
    void clickMenu(sf::Vector2i windowSize, sf::Vector2i pos) {

    };

    // view mouse click
    void clickView(sf::Vector2i windowSize, sf::Vector2i pos) {
        // menu button
        if (sf::IntRect(windowSize.x - buttonSize * 1, 0, buttonSize, buttonSize).contains(pos)) {
            States::Game temps = state;
            state = bufst;
            bufst = temps;
            return;
        };

        // speed button
        if (sf::IntRect(windowSize.x - buttonSize * 2, 0, buttonSize, buttonSize).contains(pos)) {
            world.speed = (world.speed + 1) % 3;
            return;
        };

        // level button
        if (sf::IntRect(windowSize.x - buttonSize * 3, 0, buttonSize, buttonSize).contains(pos)) {
            world.underground = !world.underground;
            return;
        };

        // state button
        if (sf::IntRect(windowSize.x - buttonSize * 4, 0, buttonSize, buttonSize).contains(pos)) {
            switch (state) {
                case States::Menu: break;
                case States::View: state = States::Edit; break;
                case States::Edit: state = States::View; break;
            };
            return;
        };

        // edit car button
        if (sf::IntRect(windowSize.x - buttonSize * 5, 0, buttonSize, buttonSize).contains(pos)) {
            gui.carMenu = !gui.carMenu;
            return;
        };

        // place car button
        if (sf::IntRect(windowSize.x - buttonSize * 6, 0, buttonSize, buttonSize).contains(pos)) {
            placeCar = !placeCar;
            return;
        };

        // click on map
        if ((pos.x < windowSize.x - paletteWidth) || !gui.carMenu) {
            // fetch cursor
            Tile* tile = world.getTile(world.cursor);
            if (tile == NULL) return;

            // check for car placement
            if (placeCar) {
                // check for existing car
                if (tile->car) {
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>("Car at this position already exists", TEXT_INV, 120));
                } else {
                    cars.objs.push_back(Car(Cars::NoAI, world.cursor, static_cast<Direction>(rand() % 4), static_cast<Cars::types>(rand() % Cars::CarCount), rand() % 24 + 8));
                    tile->car = &cars.objs.back();
                };
                placeCar = false;
            } else {
                // check for car
                if (tile->car)
                    world.control = world.control == tile->car ? NULL : tile->car;
                else if (world.control) {
                    // check for recurence
                    for (int i = world.control->dest.size() - 1; i >= 0; i--) {
                        if (world.control->dest[i] == Vector2s(world.cursor)) {
                            // cut path
                            int j = world.control->dest.size();
                            for (i++; i < j; i++) {
                                world.control->dest.pop_back();
                            };
                            return;
                        };
                    };

                    // path find to tile
                    Vector2s pos;
                    if (world.control->dest.empty())
                        pos = Vector2s(world.control->tp);
                    else
                        pos = world.control->dest.back();
                    if (!world.control->pathfindPoint(pos, tileFetch, Vector2s(world.cursor)))
                        msgBuffer.push_back(Triplet<char*, sf::Color, int>("Unable to generate a route", TEXT_INV, 120));
                };
                return;
            };
        };
    };

    // edit mouse click
    void clickEdit(sf::Vector2i windowSize, sf::Vector2i pos) {
        // menu button
        if (sf::IntRect(windowSize.x - buttonSize * 1, 0, buttonSize, buttonSize).contains(pos)) {
            States::Game temps = state;
            state = bufst;
            bufst = temps;
            return;
        };

        // speed button
        if (sf::IntRect(windowSize.x - buttonSize * 2, 0, buttonSize, buttonSize).contains(pos)) {
            world.speed = (world.speed + 1) % 3;
            return;
        };

        // level button
        if (sf::IntRect(windowSize.x - buttonSize * 3, 0, buttonSize, buttonSize).contains(pos)) {
            world.underground = !world.underground;
            return;
        };

        // state button
        if (sf::IntRect(windowSize.x - buttonSize * 4, 0, buttonSize, buttonSize).contains(pos)) {
            switch (state) {
                case States::Menu: break;
                case States::View: state = States::Edit; break;
                case States::Edit: state = States::View; break;
            };
            return;
        };

        // mode button
        if (sf::IntRect(windowSize.x - buttonSize * 12 - paletteThick, 0, buttonSize * 8, buttonSize).contains(pos)) {
            gui.state = static_cast<States::Editor>((pos.x - windowSize.x + buttonSize * 12 + paletteThick) / buttonSize);
            gui.cursor = -1;
            return;
        };

        // map click
        if (sf::IntRect(0, 0, windowSize.x - paletteWidth, windowSize.y).contains(pos)) {
            switch (gui.state) {
                case States::Ground: {

                };
                break;
                case States::Outline: {

                };
                break;
                case States::Deco: {

                };
                break;
                case States::Route: {

                };
                break;
                case States::Service: {

                };
                break;
                case States::RoadSign: {
                    // get tile
                    Tile* tile = world.getTile(world.cursor);
                    if (tile == NULL) {
                        gui.sobj = NULL;
                        break;
                    };
                    gui.sobj = (gui.sobj == tile->sign) ? NULL : tile->sign;
                };
                break;
                case States::TrafficLight: {
                    // get tile
                    Tile* tile = world.getTile(world.cursor);
                    if (tile == NULL) {
                        gui.lobj = NULL;
                        break;
                    };
                    gui.lobj = (gui.lobj == tile->light) ? NULL : tile->light;
                };
                break;
                default:
                case States::MapInfo: {

                };
                break;
                break;
            };
            return;
        };

        // menu click
        if (sf::IntRect(windowSize.x - paletteWidth, paletteStart, paletteWidth, windowSize.y - paletteStart).contains(pos)) {
            switch (gui.state) {
                case States::Ground: {
                    // set cursor
                    int x = (pos.x - windowSize.x + paletteWidth) / buttonSize;
                    int y = (pos.y - paletteStart) / buttonSize;
                    int i = y * 4 + x;
                    gui.cursor = (i >= 0 && i < Ground::GroundCount && gui.cursor != i) ? i : -1;
                };
                break;
                break;
                case States::Outline: {
                    // check buttons
                    int dx = windowSize.x - paletteWidth / 2;
                    int dy = paletteStart + buttonSize * 3 / 2;

                    if (gui.cursor != Outline::Auto && gui.cursor != Outline::AutoMark) {
                        if (sf::IntRect(dx - buttonSize / 4,                                dy + buttonSize - paletteThick - buttonPadding,         buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.parts ^= 0x80;
                            break;
                        };
                        if (sf::IntRect(dx + buttonSize / 2 + paletteThick + buttonPadding, dy + buttonSize * 7 / 4,                                buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.parts ^= 0x40;
                            break;
                        };
                        if (sf::IntRect(dx - buttonSize / 4,                                dy + buttonSize * 5 / 2 + paletteThick + buttonPadding, buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.parts ^= 0x20;
                            break;
                        };
                        if (sf::IntRect(dx - buttonSize - paletteThick - buttonPadding,     dy + buttonSize * 7 / 4,                                buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.parts ^= 0x10;
                            break;
                        };
                        if (sf::IntRect(dx - buttonSize - paletteThick - buttonPadding,     dy + buttonSize - paletteThick - buttonPadding,         buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.parts ^= 0x08;
                            break;
                        };
                        if (sf::IntRect(dx + buttonSize / 2 + paletteThick + buttonPadding, dy + buttonSize - paletteThick - buttonPadding,         buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.parts ^= 0x04;
                            break;
                        };
                        if (sf::IntRect(dx - buttonSize - paletteThick - buttonPadding,     dy + buttonSize * 5 / 2 + paletteThick + buttonPadding, buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.parts ^= 0x02;
                            break;
                        };
                        if (sf::IntRect(dx + buttonSize / 2 + paletteThick + buttonPadding, dy + buttonSize * 5 / 2 + paletteThick + buttonPadding, buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.parts ^= 0x01;
                            break;
                        };

                        // check edit buttons
                        if (sf::IntRect(windowSize.x - paletteWidth + 0 * buttonSize, paletteStart + buttonSize * 5, buttonSize, buttonSize).contains(pos)) {
                            bool buffer[8] { gui.parts >> 0 & 1, gui.parts >> 1 & 1, gui.parts >> 2 & 1, gui.parts >> 3 & 1,
                                             gui.parts >> 4 & 1, gui.parts >> 5 & 1, gui.parts >> 6 & 1, gui.parts >> 7 & 1};
                            gui.parts = buffer[6] << 7 | buffer[5] << 6 | buffer[4] << 5 | buffer[7] << 4 | buffer[2] << 3 | buffer[0] << 2 | buffer[3] << 1 | buffer[1];
                            break;
                        };
                        if ((sf::IntRect(windowSize.x - paletteWidth + 1 * buttonSize, paletteStart + buttonSize * 5, buttonSize, buttonSize).contains(pos))) {
                            gui.parts = 0xFF;
                            break;
                        };
                        if ((sf::IntRect(windowSize.x - paletteWidth + 2 * buttonSize, paletteStart + buttonSize * 5, buttonSize, buttonSize).contains(pos))) {
                            gui.parts = 0x00;
                            break;
                        };
                        if (sf::IntRect(windowSize.x - paletteWidth + 3 * buttonSize, paletteStart + buttonSize * 5, buttonSize, buttonSize).contains(pos)) {
                            bool buffer[8] { gui.parts >> 0 & 1, gui.parts >> 1 & 1, gui.parts >> 2 & 1, gui.parts >> 3 & 1,
                                             gui.parts >> 4 & 1, gui.parts >> 5 & 1, gui.parts >> 6 & 1, gui.parts >> 7 & 1};
                            gui.parts = buffer[4] << 7 | buffer[7] << 6 | buffer[6] << 5 | buffer[5] << 4 | buffer[1] << 3 | buffer[3] << 2 | buffer[0] << 1 | buffer[2];
                            break;
                        };
                    };

                    // check for y-pos
                    if (pos.y >= dy + buttonSize - paletteThick - buttonPadding) break;

                    // set cursor
                    int x = (pos.x - windowSize.x + paletteWidth) / buttonSize;
                    int y = (pos.y - paletteStart) / buttonSize;
                    int i = y * 4 + x;
                    gui.cursor = (i >= 0 && i < Outline::OutlineCount && gui.cursor != i) ? i : -1;
                };
                break;
                case States::Deco: {
                    // set cursor
                    int x = (pos.x - windowSize.x + paletteWidth) / buttonSize;
                    int y = (pos.y - paletteStart) / buttonSize;
                    int i = y * 4 + x;
                    gui.cursor = (i >= 0 && i < Decoration::DecorationCount && gui.cursor != i) ? i : -1;
                };
                break;
                case States::Route: {
                    int dx = windowSize.x - paletteWidth / 2;
                    int dy = paletteStart + buttonSize * 3 / 2;

                    if (!gui.autofill) {
                        // check buttons
                        if (sf::IntRect(dx - buttonSize / 4, dy - buttonSize - paletteThick - buttonPadding, buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.route[0] = static_cast<Way::types>((gui.route[0] + 1) % 4);
                            break;
                        };
                        if (sf::IntRect(dx + buttonSize / 2 + paletteThick + buttonPadding, dy - buttonSize / 4, buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.route[1] = static_cast<Way::types>((gui.route[1] + 1) % 4);
                            break;
                        };
                        if (sf::IntRect(dx - buttonSize / 4, dy + buttonSize / 2 + paletteThick + buttonPadding, buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.route[2] = static_cast<Way::types>((gui.route[2] + 1) % 4);
                            break;
                        };
                        if (sf::IntRect(dx - buttonSize - paletteThick - buttonPadding, dy - buttonSize / 4, buttonSize / 2, buttonSize / 2).contains(pos)) {
                            gui.route[3] = static_cast<Way::types>((gui.route[3] + 1) % 4);
                            break;
                        };

                        // check rotate buttons
                        if (sf::IntRect(windowSize.x - paletteWidth + 0 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize).contains(pos)) {
                            Way::types buf[4] {gui.route[1], gui.route[2], gui.route[3], gui.route[0]};
                            for (int i = 0; i < 4; i++) gui.route[i] = buf[i];
                            break;
                        };
                        if (sf::IntRect(windowSize.x - paletteWidth + 3 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize).contains(pos)) {
                            Way::types buf[4] {gui.route[3], gui.route[0], gui.route[1], gui.route[2]};
                            for (int i = 0; i < 4; i++) gui.route[i] = buf[i];
                            break;
                        };
                    };

                    // check mode buttons
                    if (sf::IntRect(windowSize.x - paletteWidth + 1 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize).contains(pos)) {
                        gui.autofill = false;
                        break;
                    };
                    if (sf::IntRect(windowSize.x - paletteWidth + 2 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize).contains(pos)) {
                        gui.autofill = true;
                        gui.editorBuf = sf::Vector2i(-1, -1);
                        gui.originBuf = sf::Vector2i(-1, -1);
                        break;
                    };
                };
                break;
                case States::Service: {
                    // set cursor
                    int x = (pos.x - windowSize.x + paletteWidth) / buttonSize;
                    int y = (pos.y - paletteStart) / buttonSize;
                    int i = y * 4 + x;
                    gui.cursor = (i >= 0 && i < Service::ServiceCount && gui.cursor != i) ? i : -1;
                };
                break;
                case States::RoadSign: {
                    int dx = windowSize.x - paletteWidth + 16;
                    int dy = paletteStart + 16;

                    // sign reference
                    Sign* sign = gui.sobj ? gui.sobj : &gui.sign;

                    // check type buttons
                    if (sf::IntRect(dx - 16 + 0 * buttonSize, dy + 32, buttonSize, buttonSize).contains(pos)) {
                        sign->type = RoadSign::Lock;
                        break;
                    };
                    if (sf::IntRect(dx - 16 + 1 * buttonSize, dy + 32, buttonSize, buttonSize).contains(pos)) {
                        sign->type = RoadSign::Max;
                        break;
                    };
                    if (sf::IntRect(dx - 16 + 2 * buttonSize, dy + 32, buttonSize, buttonSize).contains(pos)) {
                        sign->type = RoadSign::Min;
                        break;
                    };
                    if (sf::IntRect(dx - 16 + 3 * buttonSize, dy + 32, buttonSize, buttonSize).contains(pos)) {
                        sign->type = RoadSign::Walk;
                        break;
                    };

                    // check corner buttons
                    if (sf::IntRect(dx + 0 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2).contains(pos)) {
                        sign->corner ^= 0x8;
                        break;
                    };
                    if (sf::IntRect(dx + 1 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2).contains(pos)) {
                        sign->corner ^= 0x4;
                        break;
                    };
                    if (sf::IntRect(dx + 2 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2).contains(pos)) {
                        sign->corner ^= 0x2;
                        break;
                    };
                    if (sf::IntRect(dx + 3 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2).contains(pos)) {
                        sign->corner ^= 0x1;
                        break;
                    };

                    // check special button
                    if (sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 64).contains(pos)) {
                        if (gui.sobj) {
                            // delete button
                            world.deleteSign(gui.sobj);
                            gui.sobj = NULL;
                        } else {
                            // create button
                            gui.sobj = world.createSign((world.camera - sf::Vector2i(paletteWidth / 2, 0)) / world.zoom, gui.sign);
                        };
                    };
                };
                break;
                case States::TrafficLight: {
                    int dx = windowSize.x - paletteWidth + 16;
                    int dy = paletteStart + 16;

                    // light reference
                    Light* light = gui.lobj ? gui.lobj : &gui.sample;

                    // check type buttons
                    if (sf::IntRect(dx + 16 + 0 * buttonSize, dy + 7 * 32, buttonSize, buttonSize).contains(pos)) {
                        light->type = TrafficLight::Single;
                        break;
                    };
                    if (sf::IntRect(dx + 16 + 1 * buttonSize, dy + 7 * 32, buttonSize, buttonSize).contains(pos)) {
                        light->type = TrafficLight::Mini;
                        break;
                    };
                    if (sf::IntRect(dx + 16 + 2 * buttonSize, dy + 7 * 32, buttonSize, buttonSize).contains(pos)) {
                        light->type = TrafficLight::Full;
                        break;
                    };

                    // check corner buttons
                    if (sf::IntRect(dx + 0 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2).contains(pos)) {
                        light->corner ^= 0x8;
                        break;
                    };
                    if (sf::IntRect(dx + 1 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2).contains(pos)) {
                        light->corner ^= 0x4;
                        break;
                    };
                    if (sf::IntRect(dx + 2 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2).contains(pos)) {
                        light->corner ^= 0x2;
                        break;
                    };
                    if (sf::IntRect(dx + 3 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2).contains(pos)) {
                        light->corner ^= 0x1;
                        break;
                    };

                    // check special button
                    if (sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 64).contains(pos)) {
                        if (gui.lobj) {
                            // delete button
                            world.deleteLight(gui.lobj);
                            gui.lobj = NULL;
                        } else {
                            // create button
                            gui.lobj = world.createLight((world.camera - sf::Vector2i(paletteWidth / 2, 0)) / world.zoom, gui.sample);
                        };
                    };
                };
                break;
                case States::MapInfo: {
                    int dx = windowSize.x - paletteWidth + 16;
                    int dy = paletteStart + 16;

                    // check template type
                    if (pos.y >= dy + 160 + buttonSize * 3 && pos.y < dy + 160 + buttonSize * 5) {
                        // set cursor
                        int x = (pos.x - windowSize.x + paletteWidth) / buttonSize;
                        int y = (pos.y - dy - 160 - buttonSize * 3) / buttonSize;
                        int i = y * 4 + x;
                        gui.temp = static_cast<Template::types>(i < Template::TemplateCount ? i : 0);
                    };

                    // check special button
                    if (sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 64).contains(pos)) {
                        // resize map
                        world.resizeMap(gui.temp);
                    };
                };
                break;
                default:
                break;
            };
        };
    };

} game;
