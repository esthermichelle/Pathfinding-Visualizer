enum NodeState
{
    EMPTY,
    WALL,
    START,
    TARGET,
    FRONTIER,
    VISITED,
    PATH
};


struct Position {
    int row;
    int col;
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};
