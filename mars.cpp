#include <iostream>
#include <vector>
#include <stdexcept>
#include<bits/stdc++.h>

class Rover;
class GridComponent {
public:
    virtual ~GridComponent() = default;
};

class Obstacle : public GridComponent {};

class Grid : public GridComponent {
private:
    std::vector<std::vector<GridComponent*>> gridComponents;

public:
    Grid(int gridSizeX, int gridSizeY) {
        if(gridSizeX<0 && gridSizeY<0) std::cout<<"Only positive values are allowed";
        gridComponents.resize(gridSizeX, std::vector<GridComponent*>(gridSizeY, nullptr));
    }

    void addObstacle(int x, int y) {
        if (gridComponents[x][y] == nullptr) {
            gridComponents[x][y] = new Obstacle();
        }
    }

    GridComponent* gridComponentAt(int x, int y) const {
        return gridComponents[x][y];
    }

    ~Grid() {
        for (auto& row : gridComponents) {
            for (auto& component : row) {
                delete component;
            }
        }
    }
};
//Abstract class
class RoverCommand {
public:
    virtual void execute(Rover& rover, Grid& grid) = 0;
};

class MoveCommand : public RoverCommand {
public:
    void execute(Rover& rover, Grid& grid) override;
};

class TurnLeftCommand : public RoverCommand {
public:
    void execute(Rover& rover, Grid& grid) override;
};

class TurnRightCommand : public RoverCommand {
public:
    void execute(Rover& rover, Grid& grid) override;
};

class Rover {
//Encapsulation
private:
    int x, y;
    char direction;
    int gridSizeX;
    int gridSizeY;
    bool obstacleDetected;

public:
    Rover(int x, int y, char direction, int gridSizeX, int gridSizeY)
        : direction(direction), gridSizeX(gridSizeX), gridSizeY(gridSizeY), obstacleDetected(false) {
        // Check if the starting position is within the grid boundaries
        if (x < 0 || x >= gridSizeX || y < 0 || y >= gridSizeY) {
            throw std::invalid_argument("Starting position is outside the grid boundaries.");
        }
        if (direction != 'N' && direction != 'W' && direction != 'S' && direction != 'E') {
            throw std::invalid_argument("Invalid initial direction.");
        }
        // Initialize the rover's position
        this->x = x;
        this->y = y;
    }

    void move(Grid& grid) {
        // Save current position for the status report
        int prevX = x;
        int prevY = y;

        // Implementation for moving the rover
        if (direction == 'N' && y < gridSizeY - 1) {
            y++;
        } else if (direction == 'S' && y > 0) {
            y--;
        } else if (direction == 'E' && x < gridSizeX - 1) {
            x++;
        } else if (direction == 'W' && x > 0) {
            x--;
        }

        // Check for obstacles after the move
        if (obstacleAhead(x, y, grid)) {
            // If obstacle detected, set the flag
            obstacleDetected = true;
            // Revert to the previous position
            x = prevX;
            y = prevY;
        }
    } 

    void turnLeft() {
        // Implementation for turning left
        switch (direction) {
        case 'N':
            direction = 'W';
            break;
        case 'E':
            direction = 'N';
            break;
        case 'S':
            direction = 'E';
            break;
        case 'W':
            direction = 'S';
            break;
    }
    }

    void turnRight() 
    {
        switch (direction) {
        case 'N':
            direction = 'E';
            break;
        case 'E':
            direction = 'S';
            break;
        case 'S':
            direction = 'W';
            break;
        case 'W':
            direction = 'N';
            break;
    }
}

    

    bool obstacleAhead(int nextX, int nextY, Grid& grid) const {
        // Check if there is an obstacle at the next position
        return dynamic_cast<Obstacle*>(grid.gridComponentAt(nextX, nextY)) != nullptr;
    }

    // Getter methods
    bool hasObstacleDetected() const { return obstacleDetected; }
    int getX() const { return x; }
    int getY() const { return y; }
    char getDirection() const { return direction; }
};

void MoveCommand::execute(Rover& rover, Grid& grid) {
    rover.move(grid);
}

void TurnLeftCommand::execute(Rover& rover, Grid& grid) {
    rover.turnLeft();
}

void TurnRightCommand::execute(Rover& rover, Grid& grid) {
    rover.turnRight();
}

int main() {
    // Input
    int gridSizeX, gridSizeY;
    std::cout << "Enter grid size (X Y): ";
    std::cin >> gridSizeX >> gridSizeY;
    int startX, startY;
    char startDirection;
    std::cout << "Enter starting position (X Y Direction of Rover): ";
    std::cin >> startX >> startY >> startDirection;

    try {
        Grid grid(gridSizeX, gridSizeY);
        Rover rover(startX, startY, startDirection, gridSizeX, gridSizeY);

        int numObstacles;
        std::cout << "Enter the number of obstacles: ";
        std::cin >> numObstacles;

        for (int i = 0; i < numObstacles; ++i) {
            int obsX, obsY;
            std::cout << "Enter obstacle " << i + 1 << " position (X Y): ";
            std::cin >> obsX >> obsY;

            while (obsX < 0 || obsX >= gridSizeX || obsY < 0 || obsY >= gridSizeY) {
                std::cout << "Obstacle position is outside the grid boundaries.";
                std::cout << "Enter obstacle " << i + 1 << " position (X Y): ";
                std::cin >> obsX >> obsY;
                   
            }
            if(startX == obsX && obsY == startY)  throw std::invalid_argument("Invalid! Rover is present on the location");
           
            grid.addObstacle(obsX, obsY);
        }

        std::vector<RoverCommand*> commands;
        char command;
        std::cout << "Enter commands (M/L/R, Q to stop): ";
        while (std::cin >> command && command != 'Q') {
            switch (command) {
                case 'M':
                    commands.push_back(new MoveCommand());
                    break;
                case 'L':
                    commands.push_back(new TurnLeftCommand());
                    break;
                case 'R':
                    commands.push_back(new TurnRightCommand());
                    break;
                default:
                    std::cout << "Invalid command\n";
                    break;
            }
        }

        // Execution of commands
        for (RoverCommand* cmd : commands) {
            cmd->execute(rover, grid);
        }

        // Example: Getting final position and status report
        std::cout << "Final Position: (" << rover.getX() << ", " << rover.getY() << ", " << rover.getDirection() << ")\n";

        // Status report
        if (rover.getX() == startX && rover.getY() == startY && rover.hasObstacleDetected()) {
            std::cout << "Status Report: Rover did not move. Obstacles detected.\n";
        } else if (rover.getX() == startX && rover.getY() == startY) {
            std::cout << "Status Report: Rover did not move. No obstacles detected.\n";
        } else if(rover.hasObstacleDetected()){
            std::cout << "Status Report: Rover is at (" << rover.getX() << ", " << rover.getY() << ") facing "
                      << rover.getDirection() << ". Obstacle detected.\n";
        }else{
            std::cout << "Status Report: Rover is at (" << rover.getX() << ", " << rover.getY() << ") facing "
                      << rover.getDirection() << ". No obstacle detected.\n";
            
        }
        
        // Clean up allocated memory
        for (RoverCommand* cmd : commands) {
            delete cmd;
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;  // Return a non-zero value to indicate an error
    }

    return 0;  // Return 0 to indicate successful execution
}
