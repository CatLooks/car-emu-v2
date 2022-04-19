// -- basic stuff -- //

// short vector
typedef sf::Vector2<short> Vector2s;

// boolean strings
const char* booleanNames[2] { "False", "True" };

// neighbor tile offsets
const sf::Vector2i neighborOffsets[8] {
    sf::Vector2i( 0, -1),
    sf::Vector2i( 1,  0),
    sf::Vector2i( 0,  1),
    sf::Vector2i(-1,  0),
    sf::Vector2i(-1, -1),
    sf::Vector2i( 1, -1),
    sf::Vector2i(-1,  1),
    sf::Vector2i( 1,  1)
};

// route neighbor offset
const Vector2s routeOffsets[4] {
    Vector2s( 0, -1),
    Vector2s( 1,  0),
    Vector2s( 0,  1),
    Vector2s(-1,  0),
};

// directional offset
const sf::Vector2i directionOffsets[4] {
    sf::Vector2i( 0, -1),
    sf::Vector2i( 1,  0),
    sf::Vector2i( 0,  1),
    sf::Vector2i(-1,  0),
};

// direction type
struct enum_type_direction {
    enum types {
        North = 0, South, West, East, None
    };
};
typedef enum_type_direction::types Direction;

// type pair
template <class A, class B>
struct Pair {
    A a; B b;
    Pair () {};
    Pair (A x, B y) {
        a = x; b = y;
    };
};
template <class A, class B, class C>
struct Triplet {
    A a; B b; C c;
    Triplet () {};
    Triplet (A x, B y, C z) {
        a = x; b = y; c = z;
    };
};

// opposite directions
const Direction oppositeDirection[5] {
    Direction::South, Direction::North, Direction::East, Direction::West, Direction::None
};

// convert direction to index
inline int convertDirection(Direction dir) {
    const int table[5] {0, 2, 1, 3, 0};
    return table[dir];
};
inline int inverseDirection(int dir) {
    const int table[5] {2, 3, 0, 1, 0};
    return table[dir];
};
inline int invertDirection(Direction dir) {
    const int table[5] {2, 0, 3, 1, 0};
    return table[dir];
};

// check adjacent blocks
Direction adjacentSide(sf::Vector2i a, sf::Vector2i b) {
    if (a + sf::Vector2i(-1,  0) == b) return Direction::East;
    if (a + sf::Vector2i( 0, -1) == b) return Direction::North;
    if (a + sf::Vector2i( 1,  0) == b) return Direction::West;
    if (a + sf::Vector2i( 0,  1) == b) return Direction::South;
    return Direction::None;
};
Direction adjacentSide(Vector2s a, Vector2s b) {
    if (a + Vector2s(-1,  0) == b) return Direction::East;
    if (a + Vector2s( 0, -1) == b) return Direction::North;
    if (a + Vector2s( 1,  0) == b) return Direction::West;
    if (a + Vector2s( 0,  1) == b) return Direction::South;
    return Direction::None;
};

// get squared distance between points
inline int sqdist(Vector2s a, Vector2s b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
};
// square number
inline int square(int n) {
    return n * n;
};
// clamp
inline int clamp(int x, int a, int b) {
    if (x <= a) return a;
    if (x >= b) return b;
    return x;
};
// lerp
inline int lerp(int a, int b, float t) {
    return a * (1 - t) + b * t;
};

// string copy
char* copyString(char* str) {
    char* res = new char[strlen(str) + 1];
    for (size_t i = 0; i < strlen(str); i++)
        res[i] = str[i];
    res[strlen(str)] = 0;
    return res;
};
char* copyString(std::vector<char> str) {
    char* res = new char[str.size() + 1];
    for (size_t i = 0; i < str.size(); i++)
        res[i] = str[i];
    res[str.size()] = 0;
    return res;
};

// allocator class
template <class type>
class Stamper {
    public:
    // destructor
    ~Stamper () {
        while (!m_stack.empty())
            m_stack.pop();
    };

    // copy element
    type* copy(type sample) {
        m_stack.push(sample);
        return &m_stack.top();
    };

    private:
    std::stack<type> m_stack;
};

// render rect
void renderRect(sf::VertexArray& vertices, sf::IntRect bounds, sf::IntRect texture, Direction direction = Direction::North) {
    switch (direction) {
        case Direction::North:
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left,                bounds.top),                 sf::Vector2f(texture.left,                 texture.top)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left + bounds.width, bounds.top),                 sf::Vector2f(texture.left + texture.width, texture.top)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height), sf::Vector2f(texture.left + texture.width, texture.top + texture.height)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left,                bounds.top + bounds.height), sf::Vector2f(texture.left,                 texture.top + texture.height)));
        break;
        case Direction::South:
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left,                bounds.top),                 sf::Vector2f(texture.left + texture.width, texture.top + texture.height)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left + bounds.width, bounds.top),                 sf::Vector2f(texture.left,                 texture.top + texture.height)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height), sf::Vector2f(texture.left,                 texture.top)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left,                bounds.top + bounds.height), sf::Vector2f(texture.left + texture.width, texture.top)));
        break;
        case Direction::East:
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left,                bounds.top),                 sf::Vector2f(texture.left + texture.width, texture.top)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left + bounds.width, bounds.top),                 sf::Vector2f(texture.left + texture.width, texture.top + texture.height)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height), sf::Vector2f(texture.left,                 texture.top + texture.height)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left,                bounds.top + bounds.height), sf::Vector2f(texture.left,                 texture.top)));
        break;
        case Direction::West:
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left,                bounds.top),                 sf::Vector2f(texture.left,                 texture.top + texture.height)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left + bounds.width, bounds.top),                 sf::Vector2f(texture.left,                 texture.top)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height), sf::Vector2f(texture.left + texture.width, texture.top)));
        vertices.append(sf::Vertex(sf::Vector2f(bounds.left,                bounds.top + bounds.height), sf::Vector2f(texture.left + texture.width, texture.top + texture.height)));
        break;
        default:
        break;
    };
};
