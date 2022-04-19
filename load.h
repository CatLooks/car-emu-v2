// -- loading functions -- //

// encapsulate into <import>
struct {

    // containers
    sf::Texture tex;
    sf::Image icon;
    sf::Font font;
    sf::Text text;

    // load stuff
    int import() {
        // load tileset
        if (!tex.loadFromFile("tileset.png"))
            return 901;

        // load font
        if (!font.loadFromFile("font.ttf"))
            return 902;

        // setup icon
        sf::Image img = tex.copyToImage();
        icon.create(32, 32, sf::Color::Black);

        // copy icon
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 32; x++) {
                icon.setPixel(x, y, img.getPixel(x + 384, y + 128));
            };
        };

        // load text
        text.setFont(font);
        text.setCharacterSize(12);
        text.setFillColor(sf::Color(255, 255, 255));
        text.setOutlineColor(sf::Color(16, 16, 16));
        text.setOutlineThickness(2.0f);

        // success
        return 0;
    };

} resource;
