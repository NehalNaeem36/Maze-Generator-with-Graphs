#include<iostream>
#include<cstdlib>
#include<ctime>
using namespace std;

/*
Maze generator using graph-based spanning tree algorithm.
The maze is represented as a 2D grid where each tile can connect to its neighbors.
Connections (paths) can be enabled or disabled to create the maze structure.
*/

//------------------------ Tile Class ----------------------------
class tile {
    tile* northNeighbor;
    tile* southNeighbor;
    tile* eastNeighbor;
    tile* westNeighbor;
    bool pathVector[4] = { false, false, false, false }; // N, S, E, W - paths start closed
    
    int x, y;  // Cartesian coordinates of the tile in the maze
    
public:
    tile(int cordx, int cordy) {
        x = cordx;
        y = cordy;
        northNeighbor = nullptr;
        southNeighbor = nullptr;
        eastNeighbor = nullptr;
        westNeighbor = nullptr;
    }
    
    // Neighbor getters
    tile* getNorthNeighbor() { return northNeighbor; }
    tile* getSouthNeighbor() { return southNeighbor; }
    tile* getEastNeighbor() { return eastNeighbor; }
    tile* getWestNeighbor() { return westNeighbor; }

    const tile* getNorthNeighbor() const { return northNeighbor; }
    const tile* getSouthNeighbor() const { return southNeighbor; }
    const tile* getEastNeighbor()  const { return eastNeighbor; }
    const tile* getWestNeighbor()  const { return westNeighbor; }

    // Path state getters
    bool hasNorthPath() const { return pathVector[0]; }
    bool hasSouthPath() const { return pathVector[1]; }
    bool hasEastPath() const { return pathVector[2]; }
    bool hasWestPath() const { return pathVector[3]; }

    // Path state setters
    void setNorthPath(bool enabled) { pathVector[0] = enabled; }
    void setSouthPath(bool enabled) { pathVector[1] = enabled; }
    void setEastPath(bool enabled) { pathVector[2] = enabled; }
    void setWestPath(bool enabled) { pathVector[3] = enabled; }

    // Coordinate getters
    int getX() const { return x; }
    int getY() const { return y; }

    // Neighbor setters - only set if not already set
    bool setNorthNeighbor(tile* n) {
        if (northNeighbor == nullptr) {
            northNeighbor = n;
            return true;
        }
        return false;
    }
    
    bool setSouthNeighbor(tile* n) {
        if (southNeighbor == nullptr) {
            southNeighbor = n;
            return true;
        }
        return false;
    }
    
    bool setEastNeighbor(tile* n) {
        if (eastNeighbor == nullptr) {
            eastNeighbor = n;
            return true;
        }
        return false;
    }
    
    bool setWestNeighbor(tile* n) {
        if (westNeighbor == nullptr) {
            westNeighbor = n;
            return true;
        }
        return false;
    }

    ~tile() {
        northNeighbor = southNeighbor = eastNeighbor = westNeighbor = nullptr;
    }
};

//------------------------ TileList Class ----------------------------
class TileList {
private:
    struct Node {
        int x;
        int y;
        tile* tilePtr;
        Node* next;
        Node* prev;

        Node(int cordx, int cordy, tile* t) : x(cordx), y(cordy), tilePtr(t), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;

public:
    TileList() : head(nullptr), tail(nullptr) {}

    bool isVisited(int x, int y) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->x == x && current->y == y) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    tile* getTileAt(int x, int y) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->x == x && current->y == y) {
                return current->tilePtr;
            }
            current = current->next;
        }
        return nullptr;
    }

    bool add(int x, int y, tile* t) {
        if (isVisited(x, y)) {
            return false;
        }

        Node* newNode = new Node(x, y, t);

        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        return true;
    }

    Node* getHead() { return head; }
    Node* getTail() { return tail; }
    const Node* getHead() const { return head; }
    const Node* getTail() const { return tail; }

    ~TileList() {
        clear();
    }

    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
    }
};

//------------------------ Maze Class ----------------------------
class maze {
    TileList* visitedList;  // Temporary traversal tracking
    TileList* allTiles;     // Permanent storage of all tiles
    TileList* shortestPath; // Stores the shortest path from start to end
    tile* startingTile;
    tile* endingTile;

    int maxX, maxY;
    int maxTiles;
    int tilesCreated;

private:
    // Helper function to check if coordinates are within bounds
    bool isInBounds(int x, int y) {
        return (x >= 0 && x < maxX && y >= 0 && y < maxY);
    }

    // Recursive function to initialize tiles
    void initTilesRecursive(int x, int y) {
        // Check if out of bounds
        if (!isInBounds(x, y)) {
            return;
        }

        // Check if tile already exists at this location
        if (visitedList->isVisited(x, y)) {
            return;
        }

        // Create new tile and add to both visited list and allTiles list
        tile* currentTile = new tile(x, y);
        visitedList->add(x, y, currentTile);
        allTiles->add(x, y, currentTile);  // Also add to permanent list
        tilesCreated++;

        // Try North (y-1)
        if (isInBounds(x, y - 1)) {
            tile* northTile = visitedList->getTileAt(x, y - 1);
            if (northTile != nullptr) {
                // Tile exists, connect it
                currentTile->setNorthNeighbor(northTile);
                northTile->setSouthNeighbor(currentTile);
            }
            else {
                // Tile doesn't exist, recurse
                initTilesRecursive(x, y - 1);
                northTile = visitedList->getTileAt(x, y - 1);
                currentTile->setNorthNeighbor(northTile);
                northTile->setSouthNeighbor(currentTile);
            }
        }

        // Try South (y+1)
        if (isInBounds(x, y + 1)) {
            tile* southTile = visitedList->getTileAt(x, y + 1);
            if (southTile != nullptr) {
                currentTile->setSouthNeighbor(southTile);
                southTile->setNorthNeighbor(currentTile);
            }
            else {
                initTilesRecursive(x, y + 1);
                southTile = visitedList->getTileAt(x, y + 1);
                currentTile->setSouthNeighbor(southTile);
                southTile->setNorthNeighbor(currentTile);
            }
        }

        // Try East (x+1)
        if (isInBounds(x + 1, y)) {
            tile* eastTile = visitedList->getTileAt(x + 1, y);
            if (eastTile != nullptr) {
                currentTile->setEastNeighbor(eastTile);
                eastTile->setWestNeighbor(currentTile);
            }
            else {
                initTilesRecursive(x + 1, y);
                eastTile = visitedList->getTileAt(x + 1, y);
                currentTile->setEastNeighbor(eastTile);
                eastTile->setWestNeighbor(currentTile);
            }
        }

        // Try West (x-1)
        if (isInBounds(x - 1, y)) {
            tile* westTile = visitedList->getTileAt(x - 1, y);
            if (westTile != nullptr) {
                currentTile->setWestNeighbor(westTile);
                westTile->setEastNeighbor(currentTile);
            }
            else {
                initTilesRecursive(x - 1, y);
                westTile = visitedList->getTileAt(x - 1, y);
                currentTile->setWestNeighbor(westTile);
                westTile->setEastNeighbor(currentTile);
            }
        }

        return;
    }

    // Helper function to open path between two tiles bidirectionally
    void openPathBetween(tile* t1, tile* t2) {
        if (t1->getNorthNeighbor() == t2) {
            t1->setNorthPath(true);
            t2->setSouthPath(true);
        }
        else if (t1->getSouthNeighbor() == t2) {
            t1->setSouthPath(true);
            t2->setNorthPath(true);
        }
        else if (t1->getEastNeighbor() == t2) {
            t1->setEastPath(true);
            t2->setWestPath(true);
        }
        else if (t1->getWestNeighbor() == t2) {
            t1->setWestPath(true);
            t2->setEastPath(true);
        }
    }

    // Recursive DFS to carve spanning tree
    void carveMazeRecursive(tile* current) {
        // Mark as visited
        visitedList->add(current->getX(), current->getY(), current);
        
        // Collect unvisited neighbors
        tile* neighbors[4] = {nullptr, nullptr, nullptr, nullptr};
        int neighborCount = 0;
        
        // Check North
        if (current->getNorthNeighbor()) {
            tile* n = current->getNorthNeighbor();
            if (!visitedList->isVisited(n->getX(), n->getY())) {
                neighbors[neighborCount++] = n;
            }
        }
        
        // Check South
        if (current->getSouthNeighbor()) {
            tile* s = current->getSouthNeighbor();
            if (!visitedList->isVisited(s->getX(), s->getY())) {
                neighbors[neighborCount++] = s;
            }
        }
        
        // Check East
        if (current->getEastNeighbor()) {
            tile* e = current->getEastNeighbor();
            if (!visitedList->isVisited(e->getX(), e->getY())) {
                neighbors[neighborCount++] = e;
            }
        }
        
        // Check West
        if (current->getWestNeighbor()) {
            tile* w = current->getWestNeighbor();
            if (!visitedList->isVisited(w->getX(), w->getY())) {
                neighbors[neighborCount++] = w;
            }
        }
        
        // Shuffle neighbors for randomness
        for (int i = neighborCount - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            tile* temp = neighbors[i];
            neighbors[i] = neighbors[j];
            neighbors[j] = temp;
        }
        
        // Visit each unvisited neighbor
        for (int i = 0; i < neighborCount; i++) {
            tile* neighbor = neighbors[i];
            
            // Check again if unvisited (might have been visited by another branch)
            if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                // Open path bidirectionally
                openPathBetween(current, neighbor);
                
                // Recurse
                carveMazeRecursive(neighbor);
            }
        }
    }

    // Helper structure for BFS pathfinding
    struct BFSNode {
        tile* currentTile;
        tile* parent;
        BFSNode* next;
        
        BFSNode(tile* t, tile* p) : currentTile(t), parent(p), next(nullptr) {}
    };

    // Simple queue for BFS
    class BFSQueue {
    private:
        BFSNode* head;
        BFSNode* tail;
    public:
        BFSQueue() : head(nullptr), tail(nullptr) {}
        
        void enqueue(tile* t, tile* parent) {
            BFSNode* newNode = new BFSNode(t, parent);
            if (tail == nullptr) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        
        BFSNode* dequeue() {
            if (head == nullptr) return nullptr;
            BFSNode* temp = head;
            head = head->next;
            if (head == nullptr) tail = nullptr;
            return temp;
        }
        
        bool isEmpty() { return head == nullptr; }
        
        ~BFSQueue() {
            while (head != nullptr) {
                BFSNode* temp = head;
                head = head->next;
                delete temp;
            }
        }
    };

    // Find shortest path using BFS
    void findShortestPath() {
        // Clear any existing path
        if (shortestPath != nullptr) {
            shortestPath->clear();
        }
        
        // Clear visited list for pathfinding
        visitedList->clear();
        
        // Parent tracking - stores parent tile for each visited tile
        TileList parentMap;
        
        // BFS queue
        BFSQueue queue;
        queue.enqueue(startingTile, nullptr);
        visitedList->add(startingTile->getX(), startingTile->getY(), startingTile);
        parentMap.add(startingTile->getX(), startingTile->getY(), nullptr);
        
        bool pathFound = false;
        
        // BFS traversal - visitedList prevents revisiting nodes (handles cycles)
        while (!queue.isEmpty()) {
            BFSNode* current = queue.dequeue();
            tile* currentTile = current->currentTile;
            
            // Check if we reached the end
            if (currentTile == endingTile) {
                pathFound = true;
                delete current;
                break;
            }
            
            // Check all neighbors with open paths
            if (currentTile->hasNorthPath() && currentTile->getNorthNeighbor()) {
                tile* neighbor = currentTile->getNorthNeighbor();
                // visitedList prevents cycles - only visit unvisited tiles
                if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                    visitedList->add(neighbor->getX(), neighbor->getY(), neighbor);
                    parentMap.add(neighbor->getX(), neighbor->getY(), currentTile);
                    queue.enqueue(neighbor, currentTile);
                }
            }
            
            if (currentTile->hasSouthPath() && currentTile->getSouthNeighbor()) {
                tile* neighbor = currentTile->getSouthNeighbor();
                if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                    visitedList->add(neighbor->getX(), neighbor->getY(), neighbor);
                    parentMap.add(neighbor->getX(), neighbor->getY(), currentTile);
                    queue.enqueue(neighbor, currentTile);
                }
            }
            
            if (currentTile->hasEastPath() && currentTile->getEastNeighbor()) {
                tile* neighbor = currentTile->getEastNeighbor();
                if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                    visitedList->add(neighbor->getX(), neighbor->getY(), neighbor);
                    parentMap.add(neighbor->getX(), neighbor->getY(), currentTile);
                    queue.enqueue(neighbor, currentTile);
                }
            }
            
            if (currentTile->hasWestPath() && currentTile->getWestNeighbor()) {
                tile* neighbor = currentTile->getWestNeighbor();
                if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                    visitedList->add(neighbor->getX(), neighbor->getY(), neighbor);
                    parentMap.add(neighbor->getX(), neighbor->getY(), currentTile);
                    queue.enqueue(neighbor, currentTile);
                }
            }
            
            delete current;
        }
        
        if (!pathFound) {
            cout << "ERROR: No path found from start to end after modifications!" << endl;
            cout << "Maze may have become disconnected." << endl;
            visitedList->clear();
            return;
        }
        
        // Reconstruct path by backtracking from end to start
        tile* current = endingTile;
        TileList reversePath;
        
        while (current != nullptr) {
            reversePath.add(current->getX(), current->getY(), current);
            current = parentMap.getTileAt(current->getX(), current->getY());
        }
        
        // Reverse the path to get start->end order
        auto* revNode = reversePath.getHead();
        while (revNode != nullptr) {
            shortestPath->add(revNode->x, revNode->y, revNode->tilePtr);
            revNode = revNode->next;
        }
        
        // Clear visited list for future use
        visitedList->clear();
        
        cout << "New shortest path found with " << getPathLength() << " tiles!" << endl;
    }

    // Recalculate shortest path after maze modifications
    void recalculateShortestPath() {
        cout << "Recalculating shortest path after modifications..." << endl;
        
        // Clear any existing path
        if (shortestPath != nullptr) {
            shortestPath->clear();
        }
        
        // Clear visited list for pathfinding
        visitedList->clear();
        
        // Parent tracking - stores parent tile for each visited tile
        TileList parentMap;
        
        // BFS queue
        BFSQueue queue;
        queue.enqueue(startingTile, nullptr);
        visitedList->add(startingTile->getX(), startingTile->getY(), startingTile);
        parentMap.add(startingTile->getX(), startingTile->getY(), nullptr);
        
        bool pathFound = false;
        
        // BFS traversal - visitedList prevents revisiting nodes (handles cycles)
        while (!queue.isEmpty()) {
            BFSNode* current = queue.dequeue();
            tile* currentTile = current->currentTile;
            
            // Check if we reached the end
            if (currentTile == endingTile) {
                pathFound = true;
                delete current;
                break;
            }
            
            // Check all neighbors with open paths
            if (currentTile->hasNorthPath() && currentTile->getNorthNeighbor()) {
                tile* neighbor = currentTile->getNorthNeighbor();
                // visitedList prevents cycles - only visit unvisited tiles
                if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                    visitedList->add(neighbor->getX(), neighbor->getY(), neighbor);
                    parentMap.add(neighbor->getX(), neighbor->getY(), currentTile);
                    queue.enqueue(neighbor, currentTile);
                }
            }
            
            if (currentTile->hasSouthPath() && currentTile->getSouthNeighbor()) {
                tile* neighbor = currentTile->getSouthNeighbor();
                if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                    visitedList->add(neighbor->getX(), neighbor->getY(), neighbor);
                    parentMap.add(neighbor->getX(), neighbor->getY(), currentTile);
                    queue.enqueue(neighbor, currentTile);
                }
            }
            
            if (currentTile->hasEastPath() && currentTile->getEastNeighbor()) {
                tile* neighbor = currentTile->getEastNeighbor();
                if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                    visitedList->add(neighbor->getX(), neighbor->getY(), neighbor);
                    parentMap.add(neighbor->getX(), neighbor->getY(), currentTile);
                    queue.enqueue(neighbor, currentTile);
                }
            }
            
            if (currentTile->hasWestPath() && currentTile->getWestNeighbor()) {
                tile* neighbor = currentTile->getWestNeighbor();
                if (!visitedList->isVisited(neighbor->getX(), neighbor->getY())) {
                    visitedList->add(neighbor->getX(), neighbor->getY(), neighbor);
                    parentMap.add(neighbor->getX(), neighbor->getY(), currentTile);
                    queue.enqueue(neighbor, currentTile);
                }
            }
            
            delete current;
        }
        
        if (!pathFound) {
            cout << "ERROR: No path found from start to end after modifications!" << endl;
            cout << "Maze may have become disconnected." << endl;
            visitedList->clear();
            return;
        }
        
        // Reconstruct path by backtracking from end to start
        tile* current = endingTile;
        TileList reversePath;
        
        while (current != nullptr) {
            reversePath.add(current->getX(), current->getY(), current);
            current = parentMap.getTileAt(current->getX(), current->getY());
        }
        
        // Reverse the path to get start->end order
        auto* revNode = reversePath.getHead();
        while (revNode != nullptr) {
            shortestPath->add(revNode->x, revNode->y, revNode->tilePtr);
            revNode = revNode->next;
        }
        
        // Clear visited list for future use
        visitedList->clear();
        
        cout << "New shortest path found with " << getPathLength() << " tiles!" << endl;
    }

    // Get the length of the shortest path
    int getPathLength() {
        if (shortestPath == nullptr) return 0;
        
        int count = 0;
        auto* current = shortestPath->getHead();
        while (current != nullptr) {
            count++;
            current = current->next;
        }
        return count;
    }

    // Check if a tile is in the shortest path
    bool isInShortestPath(tile* t) {
        if (shortestPath == nullptr) return false;
        
        auto* current = shortestPath->getHead();
        while (current != nullptr) {
            if (current->tilePtr == t) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // Apply 90% preservation randomization while protecting shortest path
    void addRandomExtraPaths(float probability) {
        auto* current = allTiles->getHead();
        int pathsAdded = 0;
        int pathsClosed = 0;
        int tilesSkipped = 0;
        
        // Create a copy of shortest path to check against
        TileList originalShortestPath;
        auto* pathNode = shortestPath->getHead();
        while (pathNode != nullptr) {
            originalShortestPath.add(pathNode->x, pathNode->y, pathNode->tilePtr);
            pathNode = pathNode->next;
        }
        
        while (current != nullptr) {
            tile* t = current->tilePtr;
            
            // Skip tiles in the shortest path completely
            bool inOriginalPath = originalShortestPath.getTileAt(t->getX(), t->getY()) != nullptr;
            if (t == startingTile || t == endingTile || inOriginalPath) {
                tilesSkipped++;
                current = current->next;
                continue;
            }
            
            // Process each direction
            tile* neighbors[4] = {
                t->getNorthNeighbor(),
                t->getSouthNeighbor(),
                t->getEastNeighbor(),
                t->getWestNeighbor()
            };
            
            for (int dir = 0; dir < 4; dir++) {
                tile* neighbor = neighbors[dir];
                if (!neighbor) continue;
                
                bool neighborInPath = originalShortestPath.getTileAt(neighbor->getX(), neighbor->getY()) != nullptr;
                
                // Don't modify connections to path/start/end tiles
                if (!neighborInPath && neighbor != startingTile && neighbor != endingTile) {
                    bool currentlyOpen = false;
                    
                    // Check if path is open
                    switch(dir) {
                        case 0: currentlyOpen = t->hasNorthPath(); break;
                        case 1: currentlyOpen = t->hasSouthPath(); break;
                        case 2: currentlyOpen = t->hasEastPath(); break;
                        case 3: currentlyOpen = t->hasWestPath(); break;
                    }
                    
                    float randomValue = rand() / (float)RAND_MAX;
                    
                    if (currentlyOpen) {
                        // 10% chance to close
                        if (randomValue >= 0.90) {
                            switch(dir) {
                                case 0:
                                    t->setNorthPath(false);
                                    neighbor->setSouthPath(false);
                                    break;
                                case 1:
                                    t->setSouthPath(false);
                                    neighbor->setNorthPath(false);
                                    break;
                                case 2:
                                    t->setEastPath(false);
                                    neighbor->setWestPath(false);
                                    break;
                                case 3:
                                    t->setWestPath(false);
                                    neighbor->setEastPath(false);
                                    break;
                            }
                            pathsClosed++;
                        }
                    } else {
                        // 10% chance to open
                        if (randomValue >= 0.90) {
                            openPathBetween(t, neighbor);
                            pathsAdded++;
                        }
                    }
                }
            }
            
            current = current->next;
        }
        
        cout << "Paths modified: " << pathsAdded << " opened, " << pathsClosed << " closed" << endl;
        cout << "Tiles in shortest path skipped: " << tilesSkipped << endl;
        cout << "Connections to start/end/path tiles PRESERVED" << endl;
        cout << "Spanning tree preserved: ~90% of original structure maintained" << endl;
    }

public:
    maze(int width, int height) {
        if (width <= 0 || height <= 0) {
            cout << "Warning: Invalid maze dimensions. Defaulting to 10x10." << endl;
            width = 10;
            height = 10;
        }
        maxX = width;
        maxY = height;
        maxTiles = maxX * maxY;
        tilesCreated = 0;
        visitedList = new TileList();
        allTiles = new TileList();
        shortestPath = new TileList();
        srand(time(0));
        
        cout << "\n=== Initializing Maze ===" << endl;
        initTiles();
        
        cout << "\n=== Generating Maze Paths ===" << endl;
        generateMazePaths();
        
        cout << "\n=== Maze Ready ===" << endl;
    }

    void initTiles() {
        // Start recursion from (0,0)
        initTilesRecursive(0, 0);

        // Set starting tile at (0,0)
        startingTile = visitedList->getTileAt(0, 0);

        // Generate random ending tile coordinates within bounds
        int endX = rand() % maxX;
        int endY = rand() % maxY;
        endingTile = visitedList->getTileAt(endX, endY);

        // Clear the visited list for reuse in maze generation/traversal
        visitedList->clear();
    }

    void generateMazePaths() {
        visitedList->clear();
        
        // STEP 1: Create spanning tree
        carveMazeRecursive(startingTile);
        visitedList->clear();
        cout << "Spanning tree generated successfully!" << endl;
        
        // STEP 2: Find shortest path in spanning tree
        findShortestPath();
        
        // STEP 3: Apply 90% preservation randomization
        addRandomExtraPaths(0.90);
        
        // STEP 4: Recalculate shortest path
        recalculateShortestPath();
        
        cout << "Maze generation complete!" << endl;
    }

    ~maze() {
        auto* current = allTiles->getHead();
        while (current != nullptr) {
            delete current->tilePtr;
            current = current->next;
        }
        delete allTiles;
        delete visitedList;
        delete shortestPath;
    }

    void validateMaze() {
        cout << "\n===== Maze Validation =====" << endl;
        cout << "Expected tiles: " << maxTiles << endl;
        cout << "Tiles created: " << tilesCreated << endl;
        cout << "Maze bounds: X[0-" << maxX - 1 << "], Y[0-" << maxY - 1 << "]" << endl;
        cout << "Starting Tile: (0, 0)" << endl;
        cout << "Ending Tile: (" << endingTile->getX() << ", " << endingTile->getY() << ")" << endl;
        cout << "Shortest Path Length: " << getPathLength() << " tiles" << endl;
        cout << "\n--- All Tiles Details ---" << endl;

        auto* current = allTiles->getHead();
        int count = 0;
        int validCount = 0;
        int invalidCount = 0;
        int pathTile_count = 0;

        while (current != nullptr) {
            count++;
            tile* t = current->tilePtr;
            int tx = current->x;
            int ty = current->y;

            // Check if tile is within bounds
            bool valid = isInBounds(tx, ty);
            if (valid) validCount++;
            else invalidCount++;
            
            // Check if tile is in shortest path
            bool inPath = isInShortestPath(t);
            if (inPath) pathTile_count++;

            cout << "Tile #" << count << ": (" << tx << ", " << ty << ")";
            cout << " [" << (valid ? "VALID" : "INVALID") << "]";
            
            // Mark special tiles
            if (t == startingTile) cout << " [START]";
            else if (t == endingTile) cout << " [END]";
            else if (inPath) cout << " [PATH]";

            // Count open paths
            int openPathCount = 0;
            if (t->hasNorthPath()) openPathCount++;
            if (t->hasSouthPath()) openPathCount++;
            if (t->hasEastPath()) openPathCount++;
            if (t->hasWestPath()) openPathCount++;

            // Count neighbors
            int neighborCount = 0;
            if (t->getNorthNeighbor() != nullptr) neighborCount++;
            if (t->getSouthNeighbor() != nullptr) neighborCount++;
            if (t->getEastNeighbor() != nullptr) neighborCount++;
            if (t->getWestNeighbor() != nullptr) neighborCount++;

            cout << " - Neighbors: " << neighborCount;
            cout << " (N:" << (t->getNorthNeighbor() ? "Y" : "N");
            cout << " S:" << (t->getSouthNeighbor() ? "Y" : "N");
            cout << " E:" << (t->getEastNeighbor() ? "Y" : "N");
            cout << " W:" << (t->getWestNeighbor() ? "Y" : "N") << ")";
            
            cout << " - Open Paths: " << openPathCount;
            cout << " (N:" << (t->hasNorthPath() ? "O" : "C");
            cout << " S:" << (t->hasSouthPath() ? "O" : "C");
            cout << " E:" << (t->hasEastPath() ? "O" : "C");
            cout << " W:" << (t->hasWestPath() ? "O" : "C") << ")" << endl;

            current = current->next;
        }

        cout << "\n--- Summary ---" << endl;
        cout << "Total tiles in list: " << count << endl;
        cout << "Valid tiles: " << validCount << endl;
        cout << "Invalid tiles: " << invalidCount << endl;
        cout << "Tiles in shortest path: " << pathTile_count << endl;

        if (count == maxTiles && invalidCount == 0 && pathTile_count == getPathLength()) {
            cout << "Maze validation PASSED!" << endl;
        }
        else {
            cout << "Maze validation FAILED!" << endl;
        }
        cout << "========================" << endl;
    }

    void displayShortestPath() {
        cout << "\n===== Shortest Path =====" << endl;
        
        if (shortestPath == nullptr || shortestPath->getHead() == nullptr) {
            cout << "No path available!" << endl;
            return;
        }
        
        cout << "Path length: " << getPathLength() << " tiles" << endl;
        cout << "Path: ";
        
        auto* current = shortestPath->getHead();
        int step = 0;
        while (current != nullptr) {
            cout << "(" << current->x << "," << current->y << ")";
            if (current->next != nullptr) {
                cout << " -> ";
            }
            current = current->next;
            step++;
            
            // Line break every 5 steps for readability
            if (step % 5 == 0 && current != nullptr) {
                cout << "\n        ";
            }
        }
        cout << endl;
        cout << "========================" << endl;
    }

    void printMazeASCII() {
        cout << "\n===== Maze ASCII Visualization =====" << endl;
        cout << "Legend: S=Start, E=End, *=Path, +=Tile, Walls=lines" << endl;
        cout << endl;
        
        // Print maze row by row
        for (int y = 0; y < maxY; y++) {
            // Print top walls for this row
            for (int x = 0; x < maxX; x++) {
                tile* t = allTiles->getTileAt(x, y);
                
                // Top-left corner
                cout << "+";
                
                // Top wall
                if (t && t->hasNorthPath() && t->getNorthNeighbor()) {
                    cout << "   ";  // Open path (3 spaces)
                } else {
                    cout << "---";  // Wall
                }
            }
            cout << "+" << endl;  // Close the row
            
            // Print left walls and tile contents
            for (int x = 0; x < maxX; x++) {
                tile* t = allTiles->getTileAt(x, y);
                
                // Left wall
                if (t && t->hasWestPath() && t->getWestNeighbor()) {
                    cout << " ";  // Open path
                } else {
                    cout << "|";  // Wall
                }
                
                // Tile content
                if (t == startingTile) {
                    cout << " S ";  // Start
                } else if (t == endingTile) {
                    cout << " E ";  // End
                } else if (isInShortestPath(t)) {
                    cout << " * ";  // Path
                } else {
                    cout << "   ";  // Empty
                }
            }
            
            // Right wall (always closed - boundary)
            cout << "|" << endl;
        }
        
        // Print bottom boundary
        for (int x = 0; x < maxX; x++) {
            cout << "+---";
        }
        cout << "+" << endl;
        
        cout << "====================================" << endl;
    }
};

int main() {
    maze myMaze(10, 10);
    myMaze.printMazeASCII();
    myMaze.displayShortestPath();
    myMaze.validateMaze();
    return 0;
}