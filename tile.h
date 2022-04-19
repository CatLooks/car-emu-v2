// -- tile objects -- //

// flower chance
const int FLOWER_CHANCE = 13;

// tile object
struct Tile {
    // visual elements
    Ground::types groundType;
    Ground::types tunnelType;
    Outline::types groundOut;
    Outline::types tunnelOut;
    Decoration::types groundDeco;
    Decoration::types tunnelDeco;
    sf::Uint8 groundParts;
    sf::Uint8 tunnelParts;

    // logistic elements
    struct Sign* sign;
    Way::types groundWay[4];
    Way::types tunnelWay[4];
    Service::types service;
    struct Light* light;

    // car buffers
    struct Car* car; // car reference
    struct Car* occ; // car occupier
    int pfb[2]; // path finding buffer

    // default values
    void reset(Template::types tmp) {
        // reset visual elements
        groundParts = 0;
        tunnelParts = 0;
        groundOut = Outline::None;
        tunnelOut = Outline::None;

        switch (tmp) {
            case Template::Field: {
                groundType = rand() % FLOWER_CHANCE ? Ground::Grass : Ground::Flowers;
                tunnelType = Ground::Void;
                groundDeco = Decoration::None;
                tunnelDeco = Decoration::None;
            };
            break;
            case Template::Sea: {
                groundType = Ground::Water;
                tunnelType = Ground::Void;
                groundDeco = Decoration::None;
                tunnelDeco = Decoration::None;
            };
            break;
            case Template::Desert: {
                groundType = Ground::Sand;
                tunnelType = Ground::Void;
                groundDeco = Decoration::None;
                tunnelDeco = Decoration::None;
            };
            break;
            default: {
                groundType = Ground::Void;
                tunnelType = Ground::Void;
                groundDeco = Decoration::None;
                tunnelDeco = Decoration::None;
            };
            break;
        };

        // reset logistic elements
        for (int i = 0; i < 4; i++) {
            groundWay[i] = Way::None;
            tunnelWay[i] = Way::None;
        };
        service = Service::None;
        light = NULL;
        sign = NULL;
        car = NULL;
        occ = NULL;
        pfb[0] = 0;
        pfb[1] = 0;
    };

    // load tile
    void load(FILE* fp) {
        // load bytes
        sf::Uint8 bytes[11];
        light = NULL;
        sign = NULL;
        car = NULL;
        occ = NULL;
        pfb[0] = 0;
        pfb[1] = 0;

        for (int i = 0; i < 11; i++)
            bytes[i] = fgetc(fp);

        // convert bytes to data
        groundType = static_cast<Ground::types>(bytes[0]);
        tunnelType = static_cast<Ground::types>(bytes[1]);

        groundOut = static_cast<Outline::types>(bytes[2]);
        tunnelOut = static_cast<Outline::types>(bytes[3]);
        groundParts = bytes[4];
        tunnelParts = bytes[5];
        groundDeco = static_cast<Decoration::types>(bytes[6]);
        tunnelDeco = static_cast<Decoration::types>(bytes[7]);

        service = static_cast<Service::types>(bytes[8] & 31);

        groundWay[0] = static_cast<Way::types>(bytes[9] >> 6 & 3);
        groundWay[1] = static_cast<Way::types>(bytes[9] >> 4 & 3);
        groundWay[2] = static_cast<Way::types>(bytes[9] >> 2 & 3);
        groundWay[3] = static_cast<Way::types>(bytes[9] & 3);
        tunnelWay[0] = static_cast<Way::types>(bytes[10] >> 6 & 3);
        tunnelWay[1] = static_cast<Way::types>(bytes[10] >> 4 & 3);
        tunnelWay[2] = static_cast<Way::types>(bytes[10] >> 2 & 3);
        tunnelWay[3] = static_cast<Way::types>(bytes[10] & 3);
    };

    // dump tile
    void dump(FILE* fp) {
        // generate bytes
        sf::Uint8 bytes[11] {
            groundType, tunnelType, groundOut, tunnelOut, groundParts, tunnelParts, groundDeco, tunnelDeco, service,
            sf::Uint8(groundWay[0] << 6 | groundWay[1] << 4 | groundWay[2] << 2 | groundWay[3]),
            sf::Uint8(tunnelWay[0] << 6 | tunnelWay[1] << 4 | tunnelWay[2] << 2 | tunnelWay[3])
        };

        // dump bytes
        for (int i = 0; i < 11; i++)
            fputc(bytes[i], fp);
    };

    // fetch variadic attributes
    inline Ground::types&  ground (bool u) { return u ? tunnelType  : groundType;  };
    inline Outline::types& outline(bool u) { return u ? tunnelOut   : groundOut;   };
    inline Decoration::types& deco(bool u) { return u ? tunnelDeco  : groundDeco;  };
    inline sf::Uint8&      parts  (bool u) { return u ? tunnelParts : groundParts; };
    inline Way::types*     routes (bool u) { return u ? tunnelWay   : groundWay;   };

    // render tile attributes
    void renderAttribs(Renderer& renderer, sf::IntRect& pos, bool underground, int layer, sf::Vector2i view, int zoom, int tx, int ty, int x, int y, bool routes, bool services, bool rsc, bool tlc, unsigned int timer) {
        // variadic attributes
        Ground::types&      gnd = this->ground(underground);
        Outline::types      out = this->outline(underground);
        Decoration::types& deco = this->deco(underground);
        sf::Uint8&          pts = this->parts(underground);
        Way::types*         way = this->routes(underground);

        // render ground
        switch (gnd) {
            case Ground::Void:
            break;
            case Ground::Water:
            renderer.push(layer, pos, waterAnimation[animFrame & 7]);
            break;
            case Ground::Grass:
            renderer.push(layer, pos, grassAnimation[animFrame & 3]);
            break;
            case Ground::Flowers:
            renderer.push(layer, pos, flowerAnimation[animFrame & 3]);
            break;
            default:
            renderer.push(layer, pos, sf::IntRect(Ground::textures[gnd], sf::Vector2i(32, 32)));
        };

        // render outlines
        if (out && pts) {
            // get tile outline
            bool outN  = (pts >> 7) & 1;
            bool outW  = (pts >> 6) & 1;
            bool outS  = (pts >> 5) & 1;
            bool outE  = (pts >> 4) & 1;
            bool outNE = (pts >> 3) & 1;
            bool outNW = (pts >> 2) & 1;
            bool outSE = (pts >> 1) & 1;
            bool outSW = (pts >> 0) & 1;

            // get outline textures
            unsigned char partNE = (outN && outE) ? 2 : (outN ? 0 : (outE ? 1 : (outNE ? 3 : 4)));
            unsigned char partNW = (outW && outN) ? 2 : (outW ? 0 : (outN ? 1 : (outNW ? 3 : 4)));
            unsigned char partSW = (outS && outW) ? 2 : (outS ? 0 : (outW ? 1 : (outSW ? 3 : 4)));
            unsigned char partSE = (outE && outS) ? 2 : (outE ? 0 : (outS ? 1 : (outSE ? 3 : 4)));

            // calculate offsets
            int offx = view.x % zoom;
            int offy = view.y % zoom;
            int hz = zoom / 2;

            // render outlines
            if (partNE < 4) {
                sf::IntRect outlineTex = sf::IntRect((sf::Vector2i)Outline::textures[out] + sf::Vector2i((partNE & 1) * 16, (partNE >> 1) * 16), sf::Vector2i(16, 16));
                renderer.push(layer, sf::IntRect(x * zoom - offx, y * zoom - offy, hz, hz), outlineTex, Direction::North);
            };
            if (partNW < 4) {
                sf::IntRect outlineTex = sf::IntRect((sf::Vector2i)Outline::textures[out] + sf::Vector2i((partNW & 1) * 16, (partNW >> 1) * 16), sf::Vector2i(16, 16));
                renderer.push(layer, sf::IntRect(x * zoom - offx + hz, y * zoom - offy, hz, hz), outlineTex, Direction::West);
            };
            if (partSE < 4) {
                sf::IntRect outlineTex = sf::IntRect((sf::Vector2i)Outline::textures[out] + sf::Vector2i((partSE & 1) * 16, (partSE >> 1) * 16), sf::Vector2i(16, 16));
                renderer.push(layer, sf::IntRect(x * zoom - offx, y * zoom - offy + hz, hz, hz), outlineTex, Direction::East);
            };
            if (partSW < 4) {
                sf::IntRect outlineTex = sf::IntRect((sf::Vector2i)Outline::textures[out] + sf::Vector2i((partSW & 1) * 16, (partSW >> 1) * 16), sf::Vector2i(16, 16));
                renderer.push(layer, sf::IntRect(x * zoom - offx + hz, y * zoom - offy + hz, hz, hz), outlineTex, Direction::South);
            };
        };

        // render decorations
        if (deco) {
            switch (Decoration::rotation[deco]) {
                case 0:
                renderer.push(layer, pos, sf::IntRect(Decoration::textures[deco], sf::Vector2i(32, 32)));
                break;
                case 1:
                renderer.push(layer, pos, sf::IntRect(Decoration::textures[deco], sf::Vector2i(32, 32)), static_cast<Direction>((tx ^ ty) & 3));
                break;
                default:
                renderer.push(layer, pos, sf::IntRect(Decoration::textures[deco], sf::Vector2i(32, 32)), ((tx ^ ty) & 1) ? Direction::South : Direction::North);
                break;
            };
        };

        // render routes
        if (routes && (underground || ((!this->light->lock(timer) && !this->sign->lock()) || timer % 60 < 30))) {
            if (way[0])
                renderer.push(layer, pos, sf::IntRect(Way::textures[way[0] - 1][underground], sf::Vector2i(32, 32)), Direction::North);
            if (way[1])
                renderer.push(layer, pos, sf::IntRect(Way::textures[way[1] - 1][underground], sf::Vector2i(32, 32)), Direction::West);
            if (way[2])
                renderer.push(layer, pos, sf::IntRect(Way::textures[way[2] - 1][underground], sf::Vector2i(32, 32)), Direction::South);
            if (way[3])
                renderer.push(layer, pos, sf::IntRect(Way::textures[way[3] - 1][underground], sf::Vector2i(32, 32)), Direction::East);
            if (way[0] || way[1] || way[2] || way[3])
                renderer.push(layer, pos, sf::IntRect(Way::textureJunction, sf::Vector2i(32, 32)));
        };

        // render ground level objects
        if (!underground) {
            // render services
            if (services && this->service) {
                renderer.push(layer, pos, sf::IntRect(Service::textures[this->service], sf::Vector2i(32, 32)));
            };

            // render signs
            if (this->sign)
                this->sign->render(renderer, view, zoom, rsc);

            // render lights
            if (this->light)
                this->light->render(renderer, view, zoom, timer, tlc);
        };
    };

    // render tile
    void render(Renderer& renderer, sf::IntRect& pos, sf::Vector2i& view, int x, int y, int tx, int ty, int zoom, bool underground, bool editor, bool routes, bool services, bool rsc, bool tlc, unsigned int timer) {
        // get tile layer
        unsigned int layer = (this->tunnelType ? (Ground::ascend[this->groundType] + 1) : 0) * 2;

        // render tile
        if (underground) {
            if (this->tunnelType)
                renderAttribs(renderer, pos, true, 0, view, zoom, tx, ty, x, y, routes, services, rsc, tlc, timer);
            else {
                renderAttribs(renderer, pos, false, 0, view, zoom, tx, ty, x, y, routes, services, rsc, tlc, timer);
                if (editor) renderer.push(7, pos, COL9);
            };
        } else {
            if (this->groundType)
                renderAttribs(renderer, pos, false, layer, view, zoom, tx, ty, x, y, routes, services, rsc, tlc, timer);
            else {
                renderAttribs(renderer, pos, true, layer, view, zoom, tx, ty, x, y, routes, services, rsc, tlc, timer);
                if (editor) renderer.push(7, pos, COL9);
            };
        };
    };
};
