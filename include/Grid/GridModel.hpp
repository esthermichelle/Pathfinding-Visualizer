#include "../Core/Types.hpp"
#include "Node.hpp"
#include <vector>

using namespace std;

class GridModel
{
public:
    GridModel(int rows, int cols);

    int getRows() const;
    int getCols() const;

    bool inBounds(Position pos) const;

    Node &getNode(Position pos);

    NodeState getNodeState(Position pos) const;
    void setNodeState(Position pos, NodeState state);

    bool isWall(Position pos) const;

    void resetStates();

private:
    int rows;
    int cols;
    vector<vector<Node>> grid;
};
