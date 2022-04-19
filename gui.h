// -- GUI manager -- //

// encapsulate into <gui>
struct {

    // editor tab
    States::Editor state = States::Ground;
    int cursor = -1;

    // car menu
    bool carMenu = false;

    // outlines
    sf::Uint8 parts = 0;

    // routes
    Way::types route[4] {Way::None};
    sf::Vector2i editorBuf = sf::Vector2i(-1, -1);
    sf::Vector2i originBuf = sf::Vector2i(-1, -1);
    bool autofill = false;

    // road signs
    Sign sign = Sign(sf::Vector2i(0, 0), 0x8, RoadSign::Lock);
    Sign* sobj = NULL;

    // traffic lights
    Light sample = Light(sf::Vector2i(0, 0), 0x8, TrafficLight::Full);
    Light* lobj = NULL;

    // map resize
    Template::types temp = Template::Field;

    // commands
    std::vector<char> cmd;
    size_t cmdcur = 0;

    // render editor GUI
    void renderEdit(Renderer& renderer, sf::Vector2i windowSize, bool eraser) {
        // render cursor
        if (state == States::Outline && cursor == Outline::AutoMark) {
            renderer.push(6, sf::IntRect((world.cursor.x + 0) * world.zoom - world.camera.x + windowSize.x / 2, (world.cursor.y + 0) * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 160, 32, 32), Direction::North);
            renderer.push(6, sf::IntRect((world.cursor.x + 1) * world.zoom - world.camera.x + windowSize.x / 2, (world.cursor.y + 0) * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 160, 32, 32), Direction::West);
            renderer.push(6, sf::IntRect((world.cursor.x + 0) * world.zoom - world.camera.x + windowSize.x / 2, (world.cursor.y + 1) * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 160, 32, 32), Direction::East);
            renderer.push(6, sf::IntRect((world.cursor.x + 1) * world.zoom - world.camera.x + windowSize.x / 2, (world.cursor.y + 1) * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 160, 32, 32), Direction::South);
        } else if (state == States::Route && autofill) {
            if (originBuf.x >= 0)
                renderer.push(6, sf::IntRect(originBuf.x * world.zoom - world.camera.x + windowSize.x / 2, originBuf.y * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(480, 160, 32, 32));
            if (editorBuf.x >= 0)
                renderer.push(6, sf::IntRect(editorBuf.x * world.zoom - world.camera.x + windowSize.x / 2 + world.zoom / 4, editorBuf.y * world.zoom - world.camera.y + windowSize.y / 2 + world.zoom / 4, world.zoom / 2, world.zoom / 2), sf::IntRect(448, 96, 32, 32));
            renderer.push(6, sf::IntRect(world.cursor.x * world.zoom - world.camera.x + windowSize.x / 2, world.cursor.y * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 96, 32, 32));
        } else {
            if (eraser) renderer.push(6, sf::IntRect(world.cursor.x * world.zoom - world.camera.x + windowSize.x / 2 + world.zoom / 4, world.cursor.y * world.zoom - world.camera.y + windowSize.y / 2 + world.zoom / 4, world.zoom / 2, world.zoom / 2), sf::IntRect(448, 96, 32, 32));
            renderer.push(6, sf::IntRect(world.cursor.x * world.zoom - world.camera.x + windowSize.x / 2, world.cursor.y * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 96, 32, 32));
        };

        // palette background
        renderer.push(7, sf::IntRect(windowSize.x - paletteWidth, 0, paletteWidth, windowSize.y), COL2);
        renderer.push(7, sf::IntRect(windowSize.x - paletteWidth - paletteThick, 0, paletteThick, windowSize.y), COL1);
        renderer.push(7, sf::IntRect(windowSize.x - paletteWidth, buttonSize, paletteWidth, labelSize * 3 / 2), COL3);

        // render tab label
        renderer.text(States::strings[state], sf::IntRect(windowSize.x - paletteWidth, buttonSize + 8, paletteWidth, 0), TEXT_LAB, 24);

        // render tab contents
        switch (state) {
            case States::Ground: {
                // render tiles
                for (int i = 0; i < Ground::GroundCount; i++) {
                    int x = windowSize.x - buttonSize * (4 - i % 4);
                    int y = buttonSize * (i / 4) + paletteStart;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Ground::textures[i], sf::Vector2i(32, 32)));
                };

                // render cursor
                if (cursor >= 0 && cursor < Ground::GroundCount) {
                    int x = windowSize.x - buttonSize * (4 - cursor % 4);
                    int y = buttonSize * (cursor / 4) + paletteStart;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));
                };

                // render label
                if (cursor >= 0)
                    renderer.text(Ground::names[cursor], sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);
                else
                    renderer.text(textNone, sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);
            };
            break;
            case States::Outline: {
                // render tiles
                for (int i = 0; i < Outline::OutlineCount; i++) {
                    int x = windowSize.x - buttonSize * (4 - i % 4);
                    int y = buttonSize * (i / 4) + paletteStart;
                    if (i != Outline::None && i != Outline::Auto && i != Outline::AutoMark) {
                        renderer.push(7, sf::IntRect(x, y,                                   buttonSize / 2, buttonSize / 2), sf::IntRect(Outline::textures[i] + sf::Vector2i(0, 16), sf::Vector2i(16, 16)), Direction::North);
                        renderer.push(7, sf::IntRect(x + buttonSize / 2, y,                  buttonSize / 2, buttonSize / 2), sf::IntRect(Outline::textures[i] + sf::Vector2i(0, 16), sf::Vector2i(16, 16)), Direction::West);
                        renderer.push(7, sf::IntRect(x, y + buttonSize / 2,                  buttonSize / 2, buttonSize / 2), sf::IntRect(Outline::textures[i] + sf::Vector2i(0, 16), sf::Vector2i(16, 16)), Direction::East);
                        renderer.push(7, sf::IntRect(x + buttonSize / 2, y + buttonSize / 2, buttonSize / 2, buttonSize / 2), sf::IntRect(Outline::textures[i] + sf::Vector2i(0, 16), sf::Vector2i(16, 16)), Direction::South);
                    } else {
                        renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Outline::textures[i], sf::Vector2i(32, 32)));
                    };
                };

                // render cursor
                if (cursor >= 0 && cursor < Outline::OutlineCount) {
                    int x = windowSize.x - buttonSize * (4 - cursor % 4);
                    int y = buttonSize * (cursor / 4) + paletteStart;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));
                };

                // render label
                if (cursor >= 0)
                    renderer.text(Outline::names[cursor], sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);
                else
                    renderer.text(textNone, sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);

                // render preview
                int x = windowSize.x - paletteWidth / 2 - buttonSize / 2;
                int y = paletteStart + buttonSize * 3;
                renderer.push(7, sf::IntRect(x - paletteThick, y - paletteThick, buttonSize + paletteThick * 2, buttonSize + paletteThick * 2), COL1);
                renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), COL2);

                // get outline
                Outline::types outline = Outline::None;
                if (cursor >= 0) outline = static_cast<Outline::types>(cursor);
                if (outline == Outline::None) outline = Outline::Border;

                // get tile outline
                bool outN  = (this->parts >> 7) & 1;
                bool outW  = (this->parts >> 6) & 1;
                bool outS  = (this->parts >> 5) & 1;
                bool outE  = (this->parts >> 4) & 1;
                bool outNE = (this->parts >> 3) & 1;
                bool outNW = (this->parts >> 2) & 1;
                bool outSE = (this->parts >> 1) & 1;
                bool outSW = (this->parts >> 0) & 1;

                if (outline) {
                    if (outline == Outline::Auto || outline == Outline::AutoMark) {
                        // render auto symbol
                        renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Outline::textures[Outline::Auto], sf::Vector2i(32, 32)));

                    } else if (this->parts) {
                        // get outline textures
                        unsigned char partNE = (outN && outE) ? 2 : (outN ? 0 : (outE ? 1 : (outNE ? 3 : 4)));
                        unsigned char partNW = (outW && outN) ? 2 : (outW ? 0 : (outN ? 1 : (outNW ? 3 : 4)));
                        unsigned char partSW = (outS && outW) ? 2 : (outS ? 0 : (outW ? 1 : (outSW ? 3 : 4)));
                        unsigned char partSE = (outE && outS) ? 2 : (outE ? 0 : (outS ? 1 : (outSE ? 3 : 4)));

                        // calculate offsets
                        int hz = buttonSize / 2;

                        // render outlines
                        if (partNE < 4) {
                            sf::IntRect outlineTex = sf::IntRect((sf::Vector2i)Outline::textures[outline] + sf::Vector2i((partNE & 1) * 16, (partNE >> 1) * 16), sf::Vector2i(16, 16));
                            renderer.push(7, sf::IntRect(x, y, hz, hz), outlineTex, Direction::North);
                        };
                        if (partNW < 4) {
                            sf::IntRect outlineTex = sf::IntRect((sf::Vector2i)Outline::textures[outline] + sf::Vector2i((partNW & 1) * 16, (partNW >> 1) * 16), sf::Vector2i(16, 16));
                            renderer.push(7, sf::IntRect(x + hz, y, hz, hz), outlineTex, Direction::West);
                        };
                        if (partSE < 4) {
                            sf::IntRect outlineTex = sf::IntRect((sf::Vector2i)Outline::textures[outline] + sf::Vector2i((partSE & 1) * 16, (partSE >> 1) * 16), sf::Vector2i(16, 16));
                            renderer.push(7, sf::IntRect(x, y + hz, hz, hz), outlineTex, Direction::East);
                        };
                        if (partSW < 4) {
                            sf::IntRect outlineTex = sf::IntRect((sf::Vector2i)Outline::textures[outline] + sf::Vector2i((partSW & 1) * 16, (partSW >> 1) * 16), sf::Vector2i(16, 16));
                            renderer.push(7, sf::IntRect(x + hz, y + hz, hz, hz), outlineTex, Direction::South);
                        };
                    };
                };

                // render buttons
                int dx = windowSize.x - paletteWidth / 2;
                int dy = paletteStart + buttonSize * 3 / 2;
                bool locked = outline == Outline::Auto || outline == Outline::AutoMark;

                renderer.push(7, sf::IntRect(dx - buttonSize / 4,                                dy + buttonSize - paletteThick - buttonPadding,         buttonSize / 2, buttonSize / 2), BTN_ROUTE[locked ? 2 : outN]);
                renderer.push(7, sf::IntRect(dx + buttonSize / 2 + paletteThick + buttonPadding, dy + buttonSize * 7 / 4,                                buttonSize / 2, buttonSize / 2), BTN_ROUTE[locked ? 2 : outW]);
                renderer.push(7, sf::IntRect(dx - buttonSize / 4,                                dy + buttonSize * 5 / 2 + paletteThick + buttonPadding, buttonSize / 2, buttonSize / 2), BTN_ROUTE[locked ? 2 : outS]);
                renderer.push(7, sf::IntRect(dx - buttonSize - paletteThick - buttonPadding,     dy + buttonSize * 7 / 4,                                buttonSize / 2, buttonSize / 2), BTN_ROUTE[locked ? 2 : outE]);
                renderer.push(7, sf::IntRect(dx - buttonSize - paletteThick - buttonPadding,     dy + buttonSize - paletteThick - buttonPadding,         buttonSize / 2, buttonSize / 2), BTN_ROUTE[locked ? 2 : outNE]);
                renderer.push(7, sf::IntRect(dx + buttonSize / 2 + paletteThick + buttonPadding, dy + buttonSize - paletteThick - buttonPadding,         buttonSize / 2, buttonSize / 2), BTN_ROUTE[locked ? 2 : outNW]);
                renderer.push(7, sf::IntRect(dx - buttonSize - paletteThick - buttonPadding,     dy + buttonSize * 5 / 2 + paletteThick + buttonPadding, buttonSize / 2, buttonSize / 2), BTN_ROUTE[locked ? 2 : outSE]);
                renderer.push(7, sf::IntRect(dx + buttonSize / 2 + paletteThick + buttonPadding, dy + buttonSize * 5 / 2 + paletteThick + buttonPadding, buttonSize / 2, buttonSize / 2), BTN_ROUTE[locked ? 2 : outSW]);

                // render rotate buttons
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 0 * buttonSize, paletteStart + buttonSize * 5, buttonSize, buttonSize), BTN_ROTATE[0]);
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 1 * buttonSize, paletteStart + buttonSize * 5, buttonSize, buttonSize), BTN_ROTATE[2]);
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 2 * buttonSize, paletteStart + buttonSize * 5, buttonSize, buttonSize), BTN_ROTATE[3]);
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 3 * buttonSize, paletteStart + buttonSize * 5, buttonSize, buttonSize), BTN_ROTATE[1]);
            };
            break;
            case States::Deco: {
                // render tiles
                for (int i = 0; i < Decoration::DecorationCount; i++) {
                    int x = windowSize.x - buttonSize * (4 - i % 4);
                    int y = buttonSize * (i / 4) + paletteStart;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Decoration::textures[i], sf::Vector2i(32, 32)));
                };

                // render cursor
                if (cursor >= 0 && cursor < Decoration::DecorationCount) {
                    int x = windowSize.x - buttonSize * (4 - cursor % 4);
                    int y = buttonSize * (cursor / 4) + paletteStart;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));
                };

                // render label
                if (cursor >= 0)
                    renderer.text(Decoration::names[cursor], sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);
                else
                    renderer.text(textNone, sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);
            };
            break;
            case States::Route: {
                // render label
                renderer.text(autofill ? sf::String("Auto") : sf::String("Manual"), sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);

                // render preview
                int x = windowSize.x - paletteWidth / 2 - buttonSize / 2;
                int y = paletteStart + buttonSize;

                // render preview border
                renderer.push(7, sf::IntRect(x - paletteThick, y - paletteThick, buttonSize + paletteThick * 2, buttonSize + paletteThick * 2), COL1);
                renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), COL2);

                // render arrows
                if (autofill) {
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Outline::textures[Outline::Auto], sf::Vector2i(32, 32)));
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Way::textureJunction, sf::Vector2i(32, 32)));
                } else {
                    if (route[0])
                        renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Way::textures[route[0] - 1][world.underground], sf::Vector2i(32, 32)), Direction::North);
                    if (route[1])
                        renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Way::textures[route[1] - 1][world.underground], sf::Vector2i(32, 32)), Direction::West);
                    if (route[2])
                        renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Way::textures[route[2] - 1][world.underground], sf::Vector2i(32, 32)), Direction::South);
                    if (route[3])
                        renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Way::textures[route[3] - 1][world.underground], sf::Vector2i(32, 32)), Direction::East);
                    if (route[0] || route[1] || route[2] || route[3])
                        renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Way::textureJunction, sf::Vector2i(32, 32)));
                };

                // render buttons
                int dx = windowSize.x - paletteWidth / 2;
                int dy = paletteStart + buttonSize * 3 / 2;
                renderer.push(7, sf::IntRect(dx - buttonSize / 4, dy - buttonSize - paletteThick - buttonPadding, buttonSize / 2, buttonSize / 2),     BTN_ROUTE[autofill ? 2 : world.underground]);
                renderer.push(7, sf::IntRect(dx + buttonSize / 2 + paletteThick + buttonPadding, dy - buttonSize / 4, buttonSize / 2, buttonSize / 2), BTN_ROUTE[autofill ? 2 : world.underground]);
                renderer.push(7, sf::IntRect(dx - buttonSize / 4, dy + buttonSize / 2 + paletteThick + buttonPadding, buttonSize / 2, buttonSize / 2), BTN_ROUTE[autofill ? 2 : world.underground]);
                renderer.push(7, sf::IntRect(dx - buttonSize - paletteThick - buttonPadding, dy - buttonSize / 4, buttonSize / 2, buttonSize / 2),     BTN_ROUTE[autofill ? 2 : world.underground]);

                // render rotate buttons
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 0 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize), BTN_ROTATE[0]);
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 3 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize), BTN_ROTATE[1]);

                // render mode buttons
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 1 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize), BTN_ROTATE[2]);
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 2 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                renderer.push(7, sf::IntRect(windowSize.x - paletteWidth + 2 * buttonSize, paletteStart + buttonSize * 3, buttonSize, buttonSize), sf::IntRect(Outline::textures[Outline::Auto], sf::Vector2i(32, 32)));
            };
            break;
            case States::Service: {
                // render tiles
                for (int i = 0; i < Service::ServiceCount; i++) {
                    int x = windowSize.x - buttonSize * (4 - i % 4);
                    int y = buttonSize * (i / 4) + paletteStart;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Service::textures[i], sf::Vector2i(32, 32)));
                };

                // render cursor
                if (cursor >= 0 && cursor < Service::ServiceCount) {
                    int x = windowSize.x - buttonSize * (4 - cursor % 4);
                    int y = buttonSize * (cursor / 4) + paletteStart;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));
                };

                // render label
                if (cursor >= 0)
                    renderer.text(Service::names[cursor], sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);
                else
                    renderer.text(textNone, sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);
            };
            break;
            case States::RoadSign: {
                char text[256];
                int dx = windowSize.x - paletteWidth + 16;
                int dy = paletteStart + 16;

                if (sobj) {
                    // render text
                    renderer.text(sf::String("Object"), sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);

                    // render select
                    renderer.push(6, sf::IntRect(sobj->pos.x * world.zoom - world.camera.x + windowSize.x / 2, sobj->pos.y * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 96, 32, 32));

                    // render types
                    for (int x = 0; x < 4; x++) {
                        renderer.push(7, sf::IntRect(dx - 16 + x * buttonSize, dy + 32, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                        renderer.push(7, sf::IntRect(dx + 8 + x * buttonSize, dy + 32 - 6, buttonSize / 4, buttonSize), sf::IntRect(RoadSign::textures[x], sf::Vector2i(8, 32)));
                    };
                    renderer.push(7, sf::IntRect(dx - 16 + sobj->type * buttonSize, dy + 32, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));
                    renderer.text(sf::String(RoadSign::names[sobj->type]), sf::IntRect(dx - 16, dy - 5, buttonSize * 4, buttonSize / 4), TEXT_USE, 24);

                    // render corners
                    renderer.push(7, sf::IntRect(dx + 0 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sobj->corner >> 3 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 1 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sobj->corner >> 2 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 2 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sobj->corner >> 1 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 3 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sobj->corner >> 0 & 1) ? 0 : 3]);

                    // render speed
                    renderer.push(7, sf::IntRect(dx, dy + 192, 96, 48), COL3);
                    renderer.push(7, sf::IntRect(dx + 96,  dy + 192, 128, 48), COL1);
                    renderer.push(7, sf::IntRect(dx + 100, dy + 196, 120, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 100, dy + 196, sobj->speed * 119 / 250, 40), COL2);

                    // render speed labels
                    renderer.text(sf::String("Speed"), sf::IntRect(dx, dy + 194, 96, 32), TEXT_USE, 24);
                    sprintf(text, "%d", sobj->speed);
                    renderer.text(sf::String(text), sf::IntRect(dx + 96, dy + 194, 128, 32), TEXT_USE, 24);

                    // render position
                    renderer.push(7, sf::IntRect(dx,      dy + 13 * 16 + buttonSize + 0 * 48, 48,  48 * 2), COL3);
                    renderer.push(7, sf::IntRect(dx + 48, dy + 13 * 16 + buttonSize + 0 * 48, 176, 48 * 2), COL1);

                    renderer.text(sf::String("X"), sf::IntRect(dx + 3, dy + 13 * 16 + buttonSize + 0 * 48, 48, 32), TEXT_USE, 24);
                    renderer.text(sf::String("Y"), sf::IntRect(dx + 3, dy + 13 * 16 + buttonSize + 1 * 48, 48, 32), TEXT_USE, 24);
                    sprintf(text, "%d", sobj->pos.x);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 13 * 16 + buttonSize + 0 * 48, 176, 32), TEXT_USE, 24);
                    sprintf(text, "%d", sobj->pos.y);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 13 * 16 + buttonSize + 1 * 48, 176, 32), TEXT_USE, 24);

                    // render delete button
                    renderer.push(7, sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 64), COL1);
                    renderer.push(7, sf::IntRect(dx + 4, windowSize.y - 68, paletteWidth - 40, 56), COL6);
                    renderer.text(sf::String("Delete"), sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 48), TEXT_USE, 24);

                } else {
                    // render text
                    renderer.text(sf::String("Constructor"), sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);

                    // render types
                    for (int x = 0; x < 4; x++) {
                        renderer.push(7, sf::IntRect(dx - 16 + x * buttonSize, dy + 32, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                        renderer.push(7, sf::IntRect(dx + 8 + x * buttonSize, dy + 32 - 6, buttonSize / 4, buttonSize), sf::IntRect(RoadSign::textures[x], sf::Vector2i(8, 32)));
                    };
                    renderer.push(7, sf::IntRect(dx - 16 + sign.type * buttonSize, dy + 32, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));
                    renderer.text(sf::String(RoadSign::names[sign.type]), sf::IntRect(dx - 16, dy - 5, buttonSize * 4, buttonSize / 4), TEXT_USE, 24);

                    // render corners
                    renderer.push(7, sf::IntRect(dx + 0 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sign.corner >> 3 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 1 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sign.corner >> 2 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 2 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sign.corner >> 1 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 3 * buttonSize, dy + 4 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sign.corner >> 0 & 1) ? 0 : 3]);

                    // render speed
                    renderer.push(7, sf::IntRect(dx, dy + 192, 96, 48), COL3);
                    renderer.push(7, sf::IntRect(dx + 96,  dy + 192, 128, 48), COL1);
                    renderer.push(7, sf::IntRect(dx + 100, dy + 196, 120, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 100, dy + 196, sign.speed * 119 / 250, 40), COL2);

                    // render speed labels
                    renderer.text(sf::String("Speed"), sf::IntRect(dx, dy + 194, 96, 32), TEXT_USE, 24);
                    sprintf(text, "%d", sign.speed);
                    renderer.text(sf::String(text), sf::IntRect(dx + 96, dy + 194, 128, 32), TEXT_USE, 24);

                    // render create button
                    renderer.push(7, sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 64), COL1);
                    renderer.push(7, sf::IntRect(dx + 4, windowSize.y - 68, paletteWidth - 40, 56), COL5);
                    renderer.text(sf::String("Create"), sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 48), TEXT_USE, 24);
                };
            };
            break;
            case States::TrafficLight: {
                if (lobj) {
                    // render text
                    renderer.text(sf::String("Object"), sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);

                    // render select
                    renderer.push(6, sf::IntRect(lobj->pos.x * world.zoom - world.camera.x + windowSize.x / 2, lobj->pos.y * world.zoom - world.camera.y + windowSize.y / 2, world.zoom, world.zoom), sf::IntRect(448, 96, 32, 32));

                    // render timers
                    char text[256];
                    int dx = windowSize.x - paletteWidth + 16;
                    int dy = paletteStart + 16;

                    // render back planes
                    renderer.push(7, sf::IntRect(dx,      dy + 0 * 48, 48,  48 * 4), COL3);
                    renderer.push(7, sf::IntRect(dx + 48, dy + 0 * 48, 176, 48 * 4), COL1);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 0 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 0 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 1 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 1 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 2 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 2 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 3 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 3 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 0 * 48 + 4, lobj->red / 60,              40), COL2);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 0 * 48 + 4, lobj->red % 60 * 80 / 59,    40), COL2);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 1 * 48 + 4, lobj->green / 60,            40), COL2);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 1 * 48 + 4, lobj->green % 60 * 80 / 59,  40), COL2);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 2 * 48 + 4, lobj->yellow / 60,           40), COL2);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 2 * 48 + 4, lobj->yellow % 60 * 80 / 59, 40), COL2);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 3 * 48 + 4, lobj->init / 60,             40), COL2);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 3 * 48 + 4, lobj->init % 60 * 80 / 59,   40), COL2);

                    // render light labels
                    renderer.text(sf::String("R"), sf::IntRect(dx + 0, dy + 0 * 48, 48, 32), sf::Color::Red, 24);
                    renderer.text(sf::String("G"), sf::IntRect(dx + 0, dy + 1 * 48, 48, 32), sf::Color::Green, 24);
                    renderer.text(sf::String("Y"), sf::IntRect(dx + 2, dy + 2 * 48, 48, 32), sf::Color::Yellow, 24);
                    renderer.text(sf::String("S"), sf::IntRect(dx + 1, dy + 3 * 48, 48, 32), TEXT_USE, 24);

                    // render ticks
                    sprintf(text, " %d'", lobj->red / 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 0 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d\"", lobj->red % 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 136, dy + 0 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d'", lobj->green / 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 1 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d\"", lobj->green % 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 136, dy + 1 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d'", lobj->yellow / 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 2 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d\"", lobj->yellow % 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 136, dy + 2 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d'", lobj->init / 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 3 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d\"", lobj->init % 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 136, dy + 3 * 48, 88, 32), TEXT_USE, 24);

                    // render types
                    renderer.push(7, sf::IntRect(dx + 16 + 0 * buttonSize, dy + 7 * 32, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                    renderer.push(7, sf::IntRect(dx + 16 + 1 * buttonSize, dy + 7 * 32, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                    renderer.push(7, sf::IntRect(dx + 16 + 2 * buttonSize, dy + 7 * 32, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                    renderer.push(7, sf::IntRect(dx + 40 + 0 * buttonSize, dy + 7 * 32 - 6, buttonSize / 4, buttonSize), sf::IntRect(TrafficLight::textures[0][2], sf::Vector2i(8, 32)));
                    renderer.push(7, sf::IntRect(dx + 40 + 1 * buttonSize, dy + 7 * 32 - 6, buttonSize / 4, buttonSize), sf::IntRect(TrafficLight::textures[1][2], sf::Vector2i(8, 32)));
                    renderer.push(7, sf::IntRect(dx + 40 + 2 * buttonSize, dy + 7 * 32 - 6, buttonSize / 4, buttonSize), sf::IntRect(TrafficLight::textures[2][2], sf::Vector2i(8, 32)));
                    renderer.push(7, sf::IntRect(dx + 16 + lobj->type * buttonSize, dy + 7 * 32, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));

                    // render corners
                    renderer.push(7, sf::IntRect(dx + 0 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(lobj->corner >> 3 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 1 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(lobj->corner >> 2 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 2 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(lobj->corner >> 1 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 3 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(lobj->corner >> 0 & 1) ? 0 : 3]);

                    // render position
                    renderer.push(7, sf::IntRect(dx,      dy + 10 * 32 + buttonSize + 0 * 48, 48,  48 * 2), COL3);
                    renderer.push(7, sf::IntRect(dx + 48, dy + 10 * 32 + buttonSize + 0 * 48, 176, 48 * 2), COL1);

                    renderer.text(sf::String("X"), sf::IntRect(dx + 3, dy + 10 * 32 + buttonSize + 0 * 48, 48, 32), TEXT_USE, 24);
                    renderer.text(sf::String("Y"), sf::IntRect(dx + 3, dy + 10 * 32 + buttonSize + 1 * 48, 48, 32), TEXT_USE, 24);
                    sprintf(text, "%d", lobj->pos.x);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 10 * 32 + buttonSize + 0 * 48, 176, 32), TEXT_USE, 24);
                    sprintf(text, "%d", lobj->pos.y);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 10 * 32 + buttonSize + 1 * 48, 176, 32), TEXT_USE, 24);

                    // render delete button
                    renderer.push(7, sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 64), COL1);
                    renderer.push(7, sf::IntRect(dx + 4, windowSize.y - 68, paletteWidth - 40, 56), COL6);
                    renderer.text(sf::String("Delete"), sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 48), TEXT_USE, 24);

                } else {
                    // render text
                    renderer.text(sf::String("Constructor"), sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);

                    // render timers
                    char text[256];
                    int dx = windowSize.x - paletteWidth + 16;
                    int dy = paletteStart + 16;

                    // render back planes
                    renderer.push(7, sf::IntRect(dx,      dy + 0 * 48, 48,  48 * 4), COL3);
                    renderer.push(7, sf::IntRect(dx + 48, dy + 0 * 48, 176, 48 * 4), COL1);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 0 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 0 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 1 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 1 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 2 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 2 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 3 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 3 * 48 + 4, 80, 40), COL4);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 0 * 48 + 4, sample.red / 60,              40), COL2);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 0 * 48 + 4, sample.red % 60 * 80 / 59,    40), COL2);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 1 * 48 + 4, sample.green / 60,            40), COL2);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 1 * 48 + 4, sample.green % 60 * 80 / 59,  40), COL2);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 2 * 48 + 4, sample.yellow / 60,           40), COL2);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 2 * 48 + 4, sample.yellow % 60 * 80 / 59, 40), COL2);
                    renderer.push(7, sf::IntRect(dx + 52,  dy + 3 * 48 + 4, sample.init / 60,             40), COL2);
                    renderer.push(7, sf::IntRect(dx + 140, dy + 3 * 48 + 4, sample.init % 60 * 80 / 59,   40), COL2);

                    // render light labels
                    renderer.text(sf::String("R"), sf::IntRect(dx + 0, dy + 0 * 48, 48, 32), sf::Color::Red, 24);
                    renderer.text(sf::String("G"), sf::IntRect(dx + 0, dy + 1 * 48, 48, 32), sf::Color::Green, 24);
                    renderer.text(sf::String("Y"), sf::IntRect(dx + 2, dy + 2 * 48, 48, 32), sf::Color::Yellow, 24);
                    renderer.text(sf::String("S"), sf::IntRect(dx + 1, dy + 3 * 48, 48, 32), TEXT_USE, 24);

                    // render ticks
                    sprintf(text, " %d'", sample.red / 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 0 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d\"", sample.red % 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 136, dy + 0 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d'", sample.green / 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 1 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d\"", sample.green % 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 136, dy + 1 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d'", sample.yellow / 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 2 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d\"", sample.yellow % 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 136, dy + 2 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d'", sample.init / 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 3 * 48, 88, 32), TEXT_USE, 24);
                    sprintf(text, " %d\"", sample.init % 60);
                    renderer.text(sf::String(text), sf::IntRect(dx + 136, dy + 3 * 48, 88, 32), TEXT_USE, 24);

                    // render types
                    renderer.push(7, sf::IntRect(dx + 16 + 0 * buttonSize, dy + 7 * 32, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                    renderer.push(7, sf::IntRect(dx + 16 + 1 * buttonSize, dy + 7 * 32, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                    renderer.push(7, sf::IntRect(dx + 16 + 2 * buttonSize, dy + 7 * 32, buttonSize, buttonSize), sf::IntRect(288, 160, 32, 32));
                    renderer.push(7, sf::IntRect(dx + 40 + 0 * buttonSize, dy + 7 * 32 - 6, buttonSize / 4, buttonSize), sf::IntRect(TrafficLight::textures[0][2], sf::Vector2i(8, 32)));
                    renderer.push(7, sf::IntRect(dx + 40 + 1 * buttonSize, dy + 7 * 32 - 6, buttonSize / 4, buttonSize), sf::IntRect(TrafficLight::textures[1][2], sf::Vector2i(8, 32)));
                    renderer.push(7, sf::IntRect(dx + 40 + 2 * buttonSize, dy + 7 * 32 - 6, buttonSize / 4, buttonSize), sf::IntRect(TrafficLight::textures[2][2], sf::Vector2i(8, 32)));
                    renderer.push(7, sf::IntRect(dx + 16 + sample.type * buttonSize, dy + 7 * 32, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));

                    // render corners
                    renderer.push(7, sf::IntRect(dx + 0 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sample.corner >> 3 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 1 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sample.corner >> 2 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 2 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sample.corner >> 1 & 1) ? 0 : 3]);
                    renderer.push(7, sf::IntRect(dx + 3 * buttonSize, dy + 16 * 16 + buttonSize, buttonSize / 2, buttonSize / 2), BTN_ROUTE[(sample.corner >> 0 & 1) ? 0 : 3]);

                    // render create button
                    renderer.push(7, sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 64), COL1);
                    renderer.push(7, sf::IntRect(dx + 4, windowSize.y - 68, paletteWidth - 40, 56), COL5);
                    renderer.text(sf::String("Create"), sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 48), TEXT_USE, 24);
                };
            };
            break;
            case States::MapInfo: {
                char text[256];
                int dx = windowSize.x - paletteWidth + 16;
                int dy = paletteStart + 16;

                // render size
                renderer.push(7, sf::IntRect(dx,      dy, 48,  48 * 2), COL3);
                renderer.push(7, sf::IntRect(dx + 48, dy, 176, 48 * 2), COL1);
                renderer.text(sf::String("W"), sf::IntRect(dx + 2, dy + 0 * 48, 48, 32), TEXT_USE, 24);
                renderer.text(sf::String("H"), sf::IntRect(dx + 1, dy + 1 * 48, 48, 32), TEXT_USE, 24);
                sprintf(text, "%d", world.size.x);
                renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 0 * 48, 176, 32), TEXT_USE, 24);
                sprintf(text, "%d", world.size.y);
                renderer.text(sf::String(text), sf::IntRect(dx + 48, dy + 1 * 48, 176, 32), TEXT_USE, 24);

                // render resize arrows
                renderer.push(7, sf::IntRect(dx + 80, dy + 112 + buttonSize, buttonSize, buttonSize), sf::IntRect(Way::textures[0][1], sf::Vector2i(32, 32)), Direction::North);
                renderer.push(7, sf::IntRect(dx + 80, dy + 112 + buttonSize, buttonSize, buttonSize), sf::IntRect(Way::textures[1][1], sf::Vector2i(32, 32)), Direction::South);
                renderer.push(7, sf::IntRect(dx + 80, dy + 112 + buttonSize, buttonSize, buttonSize), sf::IntRect(Way::textures[0][0], sf::Vector2i(32, 32)), Direction::East);
                renderer.push(7, sf::IntRect(dx + 80, dy + 112 + buttonSize, buttonSize, buttonSize), sf::IntRect(Way::textures[1][0], sf::Vector2i(32, 32)), Direction::West);
                renderer.push(7, sf::IntRect(dx + 80, dy + 112 + buttonSize, buttonSize, buttonSize), sf::IntRect(Way::textureJunction, sf::Vector2i(32, 32)));

                // render resize panes
                renderer.push(7, sf::IntRect(dx + 8,   dy + 112 + buttonSize * 1, buttonSize, buttonSize), COL1);
                renderer.push(7, sf::IntRect(dx + 152, dy + 112 + buttonSize * 1, buttonSize, buttonSize), COL1);
                renderer.push(7, sf::IntRect(dx + 80,  dy + 104 + buttonSize * 0, buttonSize, buttonSize), COL1);
                renderer.push(7, sf::IntRect(dx + 80,  dy + 120 + buttonSize * 2, buttonSize, buttonSize), COL1);
                renderer.push(7, sf::IntRect(dx + 10,  dy + 114 + buttonSize * 1, buttonSize - 4, buttonSize - 4), COL3);
                renderer.push(7, sf::IntRect(dx + 154, dy + 114 + buttonSize * 1, buttonSize - 4, buttonSize - 4), COL3);
                renderer.push(7, sf::IntRect(dx + 82,  dy + 106 + buttonSize * 0, buttonSize - 4, buttonSize - 4), COL3);
                renderer.push(7, sf::IntRect(dx + 82,  dy + 122 + buttonSize * 2, buttonSize - 4, buttonSize - 3), COL3);

                // render resize text
                sprintf(text, "%d", world.resize.left);
                renderer.text(sf::String(text), sf::IntRect(dx + 8,   dy + 107 + buttonSize * 1, buttonSize, buttonSize), TEXT_USE, 20);
                sprintf(text, "%d", world.resize.width);
                renderer.text(sf::String(text), sf::IntRect(dx + 152, dy + 107 + buttonSize * 1, buttonSize, buttonSize), TEXT_USE, 20);
                sprintf(text, "%d", world.resize.top);
                renderer.text(sf::String(text), sf::IntRect(dx + 80,  dy +  99 + buttonSize * 0, buttonSize, buttonSize), TEXT_USE, 20);
                sprintf(text, "%d", world.resize.height);
                renderer.text(sf::String(text), sf::IntRect(dx + 80,  dy + 115 + buttonSize * 2, buttonSize, buttonSize), TEXT_USE, 20);

                // render template label
                renderer.text(sf::String("Templates"), sf::IntRect(dx, dy + 116 + buttonSize * 3, 224, 32), TEXT_LAB, 24);

                // render template previews
                for (int i = 0; i < Template::TemplateCount; i++) {
                    int x = windowSize.x - buttonSize * (4 - i % 4);
                    int y = buttonSize * (i / 4) + dy + 160 + buttonSize * 3;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(Ground::textures[Template::textures[i]], sf::Vector2i(32, 32)));
                };

                // render cursor
                if (temp >= 0 && temp < Template::TemplateCount) {
                    int x = windowSize.x - buttonSize * (4 - temp % 4);
                    int y = buttonSize * (temp / 4) + dy + 160 + buttonSize * 3;
                    renderer.push(7, sf::IntRect(x, y, buttonSize, buttonSize), sf::IntRect(448, 96, 32, 32));
                };

                // render label
                if (temp >= 0)
                    renderer.text(Template::names[temp], sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);
                else
                    renderer.text(textNone, sf::IntRect(windowSize.x - paletteWidth, buttonSize + labelSize / 2 + 8, paletteWidth, 0), TEXT_USE, 24);

                // render create button
                renderer.push(7, sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 64), COL1);
                renderer.push(7, sf::IntRect(dx + 4, windowSize.y - 68, paletteWidth - 40, 56), COL8);
                renderer.text(sf::String("Resize"), sf::IntRect(dx, windowSize.y - 72, paletteWidth - 32, 48), TEXT_USE, 24);
            };
            break;
            default:
            break;
        };
    };

    // render car gui
    void renderCarGUI(Renderer& renderer, sf::Vector2i windowSize) {
        // place car button
        renderer.push(7, sf::IntRect(windowSize.x - buttonSize * 6, 0, buttonSize, buttonSize), BTN_CAR_PLACE);

        // car edit button
        renderer.push(7, sf::IntRect(windowSize.x - buttonSize * 5, 0, buttonSize, buttonSize), BTN_CAR_EDIT);
    };

    // render control buttons
    void renderCtrl(Renderer& renderer, sf::Vector2i windowSize, States::Game wState, bool command) {
        // menu button
        renderer.push(7, sf::IntRect(windowSize.x - buttonSize, 0, buttonSize, buttonSize), BTN_MENU);

        // speed button
        renderer.push(7, sf::IntRect(windowSize.x - buttonSize * 2, 0, buttonSize, buttonSize), BTN_SPEED[world.speed]);

        // level button
        renderer.push(7, sf::IntRect(windowSize.x - buttonSize * 3, 0, buttonSize, buttonSize), BTN_LEVEL[world.underground]);

        // state button
        renderer.push(7, sf::IntRect(windowSize.x - buttonSize * 4, 0, buttonSize, buttonSize), BTN_STATE[wState == States::View]);

        // render command input
        if (command) {
            int dx = 16;
            int dy = windowSize.y - 16 - 48;

            // render command field
            renderer.push(7, sf::IntRect(dx, dy, windowSize.x - 32, 48), COL9);

            // render command text
            char text[1023];
            int off = 1;

            text[0] = '/';
            for (size_t i = 0; i < cmd.size(); i++) {
                if (i == cmdcur) {
                    if (lights.timer % 50 < 25) {
                        text[cmdcur + off] = '|';
                        off++;
                    };
                };
                text[i + off] = cmd[i];
            };
            if (cmdcur == cmd.size()) {
                if (lights.timer % 50 < 25) {
                    text[cmdcur + off] = '|';
                    off++;
                };
            };
            text[cmd.size() + off] = 0;

            renderer.text(sf::String(text), sf::IntRect(dx + 8, dy + 12, 0, 0), TEXT_USE, 20);
        };

        // simulation buttons
        if (wState == States::View) {


            return;
        };

        // editor buttons
        if (wState == States::Edit) {
            // tab buttons
            for (int i = 0; i < 8; i++)
                renderer.push(7, sf::IntRect(windowSize.x - buttonSize * (12 - i) - paletteThick, 0, buttonSize, buttonSize), BTN_MODE[world.underground][i]);

            return;
        };
    };

} gui;
