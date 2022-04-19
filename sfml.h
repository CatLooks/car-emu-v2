// -- SFML-related stuff -- //

// encapsulate into <sfml>
struct {

    // constants
    const char* appName = "Roads Simulator";
    const char* appAuth = "by CatLooks";

    // variables
    sf::RenderWindow app;
    sf::View view;
    bool app_fs = false;

    // create window
    void createWindow(bool fs) {
        // create window
        if (fs)
            app.create(sf::VideoMode::getFullscreenModes()[0], appName, sf::Style::Fullscreen);
        else
            app.create(sf::VideoMode(1280, 720), appName, sf::Style::Close);

        // set icon
        app.setIcon(32, 32, resource.icon.getPixelsPtr());

        // limit framerate
        app.setVerticalSyncEnabled(true);
    };

    // handle window events
    bool handleEvent(sf::Event& evt) {
        // close event
        if (evt.type == sf::Event::Closed) {
            app.close();
            return false;
        };

        // keypress event
        if (evt.type == sf::Event::KeyPressed) {
            // toggle fullscreen
            if (evt.key.code == sf::Keyboard::F11) {
                app_fs = !app_fs;
                app.close();
                createWindow(app_fs);
                return false;
            };
        };

        // game event
        return true;
    };

} sfml;
