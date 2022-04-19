// -- traffic light objects -- //

// encapsulate into <lights>
struct light_mgr_t {

    // lights container
    std::vector<struct Light*> objs;

    // light timer
    unsigned int timer = 0;

    // update lights
    void update() {
        timer++;
    };

} lights;

// traffic light object
struct Light {

    // state
    TrafficLight::types type;
    sf::Vector2i pos;
    sf::Uint8 corner;

    // type
    unsigned int red;
    unsigned int green;
    unsigned int yellow;
    unsigned int init;

    // constructors
    Light (FILE* fp) { load(fp); };
    Light (sf::Vector2i pos, sf::Uint8 corner, TrafficLight::types type) {
        this->corner = corner;
        this->pos = pos;
        this->type = type;

        // default timers
        yellow = 60;
        green = 600;
        red = 1200;
        init = 0;
    };

    // create a copy
    Light* copy() {
        Light* ptr = new Light(pos, corner, type);
        lights.objs.push_back(ptr);

        ptr->red = red;
        ptr->green = green;
        ptr->yellow = yellow;
        return ptr;
    };

    // load from stream
    void load(FILE* fp) {
        sf::Uint8 bytes[14];
        for (int i = 0; i < 14; i++)
            bytes[i] = fgetc(fp);

        type = static_cast<TrafficLight::types>(bytes[0]);
        corner = bytes[1] & 0xF;
        pos.x = bytes[2] | bytes[3] << 8;
        pos.y = bytes[4] | bytes[5] << 8;
        red = bytes[6] | bytes[7] << 8;
        yellow = bytes[8] | bytes[9] << 8;
        green = bytes[10] | bytes[11] << 8;
        init = bytes[12] | bytes[13] << 8;
    };

    // dump to stream
    void dump(FILE* fp) {
        sf::Uint8 bytes[14] {
            type, corner, sf::Uint8(pos.x & 0xFF), sf::Uint8(pos.x >> 8), sf::Uint8(pos.y & 0xFF), sf::Uint8(pos.y >> 8),
            sf::Uint8(red & 0xFF), sf::Uint8(red >> 8), sf::Uint8(yellow & 0xFF), sf::Uint8(yellow >> 8),
            sf::Uint8(green & 0xFF), sf::Uint8(green >> 8), sf::Uint8(init & 0xFF), sf::Uint8(init >> 8)
        };
        for (int i = 0; i < 14; i++)
            fputc(bytes[i], fp);
    };

    // get lock
    bool lock(unsigned int timer) {
        // check for null pointer
        if (this == NULL) return false;

        // check for invalid settings
        if (red + green == 0) return false;

        // return locking
        return (timer % (red + green) + init) % (red + green) < red;
    };

    // get state
    TrafficLight::States state(unsigned int timer) {
        // check for invalid settings
        if (red + green == 0) return TrafficLight::Off;

        // get local cycle time
        unsigned int local = (timer % (red + green) + init) % (red + green);

        // return color
        if (local >= red) return TrafficLight::Green;
        if (local >= yellow && local < red - yellow) return TrafficLight::Red;
        return TrafficLight::Yellow;
    };

    // render light
    void render(Renderer& renderer, sf::Vector2i view, int zoom, unsigned int timer, bool center) {
        // get color
        TrafficLight::States id = state(timer);

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
            renderer.push(4, bounds, sf::IntRect(TrafficLight::textures[type][id], sf::Vector2i(8, 32)));
        };
    };

};
