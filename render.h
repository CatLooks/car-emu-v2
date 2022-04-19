// -- render objects -- //

// rect object
struct RenderRect {
    sf::IntRect pos;
    sf::IntRect tex;
    Direction dir;

    // constructor
    RenderRect (sf::IntRect position, sf::IntRect texture, Direction direction = Direction::North) {
        pos = position;
        tex = texture;
        dir = direction;
    };
};

// text object
struct RenderText {
    sf::String str;
    sf::IntRect pos;
    sf::Color col;
    int cs;

    // constructor
    RenderText (sf::String str, sf::IntRect position, sf::Color color, int charSize) {
        this->str = str;
        pos = position;
        col = color;
        cs = charSize;
    };
};

// renderer object
struct Renderer {
    // variables
    std::vector<RenderRect> rects[8];
    std::vector<RenderText> texts;

    // push rect
    inline void push(int id, RenderRect& rect) {
        rects[id].push_back(rect);
    };
    inline void push(int id, sf::IntRect pos, sf::IntRect tex, Direction dir = Direction::North) {
        rects[id].push_back(RenderRect(pos, tex, dir));
    };

    // push text
    inline void text(sf::String str, sf::IntRect position, sf::Color color, int charSize) {
        texts.push_back(RenderText(str, position, color, charSize));
    };

    // render buffer
    Pair<int, int> render() {
        sf::VertexArray arr(sf::Quads);

        // render rects
        for (int i = 0; i < 8; i++) {
            for (RenderRect& rect : rects[i]) {
                renderRect(arr, rect.pos, rect.tex, rect.dir);
            };
        };
        sfml.app.draw(arr, &resource.tex);

        // render texts
        for (RenderText& text : texts) {
            resource.text.setString(text.str);
            resource.text.setCharacterSize(text.cs);
            resource.text.setFillColor(text.col);
            sf::FloatRect bounds = resource.text.getGlobalBounds();

            int px = text.pos.left;
            if (text.pos.width)
                px += (text.pos.width - bounds.width) / 2;
            int py = text.pos.top;
            if (text.pos.height)
                py += (text.pos.height - bounds.height) / 2;

            resource.text.setPosition(px, py);
            sfml.app.draw(resource.text);
        };

        // return vertex count
        Pair<int, int> p;
        p.a = arr.getVertexCount();
        p.b = texts.size();
        return p;
    };
};
