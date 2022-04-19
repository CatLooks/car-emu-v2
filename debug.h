// -- debug info -- //

// message buffer
std::vector<Triplet<char*, sf::Color, int>> msgBuffer;

// command parser
void executeCommand(std::vector<char>& cmd) {
    // string buffer
    std::vector<char> buf;
    char text[256];
    int args[3];
    int id = 0;

    // parse command
    cmd.push_back(' ');
    for (char c : cmd) {
        // create string
        if (c == ' ') {
            // parse string
            buf.push_back(0);
            char* str = buf.data();

            // compare string
            switch (id++) {
                case 0: {
                    // check variable
                    if (!strcmpi(str, "minute")) {
                        args[1] = 0;
                        break;
                    };
                    if (!strcmpi(str, "hour")) {
                        args[1] = 1;
                        break;
                    };
                    if (!strcmpi(str, "day")) {
                        args[1] = 2;
                        break;
                    };
                    if (!strcmpi(str, "weather")) {
                        args[1] = 3;
                        break;
                    };
                    if (!strcmpi(str, "time")) {
                        args[1] = 4;
                        break;
                    };

                    // unknown variable
                    sprintf(text, "unknown variable: %s", str);
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_INV, 180));
                    return;
                };
                break;
                case 1: {
                    // check operation
                    if (!strcmpi(str, "query")) {
                        args[0] = 0;
                        break;
                    };
                    if (!strcmpi(str, "set")) {
                        args[0] = 1;
                        break;
                    };
                    if (!strcmpi(str, "add")) {
                        args[0] = 2;
                        break;
                    };

                    // unknown variable
                    sprintf(text, "unknown operation: %s", str);
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_INV, 180));
                    return;
                };
                break;
                case 2: {
                    // check value
                    if (!strcmpi(str, "clear")) {
                        args[2] = 0;
                        break;
                    };
                    if (!strcmpi(str, "clouds")) {
                        args[2] = 1;
                        break;
                    };
                    if (!strcmpi(str, "rain")) {
                        args[2] = 2;
                        break;
                    };
                    if (!strcmpi(str, "thunder")) {
                        args[2] = 3;
                        break;
                    };

                    // parse as number
                    bool shift = true;
                    int num = 0;
                    for (size_t i = 0; i < strlen(str); i++) {
                        if (str[i] >= '0' && str[i] <= '9') {
                            if (shift) num *= 10;
                            num += str[i] - '0';
                            shift = true;
                        } else if (str[i] == 'd') {
                            num--;
                            num *= 24;
                            shift = false;
                        } else if (str[i] == 'h' || str[i] == 'm' || str[i] == ':') {
                            num *= 60;
                            shift = false;
                        } else {
                            // unknown number
                            sprintf(text, "unknown number: '%c'", str[i]);
                            msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_INV, 180));
                            return;
                        };
                    };
                    args[2] = num;
                };
                break;
            };

            // clear buffer
            buf.clear();
        } else {
            buf.push_back(c);
        };
    };

    // weather names
    const char* weatherNames[4] {
        "Clear", "Clouds", "Rain", "Thunder"
    };

    // execute arguments
    switch (args[0]) {
        case 0: {
            switch (args[1]) {
                case 0: {
                    sprintf(text, "Minute: %d", lights.timer / 60 % 60);
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_USE, 180));
                };
                break;
                case 1: {
                    sprintf(text, "Hour: %d", lights.timer / 3600 % 24);
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_USE, 180));
                };
                break;
                case 2: {
                    sprintf(text, "Day: %d", lights.timer / 86400 + 1);
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_USE, 180));
                };
                break;
                case 3: {
                    sprintf(text, "Weather: %s", weatherNames[0]);
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_USE, 180));
                };
                break;
                case 4: {
                    sprintf(text, "Time: Day %d - %02d:%02d", lights.timer / 86400 + 1, lights.timer / 3600 % 24, lights.timer / 60 % 60);
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_USE, 180));
                };
                break;
            };
        };
        break;
        case 1: {
            switch (args[1]) {
                case 0: {
                    lights.timer = lights.timer + args[2] * 60 - (lights.timer / 60 % 60) * 60;
                };
                break;
                case 1: {
                    lights.timer = lights.timer + args[2] * 3600 - (lights.timer / 3600 % 24) * 3600;
                };
                break;
                case 2: {
                    lights.timer = lights.timer + (args[2] - 1) * 86400 - (lights.timer / 86400) * 86400;
                };
                break;
                case 3: {

                };
                break;
                case 4: {
                    lights.timer = args[2];
                };
            };
        };
        break;
        case 2: {
            switch (args[1]) {
                case 0: {
                    lights.timer += args[2] * 60;
                };
                break;
                case 1: {
                    lights.timer += args[2] * 3600;
                };
                break;
                case 2: {
                    lights.timer += (args[2] - 1) * 86400;
                };
                break;
                case 3: {
                    sprintf(text, "cannot add to variable: weather");
                    msgBuffer.push_back(Triplet<char*, sf::Color, int>(copyString(text), TEXT_INV, 180));
                    return;
                };
                break;
                case 4: {
                    lights.timer += args[2];
                };
            };
        };
        break;
    };
};

// print message buffer
void debugMessage(Renderer& renderer, sf::Vector2i windowSize) {
    int y = 80;

    // render each text
    for (int i = msgBuffer.size() - 1; i >= 0; i--) {
        Triplet<char*, sf::Color, int>& t = msgBuffer[i];
        if (t.c >= 60) {
            // stable display
            renderer.text(sf::String(t.a), sf::IntRect(0, y, windowSize.x, 0), t.b, 22);
            y -= 22;

            // decrement timer
            --t.c;
        } else {
            // stable display
            renderer.text(sf::String(t.a), sf::IntRect(0, y, windowSize.x, 0), sf::Color(t.b.r, t.b.g, t.b.b, t.c * 255 / 60), 22);
            y -= 22;

            // decrement timer
            if (--t.c == 0) {
                msgBuffer.erase(msgBuffer.begin());
            };
        };
    };
};

// print tile info
void debugTile(Renderer& renderer, Tile* tile, sf::Vector2i pos, bool debug) {
    int top = 8;
    char text[256];

    // print global timer
    int time_d = lights.timer / 86400 + 1;
    int time_h = lights.timer / 3600 % 24;
    int time_m = lights.timer / 60 % 60;

    sprintf(text, "Day %d - %02d:%02d", time_d, time_h, time_m);
    renderer.text(sf::String(text), sf::IntRect(8, top, 132, 0), TEXT_USE, 16);
    top += 22;

    // global timer box
    renderer.push(7, sf::IntRect(0, 0, 184, 40), COL1);
    renderer.push(7, sf::IntRect(4, 4, 144, 32), COL2);

    // weather icon
    renderer.push(7, sf::IntRect(148, 4, 32, 32), COL3);
    renderer.push(7, sf::IntRect(148, 4, 32, 32), TEX_WEATHER[time_h < 6 || time_h >= 21][0]);

    // check for debug info
    if (!debug) return;
    top += 22;

    // print position
    sprintf(text, "Position: %d %d", pos.x, pos.y);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;

    // check for tile
    if (!tile) return;

    // print ground
    sprintf(text, "Ground Floor: %s", Ground::names[tile->groundType]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;
    sprintf(text, "Tunnel Floor: %s", Ground::names[tile->tunnelType]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;

    // print outline
    sprintf(text, "Ground Outline: %s", Outline::names[tile->groundOut]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;
    sprintf(text, "Tunnel Outline: %s", Outline::names[tile->tunnelOut]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;

    // print decoration
    sprintf(text, "Ground Deco: %s", Decoration::names[tile->groundDeco]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;
    sprintf(text, "Tunnel Deco: %s", Decoration::names[tile->tunnelDeco]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;

    // print service
    sprintf(text, "Service: %s", Service::names[tile->service]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;

    // print pathfind buffer
    sprintf(text, "GPFB: %d", tile->pfb[0]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 22;
    sprintf(text, "TPFB: %d", tile->pfb[1]);
    renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
    top += 26;

    // print occupance
    if (tile->occ) {
        renderer.text(sf::String("- Occupied"), sf::IntRect(8, top, 0, 0), TEXT_INV, 16);
        top += 22;
    };

    // print sign
    if (tile->sign) {
        renderer.text(sf::String("- Road Sign"), sf::IntRect(8, top, 0, 0), TEXT_LAB, 16);
        top += 22;

        // print sign type
        sprintf(text, "Type: %s", RoadSign::names[tile->sign->type]);
        renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), tile->sign->type ? TEXT_USE : TEXT_INV, 16);
        top += 22;

        // print sign speed
        if (tile->sign->type == RoadSign::Min || tile->sign->type == RoadSign::Max) {
            sprintf(text, "%s: %d", RoadSign::names[tile->sign->type], tile->sign->speed);
            renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_FLG, 16);
            top += 22;
        };
    };

    // print light
    if (tile->light) {
        renderer.text(sf::String("- Traffic Light"), sf::IntRect(8, top, 0, 0), TEXT_LAB, 16);
        top += 22;

        // print light lock
        if (tile->light->lock(lights.timer))
            renderer.text(sf::String("- Locked"), sf::IntRect(8, top, 0, 0), TEXT_INV, 16);
        else
            renderer.text(sf::String("- Unlocked"), sf::IntRect(8, top, 0, 0), TEXT_FLG, 16);
        top += 22;
    };

    // print car
    if (tile->car) {
        renderer.text(sf::String("- Car"), sf::IntRect(8, top, 0, 0), TEXT_LAB, 16);
        top += 22;

        // print car type
        sprintf(text, "ID: %08X", tile->car->idc);
        renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
        top += 22;

        // print car type
        sprintf(text, "Type: %s", Cars::names[tile->car->type]);
        renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
        top += 22;

        // print car speed
        sprintf(text, "Speed: %d", tile->car->speed);
        renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
        top += 22;

        // print car position
        sprintf(text, "TPos: %d %d", tile->car->tp.x, tile->car->tp.y);
        renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
        top += 22;
        sprintf(text, "FPos: %d %d", tile->car->fp.x, tile->car->fp.y);
        renderer.text(sf::String(text), sf::IntRect(8, top, 0, 0), TEXT_USE, 16);
        top += 22;
    };
};
