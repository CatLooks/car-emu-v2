// -- car objects -- //

// car object
struct Car {

    // car state
    bool underground;
    bool updund;
    sf::Vector2i tp;  // tile position
    sf::Vector2i fp;  // fraction position
    Direction dir;
    int frame;
    sf::Uint32 idc;
    Cars::modes ai;

    // car attributes
    Cars::types type;
    int speed;
    int maxsp;

    // destination
    std::deque<Vector2s> dest;

    // get pathfing enumerator
    static int enumerator() {
        static int i = 0;
        return ++i;
    };

    // constructor
    Car (Cars::modes AI, sf::Vector2i position, Direction direction, Cars::types type, int speed) {
        this->tp = position;
        this->dir = direction;
        this->type = type;
        this->speed = speed;
        this->maxsp = speed;
        underground = false;
        updund = false;
        frame = -1;
        ai = AI;

        // car id
        static int cid = 0;
        idc = cid++;
    };

    // car decision maker
    void next() {
        // check ai mode
        switch (ai) {
            case Cars::NoAI: return;
            case Cars::Random: {

            }; break;
            case Cars::Smart: {

            }; break;
        };
    };

    // pathfind to service
    bool pathfindService(Vector2s start, Tile* (*fetch)(Vector2s), Service::types service, int count, int maxDepth = 1024) {
        // get pathfind ID
        int id = enumerator();

        // node object
        struct node_t {
            Vector2s pos;
            node_t* parent = NULL;
            bool u = false;
            int len = 0;

            // constructors
            node_t () {};
            node_t (Vector2s pos) {
                this->pos = pos;
            };
            node_t (Vector2s pos, node_t* parent, bool u, int len) {
                this->parent = parent;
                this->pos = pos;
                this->len = len;
                this->u = u;
            };
        };

        // service buffer
        std::vector<node_t*> services;

        // node buffers
        std::queue<node_t*> nodes;
        Stamper<node_t> stamper;

        // push root node
        node_t* root = stamper.copy(node_t(start));
        nodes.push(root);

        // mark root tile
        Tile* roottile = fetch(root->pos);
        if (roottile == NULL) return false;
        roottile->pfb[0] = id;

        // process queue
        while (!nodes.empty() && (int)services.size() < count) {
            // pop next node
            node_t* node = nodes.front();
            nodes.pop();

            // check node length
            if (node->len >= maxDepth)
                break;

            // fetch node tile
            Tile* tile = fetch(node->pos);
            if (tile == NULL) continue;

            // get tile routes
            Way::types* routes = tile->routes(node->u);

            // check neighbor tiles
            for (int nbi = 0; nbi < 4; nbi++) {
                // fetch neighbor tile
                Tile* nb = fetch(node->pos + routeOffsets[nbi]);
                if (nb == NULL) continue;

                // check straight route
                if (nb->pfb[node->u] != id && routes[nbi] == Way::Out && nb->routes(node->u)[inverseDirection(nbi)] == Way::In) {
                    // push new node
                    nodes.push(stamper.copy(node_t(node->pos + routeOffsets[nbi], node, node->u, node->len + 1)));
                }

                // check switch route
                else if (nb->pfb[!node->u] != id && routes[nbi] == Way::Switch && nb->routes(!node->u)[inverseDirection(nbi)] == Way::In) {
                    // push new node
                    nodes.push(stamper.copy(node_t(node->pos + routeOffsets[nbi], node, !node->u, node->len + 1)));
                } else continue;

                // mark tile
                nb->pfb[nodes.back()->u] = id;

                // check for service
                if (nb->service == service) {
                    services.push_back(nodes.back());
                };
            };
        };

        // select random service
        if (services.empty())
            return false;
        node_t* dest = services[rand() % services.size()];

        // position buffer
        std::stack<Vector2s> posbuf;

        // construct path
        while (dest->parent) {
            // push position
            posbuf.push(dest->pos);

            // process parent
            dest = dest->parent;
        };

        // push position buffer to queue
        while (!posbuf.empty()) {
            this->dest.push_back(posbuf.top());
            posbuf.pop();
        };

        // success
        return true;
    };

    // pathfind to point
    bool pathfindPoint(Vector2s start, Tile* (*fetch)(Vector2s), Vector2s position, int maxDepth = 1024) {
        // get pathfind ID
        int id = enumerator();

        // node object
        struct node_t {
            Vector2s pos;
            node_t* parent = NULL;
            bool u = false;
            int len = 0;

            // constructors
            node_t () {};
            node_t (Vector2s pos) {
                this->pos = pos;
            };
            node_t (Vector2s pos, node_t* parent, bool u, int len) {
                this->parent = parent;
                this->pos = pos;
                this->len = len;
                this->u = u;
            };
        };

        // node buffers
        std::queue<node_t*> nodes;
        Stamper<node_t> stamper;

        // push root node
        node_t* root = stamper.copy(node_t(start));
        nodes.push(root);

        // mark root tile
        Tile* roottile = fetch(root->pos);
        if (roottile == NULL) return false;
        roottile->pfb[0] = id;

        // destination
        node_t* dest = NULL;

        // process queue
        while (!nodes.empty() && !dest) {
            // pop next node
            node_t* node = nodes.front();
            nodes.pop();

            // check node length
            if (node->len >= maxDepth)
                break;

            // fetch node tile
            Tile* tile = fetch(node->pos);
            if (tile == NULL) continue;

            // get tile routes
            Way::types* routes = tile->routes(node->u);

            // check neighbor tiles
            for (int nbi = 0; nbi < 4; nbi++) {
                // fetch neighbor tile
                Tile* nb = fetch(node->pos + routeOffsets[nbi]);
                if (nb == NULL) continue;

                // check straight route
                if (nb->pfb[node->u] != id && routes[nbi] == Way::Out && nb->routes(node->u)[inverseDirection(nbi)] == Way::In) {
                    // push new node
                    nodes.push(stamper.copy(node_t(node->pos + routeOffsets[nbi], node, node->u, node->len + 1)));
                }

                // check switch route
                else if (nb->pfb[!node->u] != id && routes[nbi] == Way::Switch && nb->routes(!node->u)[inverseDirection(nbi)] == Way::In) {
                    // push new node
                    nodes.push(stamper.copy(node_t(node->pos + routeOffsets[nbi], node, !node->u, node->len + 1)));
                } else continue;

                // mark tile
                nb->pfb[nodes.back()->u] = id;

                // check for destination
                if (node->pos + routeOffsets[nbi] == position) {
                    dest = nodes.back();
                    break;
                };
            };
        };

        // check destination
        if (dest == NULL)
            return false;

        // position buffer
        std::stack<Vector2s> posbuf;

        // construct path
        while (dest->parent) {
            // push position
            posbuf.push(dest->pos);

            // process parent
            dest = dest->parent;
        };

        // push position buffer to queue
        while (!posbuf.empty()) {
            this->dest.push_back(posbuf.top());
            posbuf.pop();
        };

        // success
        return true;
    };

    // pathfind to previous pathfind ID
    bool pathfindID(Vector2s start, Tile* (*fetch)(Vector2s), int maxDepth = 1024) {
        // get pathfind ID
        int id = enumerator();

        // node object
        struct node_t {
            Vector2s pos;
            node_t* parent = NULL;
            bool u = false;
            int len = 0;

            // constructors
            node_t () {};
            node_t (Vector2s pos) {
                this->pos = pos;
            };
            node_t (Vector2s pos, node_t* parent, bool u, int len) {
                this->parent = parent;
                this->pos = pos;
                this->len = len;
                this->u = u;
            };
        };

        // node buffers
        std::queue<node_t*> nodes;
        Stamper<node_t> stamper;

        // push root node
        node_t* root = stamper.copy(node_t(start));
        nodes.push(root);

        // mark root tile
        Tile* roottile = fetch(root->pos);
        if (roottile == NULL) return false;
        roottile->pfb[0] = id;

        // destination
        node_t* dest = NULL;

        // process queue
        while (!nodes.empty() && !dest) {
            // pop next node
            node_t* node = nodes.front();
            nodes.pop();

            // check node length
            if (node->len >= maxDepth)
                break;

            // fetch node tile
            Tile* tile = fetch(node->pos);
            if (tile == NULL) continue;

            // get tile routes
            Way::types* routes = tile->routes(node->u);

            // check neighbor tiles
            for (int nbi = 0; nbi < 4; nbi++) {
                // fetch neighbor tile
                Tile* nb = fetch(node->pos + routeOffsets[nbi]);
                if (nb == NULL) continue;

                // check straight route
                if (nb->pfb[node->u] != id && routes[nbi] == Way::Out && nb->routes(node->u)[inverseDirection(nbi)] == Way::In) {
                    // push new node
                    nodes.push(stamper.copy(node_t(node->pos + routeOffsets[nbi], node, node->u, node->len + 1)));
                }

                // check switch route
                else if (nb->pfb[!node->u] != id && routes[nbi] == Way::Switch && nb->routes(!node->u)[inverseDirection(nbi)] == Way::In) {
                    // push new node
                    nodes.push(stamper.copy(node_t(node->pos + routeOffsets[nbi], node, !node->u, node->len + 1)));
                } else continue;

                // check for ID
                if (nb->pfb[nodes.back()->u] == id - 1) {
                    dest = nodes.back();
                    break;
                };

                // mark tile
                nb->pfb[nodes.back()->u] = id;
            };
        };

        // check destination
        if (dest == NULL)
            return false;

        // remove destinations up to route end
        while (this->dest.front() != dest->pos && !this->dest.empty())
            this->dest.pop_front();
        if (this->dest.empty())
            return false;
        else
            this->dest.pop_front();

        // position buffer
        std::stack<Vector2s> posbuf;

        // construct path
        while (dest->parent) {
            // push position
            this->dest.push_front(dest->pos);

            // process parent
            dest = dest->parent;
        };

        // success
        return true;
    };

    // update car buffer
    void updateBuffer(Tile* (*fetch)(Vector2s), bool fill) {
        // car level
        if (updund) {
            updund = false;
            underground = false;
        };

        // check destination count
        if (!dest.empty()) {
            // occupance buffer
            int vel = sqrt(speed);
            int len = (int)dest.size() < vel ? dest.size() : vel;

            for (int i = 0; i < len; i++) {
                // fetch tile
                Tile* tile = fetch(dest[i]);
                if (tile == NULL) return;

                // update buffer
                if (tile->occ || tile->car) {
                    fill = false;
                    continue;
                };
                tile->occ = fill ? this : NULL;
            };
        };

        // reset current buffer
        Tile* below = fetch(Vector2s(tp.x, tp.y));
        if (below == NULL) return;
        below->occ = NULL;
    };

    // update car
    void update(Tile* (*fetch)(Vector2s), Tile* center, sf::Vector2i pos, int global, int vision, int block_speed, int block_count) {
        // check for tick
        if (global == frame) return;
        frame = global;

        // check for movement
        if (dest.empty()) return;

        // fetch direction
        dir = adjacentSide(Vector2s(tp.x, tp.y), dest.front());

        // fetch next tile
        Tile* tile = fetch(dest.front());
        if (tile == NULL) return;

        // check for traffic jam
        int len = (int)dest.size() < vision ? dest.size() : vision;
        int block = 0;
        for (int i = 0; i < len; i++) {
            // fetch tile
            Tile* tile = fetch(dest[i]);
            if (tile == NULL) continue;

            // check for car
            if (tile->car) {
                if (tile->car->speed >= block_speed)
                    block++;
            };

            // check for traffic light
            if (tile->light) {
                if (tile->light->lock(lights.timer))
                    block++;
            };

            // check for road sign
            if (tile->sign) {
                if (tile->sign->type == RoadSign::Lock)
                    block++;
            };
        };

        // try to pathfind over traffic jam
        if (block >= block_count) {
            // underground state
            bool ug = underground;

            // get target line destination count
            int target_count = ((int)dest.size() - len) < 32 ? (int)dest.size() - len : 32;
            if (target_count > 0) {
                // get target pathfind ID
                int target_id = enumerator();

                // destination iterator
                auto dest_iter = dest.cbegin();

                // mark jammed line
                for (int i = 0; i < len; i++) {
                    // get position
                    Vector2s pos = *(dest_iter++);
                    Vector2s nxt = *dest_iter;

                    // get tile
                    Tile* tile = fetch(pos);
                    if (tile == NULL) return;

                    // mark tile
                    tile->pfb[ug] = target_id + 1;

                    // check for level shift
                    Tile* nb = fetch(nxt);
                    if (nb == NULL) return;
                    Direction dir = adjacentSide(pos, nxt);
                    if (nb->routes(ug)[convertDirection(dir)] == Way::Switch) {
                        ug = !ug;
                    };
                };

                // mark target line
                for (int i = 0; i < target_count; i++) {
                    // get position
                    Vector2s pos = *(dest_iter++);
                    Vector2s nxt = *dest_iter;

                    // get tile
                    Tile* tile = fetch(pos);
                    if (tile == NULL) return;

                    // mark tile
                    tile->pfb[ug] = target_id;

                    // check for level shift
                    if (i != target_count - 1) {
                        Tile* nb = fetch(nxt);
                        if (nb == NULL) return;
                        Direction dir = adjacentSide(pos, nxt);
                        if (nb->routes(ug)[convertDirection(dir)] == Way::Switch) {
                            ug = !ug;
                        };
                    };
                };

                // create new path
                pathfindID((Vector2s)tp, fetch);
            };
        };

        // check if able to move
        /*printf("Car %08X:\n", idc);
        printf("Check 0: %s\n", booleanNames[tile->car ? 1 : 0]);
        printf("Check 1: %s\n", booleanNames[!(tile->occ == NULL || tile->occ == this)]);
        printf("Check 2: %s\n", booleanNames[tile->sign->lock()]);
        printf("Check 3: %s\n", booleanNames[tile->light->lock(lights.timer)]);
        printf("Check 4: %s\n", booleanNames[tile->routes(underground)[invertDirection(dir)] != Way::In]);

        printf("Data:\n");
        printf("Routes: %d %d %d %d\n", tile->routes(underground)[0], tile->routes(underground)[1], tile->routes(underground)[2], tile->routes(underground)[3]);
        printf("Direction: %d\n", invertDirection(dir));*/

        // check for car
        if (tile->car) {
            // check if front car has the same direction
            if (tile->car->dir == dir) {
                // move car
                fp.x += directionOffsets[convertDirection(dir)].x * speed;
                fp.y += directionOffsets[convertDirection(dir)].y * speed;

                // clamp movement
                switch (dir) {
                    case Direction::North: {
                        if (tile->car->fp.y > fp.y)
                            fp.y = tile->car->fp.y;
                    };
                    break;
                    case Direction::South: {
                        if (tile->car->fp.y < fp.y)
                            fp.y = tile->car->fp.y;
                    };
                    break;
                    case Direction::West: {
                        if (tile->car->fp.x < fp.x)
                            fp.x = tile->car->fp.x;
                    };
                    break;
                    case Direction::East: {
                        if (tile->car->fp.x > fp.x)
                            fp.x = tile->car->fp.x;
                    };
                    break;
                    default:
                    break;
                };
            };
            return;
        };

        // other checks
        if (!(tile->occ == NULL || tile->occ == this)) return;
        if (tile->sign->lock()) return;
        if (tile->light->lock(lights.timer)) return;
        if (tile->routes(underground)[invertDirection(dir)] != Way::In) return;

        // fetch tile below
        Tile* below = fetch(Vector2s(tp));
        if (below == NULL) return;
        if (below->routes(underground)[dir] == Way::Switch) {
            if (underground)
                updund = true;
            else
                underground = true;
        };

        // move car
        fp.x += directionOffsets[convertDirection(dir)].x * speed;
        fp.y += directionOffsets[convertDirection(dir)].y * speed;

        // check for overflow
        if (fp.x / 256) {
            tp.x += fp.x / 256;
            fp.x = clamp(fp.x % 256, -32, 32);
            dest.pop_front();
            updateBuffer(fetch, true);
        };
        if (fp.y / 256) {
            tp.y += fp.y / 256;
            fp.y = clamp(fp.y % 256, -32, 32);
            dest.pop_front();
            updateBuffer(fetch, true);
        };
    };

    // render car
    void render(Renderer& renderer, sf::Vector2i windowSize, sf::Vector2i view, int zoom) {
        // get position
        float dx = tp.x + fp.x / 256.0f;
        float dy = tp.y + fp.y / 256.0f;
        sf::IntRect pos(dx * zoom - view.x, dy * zoom - view.y, zoom, zoom);

        // render car
        if (!(underground && !this->underground))
            renderer.push(this->underground ? 1 : 3, pos, sf::IntRect(Cars::textures[type], sf::Vector2i(32, 32)), dir);
    };

    // render path
    void renderPath(Renderer& renderer, sf::Vector2i windowSize, sf::Vector2i view, int zoom) {
        // current buffer
        sf::IntRect buf(tp.x * zoom - view.x, tp.y * zoom - view.y, zoom, zoom);
        Direction bdr = Direction::None;
        Vector2s bps = Vector2s(tp);

        // render each destination
        for (size_t i = 0; i < dest.size(); i++) {
            // get current position
            sf::IntRect pos(dest[i].x * zoom - view.x, dest[i].y * zoom - view.y, zoom, zoom);

            // get route direction
            Direction dir = adjacentSide(bps, dest[i]);

            // render buffer arrow
            renderer.push(2, buf, TEX_ROUTE[1], dir);

            // render current arrow
            renderer.push(2, pos, TEX_ROUTE[1], oppositeDirection[dir]);

            // render center
            if (dir != bdr) {
                renderer.push(2, buf, TEX_ROUTE[0]);
            };

            // check if out of range
            if (sqdist(bps, dest[i]) >= square(windowSize.x / zoom + 8))
                return;

            // update buffer
            bps = dest[i];
            buf = pos;
            bdr = dir;
        };

        // render ending
        renderer.push(2, buf, TEX_ROUTE[2]);
    };
};

// encapsulate into <cars>
struct {

    // cars containter
    std::deque<Car> objs;

} cars;
