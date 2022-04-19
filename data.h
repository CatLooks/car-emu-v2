// -- world data types -- //

// animation timer
const sf::Uint32 animSpeed = 10;
sf::Uint32 animTimer = 0;
sf::Uint32 animFrame = 0;

// animation frames
const sf::IntRect waterAnimation[] {
    sf::IntRect(0, 0,  32, 32),
    sf::IntRect(0, 0,  32, 32),
    sf::IntRect(0, 32, 32, 32),
    sf::IntRect(0, 64, 32, 32),
    sf::IntRect(0, 96, 32, 32),
    sf::IntRect(0, 96, 32, 32),
    sf::IntRect(0, 64, 32, 32),
    sf::IntRect(0, 32, 32, 32)
};
const sf::IntRect grassAnimation[] {
    sf::IntRect(32, 0,  32, 32),
    sf::IntRect(32, 32, 32, 32),
    sf::IntRect(32, 64, 32, 32),
    sf::IntRect(32, 32, 32, 32)
};
const sf::IntRect flowerAnimation[] {
    sf::IntRect(64, 0,  32, 32),
    sf::IntRect(64, 32, 32, 32),
    sf::IntRect(64, 64, 32, 32),
    sf::IntRect(64, 32, 32, 32)
};

// game states
namespace States {
    // types
    enum Game {
        Menu = 0, View, Edit
    };
    enum Editor {
        Ground = 0, Outline, Deco, Route, Service, RoadSign, TrafficLight, MapInfo,
        EditCount
    };

    // texts
    const sf::String strings[EditCount] {
        sf::String("Ground"),
        sf::String("Outlines"),
        sf::String("Decorations"),
        sf::String("Routes"),
        sf::String("Services"),
        sf::String("Road Signs"),
        sf::String("Traffic Lights"),
        sf::String("Map Size")
    };
};

// ground types
namespace Ground {
    // types
    enum types {
        Void = 0, Water, Grass, Flowers, Sand, Stone, Road, Highway, Bricks, Plate,
        GroundCount
    };

    // textures
    const sf::Vector2i textures[GroundCount] {
        sf::Vector2i(480, 96),
        sf::Vector2i(0,   0),
        sf::Vector2i(32,  0),
        sf::Vector2i(64,  0),
        sf::Vector2i(96,  0),
        sf::Vector2i(128, 0),
        sf::Vector2i(160, 0),
        sf::Vector2i(192, 0),
        sf::Vector2i(224, 0),
        sf::Vector2i(256, 0)
    };

    // similarity of grounds
    const bool similar[GroundCount][GroundCount] {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1}
    };

    // ascending of texture
    const types ascend[GroundCount] {
        Void, Void, Void, Void, Void, Void, Void, Void, Void, Void
    };

    // names
    const char* names[GroundCount] {
        "None", "Water", "Grass", "Flowers", "Sand", "Stone", "Road", "Highway", "Bricks", "Plate"
    };

    // maximum speed
    const int maxSpeed[GroundCount] {
        0
    };
};

// outline types
namespace Outline {
    // types
    enum types {
        None = 0, Border, Grass, Sand, Gate, Mark, Auto, AutoMark,
        OutlineCount
    };

    // textures
    const sf::Vector2i textures[OutlineCount] {
        sf::Vector2i(480, 96),
        sf::Vector2i(96,  32),
        sf::Vector2i(128, 32),
        sf::Vector2i(96,  64),
        sf::Vector2i(128, 64),
        sf::Vector2i(480, 128),
        sf::Vector2i(448, 128),
        sf::Vector2i(448, 128)
    };

    // outline types
    const types outlines[Ground::GroundCount] {
        None, Gate, Grass, None, Sand, None, Border, Border, Border, Border
    };

    // names
    const char* names[OutlineCount] {
        "None", "Border", "Grass", "Sand", "Gate", "Lines", "Auto Borders", "Auto Lines"
    };
};

// decoration types
namespace Decoration {
    // types
    enum types {
        None = 0, Box, Roadblock, Manhole, CrosswalkH, CrosswalkV, Sewers,
        DecorationCount
    };

    // textures
    const sf::Vector2i textures[DecorationCount] {
        sf::Vector2i(480, 96),
        sf::Vector2i(320, 64),
        sf::Vector2i(256, 32),
        sf::Vector2i(256, 64),
        sf::Vector2i(320, 32),
        sf::Vector2i(288, 32),
        sf::Vector2i(288, 64),
    };

    // free texture rotation
    const sf::Uint8 rotation[DecorationCount] {
        0, 1, 0, 1, 0, 0, 2
    };

    // names
    const char* names[DecorationCount] {
        "None", "Box", "Roadblock", "Manhole", "Crosswalk", "Crosswalk", "Sewers"
    };
};

// way types
namespace Way {
    // types
    enum types {
        None = 0, In, Out, Switch
    };

    // textures
    const sf::Vector2i textures[3][2] {
        {
            sf::Vector2i(160, 64),
            sf::Vector2i(192, 64)
        }, {
            sf::Vector2i(160, 32),
            sf::Vector2i(192, 32)
        }, {
            sf::Vector2i(224, 32),
            sf::Vector2i(224, 32)
        }
    };
    const sf::Vector2i textureJunction = sf::Vector2i(224, 64);
};

// service types
namespace Service {
    // types
    enum types {
        None = 0, Quick, Parking, Shop, Fuel, Spawn, Exit, Group, Storage, Warehouse,
        ServiceCount
    };

    // textures
    const sf::Vector2i textures[ServiceCount] {
        sf::Vector2i(480, 96),
        sf::Vector2i(32,  96),
        sf::Vector2i(64,  96),
        sf::Vector2i(96,  96),
        sf::Vector2i(128, 96),
        sf::Vector2i(160, 96),
        sf::Vector2i(192, 96),
        sf::Vector2i(224, 96),
        sf::Vector2i(256, 96),
        sf::Vector2i(288, 96)
    };

    // names
    const char* names[ServiceCount] {
        "None", "Quick", "Parking", "Mall", "Fuel Station", "Spawn Point", "Exit Point", "Group", "Storage", "Warehouse"
    };
};

// road signs
namespace RoadSign {
    // types
    enum types {
        Lock = 0, Max, Min, Walk,
        RoadSignCount
    };

    // textures
    const sf::Vector2i textures[RoadSignCount] {
        sf::Vector2i(320, 96),
        sf::Vector2i(328, 96),
        sf::Vector2i(336, 96),
        sf::Vector2i(344, 96)
    };

    // names
    const char* names[RoadSignCount] {
        "Forbid", "Max Speed", "Min Speed", "Crosswalk"
    };
};

// traffic lights
namespace TrafficLight {
    // types
    enum types {
        Single = 0, Mini, Full,
        TrafficLightCount
    };

    // states
    enum States {
        Red = 0, Yellow, Green, Off
    };

    // textures
    const sf::Vector2i textures[TrafficLightCount][4] {
        {
            sf::Vector2i(352, 96),
            sf::Vector2i(360, 96),
            sf::Vector2i(368, 96),
            sf::Vector2i(376, 96)
        }, {
            sf::Vector2i(384, 96),
            sf::Vector2i(392, 96),
            sf::Vector2i(400, 96),
            sf::Vector2i(408, 96)
        }, {
            sf::Vector2i(416, 96),
            sf::Vector2i(424, 96),
            sf::Vector2i(432, 96),
            sf::Vector2i(440, 96)
        }
    };

    // position offsets
    const sf::Vector2f offsets[4] {
        sf::Vector2f(-6.0f / 32.0f, -33.0f / 32.0f),
        sf::Vector2f(32.0f / 32.0f, -33.0f / 32.0f),
        sf::Vector2f(-6.0f / 32.0f, 3.0f   / 32.0f),
        sf::Vector2f(32.0f / 32.0f, 3.0f   / 32.0f)
    };

    // names
    const char* names[TrafficLightCount] {
        "Single", "Mini", "Full"
    };
};

// world template types
namespace Template {
    // types
    enum types {
        Void = 0, Field, Sea, Desert,
        TemplateCount
    };

    // textures
    const Ground::types textures[TemplateCount] {
        Ground::Void, Ground::Flowers, Ground::Water, Ground::Sand
    };

    // names
    const char* names[TemplateCount] {
        "Void", "Field", "Sea", "Desert"
    };
};

// cars data
namespace Cars {
    // types
    enum types {
        Small = 0, Normal, Taxi, Bus, Luxury, Van,
        CarCount
    };

    // modes
    enum modes {
        NoAI = 0, Random, Smart
    };

    // textures
    const sf::Vector2i textures[CarCount] {
        sf::Vector2i(448, 0),
        sf::Vector2i(480, 0),
        sf::Vector2i(448, 32),
        sf::Vector2i(480, 32),
        sf::Vector2i(448, 64),
        sf::Vector2i(480, 64)
    };

    // names
    const char* names[CarCount] {
        "Mini", "Normal", "Taxi", "Bus", "Luxury", "Van"
    };
};

// none text
const sf::String textNone("~");

// text colors
const sf::Color TEXT_USE = sf::Color::White;
const sf::Color TEXT_LAB = sf::Color(255, 64, 255);
const sf::Color TEXT_INV = sf::Color::Red;
const sf::Color TEXT_FLG = sf::Color::Green;

// GUI colors
const sf::IntRect COL0 = sf::IntRect(0,  128, 4, 4);
const sf::IntRect COL1 = sf::IntRect(4,  128, 4, 4);
const sf::IntRect COL2 = sf::IntRect(8,  128, 4, 4);
const sf::IntRect COL3 = sf::IntRect(12, 128, 4, 4);
const sf::IntRect COL4 = sf::IntRect(16, 128, 4, 4);
const sf::IntRect COL5 = sf::IntRect(20, 128, 4, 4);
const sf::IntRect COL6 = sf::IntRect(24, 128, 4, 4);
const sf::IntRect COL7 = sf::IntRect(28, 128, 4, 4);
const sf::IntRect COL8 = sf::IntRect(0,  132, 4, 4);
const sf::IntRect COL9 = sf::IntRect(4,  132, 4, 4);

// weather icons
const sf::IntRect TEX_WEATHER[2][6] {
    {
        sf::IntRect(480, 192, 32, 32),
        sf::IntRect(448, 192, 32, 32),
        sf::IntRect(416, 192, 32, 32),
        sf::IntRect(384, 192, 32, 32),
        sf::IntRect(352, 192, 32, 32),
        sf::IntRect(320, 192, 32, 32)
    }, {
        sf::IntRect(480, 224, 32, 32),
        sf::IntRect(448, 224, 32, 32),
        sf::IntRect(416, 224, 32, 32),
        sf::IntRect(384, 224, 32, 32),
        sf::IntRect(352, 224, 32, 32),
        sf::IntRect(320, 224, 32, 32)
    }
};

// route textures
const sf::IntRect TEX_ROUTE[3] {
    sf::IntRect(384, 160, 32, 32),
    sf::IntRect(352, 160, 32, 32),
    sf::IntRect(416, 160, 32, 32)
};

// GUI textures
const sf::IntRect BTN_MENU = sf::IntRect(32,  128, 32, 32);
const sf::IntRect BTN_SPEED[3] {
    sf::IntRect(64,  128, 32, 32),
    sf::IntRect(96,  128, 32, 32),
    sf::IntRect(128, 128, 32, 32)
};
const sf::IntRect BTN_MODE[2][States::EditCount] {
    {
        sf::IntRect(160, 128, 32, 32),
        sf::IntRect(224, 128, 32, 32),
        sf::IntRect(256, 128, 32, 32),
        sf::IntRect(288, 128, 32, 32),
        sf::IntRect(352, 128, 32, 32),
        sf::IntRect(384, 128, 32, 32),
        sf::IntRect(416, 128, 32, 32),
        sf::IntRect(320, 160, 32, 32),
    }, {
        sf::IntRect(192, 128, 32, 32),
        sf::IntRect(224, 128, 32, 32),
        sf::IntRect(256, 128, 32, 32),
        sf::IntRect(320, 128, 32, 32),
        sf::IntRect(352, 128, 32, 32),
        sf::IntRect(384, 128, 32, 32),
        sf::IntRect(416, 128, 32, 32),
        sf::IntRect(320, 160, 32, 32),
    }
};
const sf::IntRect BTN_LEVEL[2] {
    sf::IntRect(0,  160, 32, 32),
    sf::IntRect(32, 160, 32, 32)
};
const sf::IntRect BTN_STATE[2] {
    sf::IntRect(64, 160, 32, 32),
    sf::IntRect(96, 160, 32, 32)
};
const sf::IntRect BTN_CANCEL = sf::IntRect(128, 160, 32, 32);
const sf::IntRect BTN_ROUTE[4] {
    sf::IntRect(160, 160, 16, 16),
    sf::IntRect(176, 160, 16, 16),
    sf::IntRect(160, 176, 16, 16),
    sf::IntRect(176, 176, 16, 16)
};
const sf::IntRect BTN_ROTATE[4] {
    sf::IntRect(192, 160, 32, 32),
    sf::IntRect(224, 160, 32, 32),
    sf::IntRect(256, 160, 32, 32),
    sf::IntRect(128, 160, 32, 32)
};
const sf::IntRect BTN_CAR_PLACE = sf::IntRect(0, 192, 32, 32);
const sf::IntRect BTN_CAR_EDIT = sf::IntRect(128, 192, 32, 32);
const sf::IntRect BTN_CAR_AI[3] {
    sf::IntRect(32, 192, 32, 32),
    sf::IntRect(64, 192, 32, 32),
    sf::IntRect(96, 192, 32, 32)
};

// layout constants
const int buttonSize = 64;
const int labelSize = 48;
const int paletteStart = buttonSize + labelSize * 3 / 2;
const int paletteWidth = 256;
const int paletteThick = 4;
const int buttonPadding = 4;
