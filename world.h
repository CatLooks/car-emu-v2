// -- world map manager -- //

// encapsulate into <world>
struct World {

    // tile collection
    sf::Vector2i size;
    Tile* tiles;

    // view settings
    sf::Vector2i camera;
    sf::Vector2i cursor;
    bool underground;
    struct Car* control;
    int frame;
    int speed;
    int zoom;

    // map info
    sf::IntRect resize;
    char name[17];

    // constructor
    World () {
        sprintf(name, "Sample Text");

        control = NULL;
        tiles = NULL;

        underground = false;
        zoom = 64;
        speed = 0;
        frame = 0;
    };

    // destructor
    ~World () {
        if (tiles) delete[] tiles;
    };

    // access tile
    Tile* getTile(int x, int y) {
        if (x < 0 || y < 0 || x >= size.x || y >= size.y)
            return NULL;
        return tiles + y * size.x + x;
    };
    Tile* getTile(sf::Vector2i pos) {
        if (pos.x < 0 || pos.y < 0 || pos.x >= size.x || pos.y >= size.y)
            return NULL;
        return tiles + pos.y * size.x + pos.x;
    };
    Tile* getTileS(Vector2s pos) {
        if (pos.x < 0 || pos.y < 0 || pos.x >= size.x || pos.y >= size.y)
            return NULL;
        return tiles + pos.y * size.x + pos.x;
    };

    // generate world template
    void generateTemplate(sf::Vector2i size, Template::types tmp) {
        this->size = size;
        resize.width  = size.x;
        resize.height = size.y;

        // generate tiles
        if (tiles) delete tiles;
        tiles = new Tile[size.x * size.y];

        // reset tiles
        for (int i = 0; i < size.x * size.y; i++) {
            tiles[i].reset(tmp);
        };

        // center camera
        camera = size * zoom / 2;
    };

    // create sign
    Sign* createSign(sf::Vector2i pos, Sign& sample) {
        int dx = 0;
        while (true) {
            Tile* tile = getTile(pos + sf::Vector2i(dx, 0));
            if (tile == NULL) return NULL;
            if (tile->sign) {
                dx++;
                continue;
            };

            tile->sign = sample.copy();
            tile->sign->pos = pos + sf::Vector2i(dx, 0);
            return tile->sign;
        };
    };

    // delete sign
    void deleteSign(Sign* sign) {
        int id = 0;

        for (Sign* sptr : signs.objs) {
            if (sptr == sign) {
                signs.objs.erase(signs.objs.begin() + id);
                removeSign(sign);
                delete sign;
                return;
            };
            id++;
        };
    };

    // place sign
    bool placeSign(Sign* sign) {
        if (sign == NULL) return true;
        Tile* tile = getTile(sign->pos);
        if (tile == NULL) return true;
        if (tile->sign) return false;
        tile->sign = sign;
        return true;
    };

    // remove sign
    void removeSign(Sign* sign) {
        if (sign == NULL) return;
        Tile* tile = getTile(sign->pos);
        if (tile == NULL) return;
        tile->sign = NULL;
    };

    // create light
    Light* createLight(sf::Vector2i pos, Light& sample) {
        int dx = 0;
        while (true) {
            Tile* tile = getTile(pos + sf::Vector2i(dx, 0));
            if (tile == NULL) return NULL;
            if (tile->light) {
                dx++;
                continue;
            };

            tile->light = sample.copy();
            tile->light->pos = pos + sf::Vector2i(dx, 0);
            return tile->light;
        };
    };

    // delete light
    void deleteLight(Light* light) {
        int id = 0;

        for (Light* lptr : lights.objs) {
            if (lptr == light) {
                lights.objs.erase(lights.objs.begin() + id);
                removeLight(light);
                delete light;
                return;
            };
            id++;
        };
    };

    // place light
    bool placeLight(Light* light) {
        if (light == NULL) return true;
        Tile* tile = getTile(light->pos);
        if (tile == NULL) return true;
        if (tile->light) return false;
        tile->light = light;
        return true;
    };

    // remove light
    void removeLight(Light* light) {
        if (light == NULL) return;
        Tile* tile = getTile(light->pos);
        if (tile == NULL) return;
        tile->light = NULL;
    };

    // resize world
    void resizeMap(Template::types temp) {
        // create new tile buffer
        Tile* ntiles = new Tile[resize.width * resize.height];

        // copy tiles
        for (int y = 0; y < resize.height; y++) {
            for (int x = 0; x < resize.width; x++) {
                // fetch old tile
                Tile* tile = getTile(x + resize.left, y + resize.top);
                if (tile) {
                    // copy old tile
                    memcpy(ntiles + y * resize.width + x, tile, sizeof(Tile));
                    ntiles[y * resize.width + x].car = NULL;
                    ntiles[y * resize.width + x].occ = NULL;
                } else {
                    // generate new tile
                    ntiles[y * resize.width + x].reset(temp);
                };
            };
        };

        // swap tile buffers
        if (tiles) delete[] tiles;
        tiles = ntiles;

        // delete erased road signs
        for (int i = signs.objs.size() - 1; i >= 0; i--) {
            // check position
            if (!resize.contains(signs.objs[i]->pos))
                signs.objs.erase(signs.objs.begin() + i);

            // reposition
            signs.objs[i]->pos -= sf::Vector2i(resize.left, resize.top);
        };

        // delete erased traffic lights
        for (int i = lights.objs.size() - 1; i >= 0; i--) {
            // check position
            if (!resize.contains(lights.objs[i]->pos))
                lights.objs.erase(lights.objs.begin() + i);

            // reposition
            lights.objs[i]->pos -= sf::Vector2i(resize.left, resize.top);
        };

        // delete all cars
        cars.objs.clear();
        control = NULL;

        // reset sizes
        size = sf::Vector2i(resize.width, resize.height);
        resize.left = 0;
        resize.top = 0;
    };

    // update cars near tile
    void updateCars(Tile* (*fetch)(Vector2s), sf::Vector2i pos, bool underground) {
        // fetch center tile
        Tile* tile = getTile(pos);
        if (tile == NULL) return;

        // route processing buffer
        std::vector<int> routes;

        // check tile routes
        for (int nbi = 0; nbi < 4; nbi++) {
            // check 'in' route
            if (tile->routes(underground)[nbi] == Way::In) {
                // check opposite 'out' route
                if (tile->routes(underground)[inverseDirection(nbi)] == Way::Out)
                    routes.insert(routes.begin(), nbi);
                else
                    routes.push_back(nbi);
            };
        };

        // process neighbor cars
        for (int& nbi : routes) {
            // fetch neighbor tile
            Tile* nb = getTile(pos + directionOffsets[nbi]);
            if (nb == NULL) continue;

            // process car at tile
            if (nb->car) {
                // buffer car position
                sf::Vector2i bp = nb->car->tp;

                // update car
                nb->car->update(fetch, tile, pos + directionOffsets[nbi], frame, 6, 4, 4);

                // check for tile shift
                if (nb->car->tp != bp) {
                    // get resulting tile
                    Tile* res = getTile(nb->car->tp);
                    if (res == NULL) continue;

                    // replace reference
                    res->car = nb->car;
                    nb->car = NULL;
                };
            };
        };
    };

    // update cars
    void update(Tile* (*fetch)(Vector2s), sf::Vector2u windowSize) {
        // bounds size
        const int bw = 8;
        const int bh = 8;

        // calculate render bounds
        int bx = bw + windowSize.x / zoom + 2;
        int by = bh + windowSize.y / zoom + 2;

        // update camera
        if (control) {
            float dx = control->tp.x + control->fp.x / 256.0f + 0.5f;
            float dy = control->tp.y + control->fp.y / 256.0f + 0.5f;
            camera = sf::Vector2i(dx * zoom, dy * zoom);
            underground = control->underground;
        };

        // calculate view point
        sf::Vector2i view = camera - sf::Vector2i(windowSize.x / 2, windowSize.y / 2);

        // update tiles
        for (int y = -bw; y < by; y++) {
            for (int x = -bh; x < bx; x++) {
                updateCars(fetch, sf::Vector2i(view.x / zoom + x, view.y / zoom + y), false);
            };
        };

        // update frame
        frame++;
    };

    // render world
    void render(Renderer& renderer, sf::Vector2u windowSize, bool editor, bool routes, bool services, bool rsc, bool tlc, unsigned int timer, bool mapinfo) {
        // calculate render bounds
        int bx = windowSize.x / zoom + 2;
        int by = windowSize.y / zoom + 2;

        // calculate view point
        sf::Vector2i view = camera - sf::Vector2i(windowSize.x / 2, windowSize.y / 2);

        // render tiles
        for (int y = 0; y < by; y++) {
            for (int x = 0; x < bx; x++) {

                // calculate positions
                sf::IntRect pos(x * zoom - view.x % zoom, y * zoom - view.y % zoom, zoom, zoom);
                int tx = view.x / zoom + x;
                int ty = view.y / zoom + y;

                // fetch tile
                Tile* tile = getTile(tx, ty);
                if (tile == NULL)
                    continue;

                // render tile
                tile->render(renderer, pos, view, x, y, tx, ty, zoom, underground, editor, routes, services, rsc, tlc, timer);

                // render car
                if (tile->car) {
                    if (tile->car == control) {
                        tile->car->renderPath(renderer, sf::Vector2i(windowSize), view, zoom);
                    };
                    tile->car->render(renderer, sf::Vector2i(windowSize), view, zoom);
                };
            };
        };

        // render new map size
        if (mapinfo) {
            sf::IntRect dst = sf::IntRect(sf::Vector2i(windowSize) / 2 - camera + sf::Vector2i(resize.left, resize.top) * zoom, sf::Vector2i(resize.width, resize.height) * zoom);
            renderer.push(6, dst, COL7);
        };
    };

    // load world
    bool load(FILE* fp, bool edit = false) {
        // get file size
        fseek(fp, 0, 2);
        long fs = ftell(fp);
        fseek(fp, 0, 0);

        // delete old objects
        if (tiles) delete tiles;

        // delete signs
        for (Sign*& sign : signs.objs) {
            removeSign(sign);
            delete sign;
        };
        signs.objs.clear();

        // delete lights
        for (Light*& light : lights.objs) {
            removeLight(light);
            delete light;
        };
        lights.objs.clear();

        // delete cars
        cars.objs.clear();
        control = NULL;

        // fetch size
        size.x = fgetc(fp) | fgetc(fp) << 8;
        size.y = fgetc(fp) | fgetc(fp) << 8;
        resize.width  = size.x;
        resize.height = size.y;

        // generate tile buffer
        tiles = new Tile[size.x * size.y];

        // fetch tiles
        for (int i = 0; i < size.x * size.y; i++)
            tiles[i].load(fp);

        // fetch sign count
        int sc = fgetc(fp) | fgetc(fp) << 8 | fgetc(fp) << 16;

        // fetch signs
        for (int i = 0; i < sc; i++) {
            Sign data(fp);
            createSign(data.pos, data);
        };

        // fetch light count
        int lc = fgetc(fp) | fgetc(fp) << 8 | fgetc(fp) << 16;

        // fetch lights
        for (int i = 0; i < lc; i++) {
            Light data(fp);
            createLight(data.pos, data);
        };

        // fetch camera
        camera.x = fgetc(fp) | fgetc(fp) << 8 | fgetc(fp) << 16 | fgetc(fp) << 24;
        camera.y = fgetc(fp) | fgetc(fp) << 8 | fgetc(fp) << 16 | fgetc(fp) << 24;
        if (edit) camera.x += paletteWidth / 2;

        // verify file size
        if (ftell(fp) != fs) {
            // delete fetched data
            delete tiles;
            signs.objs.clear();
            lights.objs.clear();
            size = sf::Vector2i(0, 0);
            resize = sf::IntRect(0, 0, 0, 0);

            // fail
            return false;
        };

        // success
        return true;
    };

    // save world
    void dump(FILE* fp, bool edit = false) {
        // write size
        fputc(size.x & 0xFF, fp);
        fputc(size.x >> 8, fp);
        fputc(size.y & 0xFF, fp);
        fputc(size.y >> 8, fp);

        // write tiles
        for (int i = 0; i < size.x * size.y; i++)
            tiles[i].dump(fp);

        // write sign count
        fputc(signs.objs.size() >> 0  & 0xFF, fp);
        fputc(signs.objs.size() >> 8  & 0xFF, fp);
        fputc(signs.objs.size() >> 16 & 0xFF, fp);

        // write signs
        for (Sign* sign : signs.objs) {
            if (sign == NULL) continue;
            if (sign->pos.x < 0 || sign->pos.y < 0 || sign->pos.x >= size.x || sign->pos.y >= size.y) continue;
            sign->dump(fp);
        };

        // write light count
        fputc(lights.objs.size() >> 0  & 0xFF, fp);
        fputc(lights.objs.size() >> 8  & 0xFF, fp);
        fputc(lights.objs.size() >> 16 & 0xFF, fp);

        // write lights
        for (Light* light : lights.objs) {
            if (light == NULL) continue;
            if (light->pos.x < 0 || light->pos.y < 0 || light->pos.x >= size.x || light->pos.y >= size.y) continue;
            light->dump(fp);
        };

        // get real camera position
        sf::Vector2i cam(camera);
        if (edit)
            cam.x -= paletteWidth / 2;
        cam = cam * 64 / zoom;

        // write camera
        fputc(cam.x >> 0  & 0xFF, fp);
        fputc(cam.x >> 8  & 0xFF, fp);
        fputc(cam.x >> 16 & 0xFF, fp);
        fputc(cam.x >> 24 & 0xFF, fp);
        fputc(cam.y >> 0  & 0xFF, fp);
        fputc(cam.y >> 8  & 0xFF, fp);
        fputc(cam.y >> 16 & 0xFF, fp);
        fputc(cam.y >> 24 & 0xFF, fp);
    };

} world;
