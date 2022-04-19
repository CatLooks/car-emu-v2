// -- road sign objects -- //

// encapsulate into <signs>
struct {

    // signs container
    std::vector<struct Sign*> objs;

} signs;

// road sign object
struct Sign {
    // state
    RoadSign::types type;
    sf::Vector2i pos;
    sf::Uint8 corner;

    // params
    sf::Uint8 speed;

    // constructors
    Sign (FILE* fp) { load(fp); };
    Sign (sf::Vector2i pos, sf::Uint8 corner, RoadSign::types type) {
        this->corner = corner;
        this->type = type;
        this->pos = pos;

        // default values
        speed = 0;
    };

    // create a copy
    Sign* copy() {
        Sign* ptr = new Sign(pos, corner, type);
        signs.objs.push_back(ptr);

        ptr->speed = speed;
        return ptr;
    };

    // check for lock
    bool lock() {
        // check for null pointer
        if (this == NULL) return false;

        // check for locking sign
        return type == RoadSign::Lock;
    };

    // load from file
    void load(FILE* fp) {
        // load bytes
        sf::Uint8 bytes[6];
        for (int i = 0; i < 6; i++)
            bytes[i] = fgetc(fp);

        // load attribs
        pos.x = bytes[0] | bytes[1] << 8;
        pos.y = bytes[2] | bytes[3] << 8;
        corner = bytes[4] & 0xF;
        type = static_cast<RoadSign::types>(bytes[4] >> 4);
        speed = bytes[5];
    };

    // dump to file
    void dump(FILE* fp) {
        // load bytes
        sf::Uint8 bytes[6] {
            sf::Uint8(pos.x & 0xFF), sf::Uint8(pos.x >> 8 & 0xFF),
            sf::Uint8(pos.y & 0xFF), sf::Uint8(pos.y >> 8 & 0xFF),
            sf::Uint8(corner | type << 4), speed
        };

        // dump bytes
        for (int i = 0; i < 6; i++)
            fputc(bytes[i], fp);
    };

    // render sign
    void render(Renderer& renderer, sf::Vector2i view, int zoom, bool center) {
        // render center
        if (center)
            renderer.push(4, sf::IntRect(pos.x * zoom - view.x, pos.y * zoom - view.y, zoom, zoom), sf::IntRect(480, 160, 32, 32));

        // render lights
        for (int i = 0; i < 4; i++) {
            if ((corner >> (i ^ 3) & 1) == 0) continue;

            // get position
            float dx = pos.x + TrafficLight::offsets[i].x;
            float dy = pos.y + TrafficLight::offsets[i].y;
            sf::IntRect bounds(dx * zoom - view.x, dy * zoom - view.y, zoom / 4, zoom);

            // render object
            renderer.push(4, bounds, sf::IntRect(RoadSign::textures[type], sf::Vector2i(8, 32)));
        };
    };
};
