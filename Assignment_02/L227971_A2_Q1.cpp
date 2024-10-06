#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

bool validate_ship_placement(int x, int y, char dir, int size, char grid[5][5])
{
    if (dir == 'H')
    {
        if (x + size > 5)
        {
            return false;
        }
        for (int i = x; i < x + size; i++)
        {
            if (grid[y][i] != '.')
            {
                return false;
            }
        }
    }
    else if (dir == 'V')
    {
        if (y + size > 5)
        {
            return false;
        }
        for (int i = y; i < y + size; i++)
        {
            if (grid[i][x] != '.')
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

void print_grid(char grid[5][5])
{
    cout << "  1 2 3 4 5" << endl;
    for (int i = 0; i < 5; i++)
    {
        cout << i + 1 << " ";
        for (int j = 0; j < 5; j++)
        {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
}

bool check_death(char grid[5][5])
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (grid[i][j] == 'S')
            {
                return false;
            }
        }
    }
    return true;
}

void print_both_grids(char player_grid[5][5], char opponent_grid[5][5])
{
    cout << "Your Grid:          Opponent's Grid:" << endl;
    cout << "  1 2 3 4 5           1 2 3 4 5" << endl;
    for (int i = 0; i < 5; i++)
    {
        cout << i + 1 << " ";
        for (int j = 0; j < 5; j++)
        {
            cout << player_grid[i][j] << " ";
        }
        cout << "        " << i + 1 << " ";
        for (int j = 0; j < 5; j++)
        {
            if (opponent_grid[i][j] == 'S')
            {
                cout << ". ";
            }
            else
            {
                cout << opponent_grid[i][j] << " ";
            }
        }
        cout << endl;
    }
}

int main()
{
    char grid[5][5] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}};
    char opponent_grid[5][5] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}};
    int x = -1;           // 1 to 5
    int y = -1;           // 1 to 5
    char direction = ' '; // H for horizontal, V for vertical

    int p_to_c[2]; // Parent to child
    int c_to_p[2]; // Child to parent

    pipe(p_to_c);
    pipe(c_to_p);

    if (pipe(p_to_c) == -1 || pipe(c_to_p) == -1)
    {
        cout << "Pipe creation failed." << endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        cout << "Fork failed." << endl;
        return 1;
    }

    if (pid == 0) // Child AKA Player 1
    {
        close(p_to_c[1]);
        close(c_to_p[0]);
        cout << "Entering data for Player 1: " << endl;
        int ship_size = 2; // 3 ships with length 2, 3, 4
        print_grid(grid);
        while (true)
        {
            cout << "For ship of size: " << ship_size << endl;
            cout << "Enter x: ";
            cin >> x;
            while (x < 1 || x > 5)
            {
                cout << "Invalid input. Enter x: ";
                cin >> x;
            }
            cout << "Enter y: ";
            cin >> y;
            while (y < 1 || y > 5)
            {
                cout << "Invalid input. Enter y: ";
                cin >> y;
            }
            cout << "Enter direction (H or V): ";
            cin >> direction;
            while (direction != 'H' && direction != 'V')
            {
                cout << "Invalid input. Enter direction (H or V): ";
                cin >> direction;
            }

            if (validate_ship_placement(x - 1, y - 1, direction, ship_size, grid))
            {
                if (direction == 'H')
                {
                    for (int i = x - 1; i < x - 1 + ship_size; i++)
                    {
                        grid[y - 1][i] = 'S';
                    }
                }
                else
                {
                    for (int i = y - 1; i < y - 1 + ship_size; i++)
                    {
                        grid[i][x - 1] = 'S';
                    }
                }
                cout << "Ship placed successfully." << endl;
                print_grid(grid);
                if (ship_size == 4)
                {
                    break;
                }
                else
                {
                    ship_size++;
                }
            }
            else
            {
                cout << "Invalid ship placement. Try again." << endl;
            }
        }

        int placed = 1;
        write(c_to_p[1], &placed, sizeof(placed));
        read(p_to_c[0], &placed, sizeof(placed)); // player 2 grid is ready

        char data_passed;

        while (true)
        {
            cout << "Player 1 turn: " << endl;
            print_both_grids(grid, opponent_grid);
            cout << "Enter x: ";
            cin >> x;
            while (x < 1 || x > 5)
            {
                cout << "Invalid input. Enter x: ";
                cin >> x;
            }
            cout << "Enter y: ";
            cin >> y;
            while (y < 1 || y > 5)
            {
                cout << "Invalid input. Enter y: ";
                cin >> y;
            }
            write(c_to_p[1], &x, sizeof(x));
            write(c_to_p[1], &y, sizeof(y));
            read(p_to_c[0], &data_passed, sizeof(data_passed)); // H or M for hit or miss
            if (data_passed == 'H')
            {
                cout << "Hit!" << endl;
                opponent_grid[y - 1][x - 1] = 'X';
            }
            else
            {
                cout << "Miss!" << endl;
                opponent_grid[y - 1][x - 1] = 'O';
            }
            read(p_to_c[0], &data_passed, sizeof(data_passed)); // D for death
            if (data_passed == 'D')
            {
                cout << "Player 1 wins!" << endl;
                print_both_grids(grid, opponent_grid);
                exit(0);
            }
            data_passed = 'C'; // change turn
            write(c_to_p[1], &data_passed, sizeof(data_passed));

            read(p_to_c[0], &x, sizeof(x));
            read(p_to_c[0], &y, sizeof(y));
            if (grid[y - 1][x - 1] == 'S')
            {
                grid[y - 1][x - 1] = 'X';
                data_passed = 'H';
            }
            else
            {
                data_passed = 'M';
            }
            write(c_to_p[1], &data_passed, sizeof(data_passed));
            if (check_death(grid))
            {
                data_passed = 'D'; // death
                write(c_to_p[1], &data_passed, sizeof(data_passed));
                exit(0);
            }
            else // death not achieved
            {
                data_passed = 'N'; // not dead
                write(c_to_p[1], &data_passed, sizeof(data_passed));
            }

            read(p_to_c[0], &data_passed, sizeof(data_passed)); // change turn
        }
    }
    else // Parent AKA Player 2
    {
        close(p_to_c[0]);
        close(c_to_p[1]);
        int placed;
        read(c_to_p[0], &placed, sizeof(placed)); // player 1 grid is ready

        cout << "Entering data for Player 2: " << endl;
        int ship_size = 2; // 3 ships with length 2, 3, 4
        print_grid(grid);
        while (true)
        {
            cout << "For ship of size: " << ship_size << endl;
            cout << "Enter x: ";
            cin >> x;
            while (x < 1 || x > 5)
            {
                cout << "Invalid input. Enter x: ";
                cin >> x;
            }
            cout << "Enter y: ";
            cin >> y;
            while (y < 1 || y > 5)
            {
                cout << "Invalid input. Enter y: ";
                cin >> y;
            }
            cout << "Enter direction (H or V): ";
            cin >> direction;
            while (direction != 'H' && direction != 'V')
            {
                cout << "Invalid input. Enter direction (H or V): ";
                cin >> direction;
            }

            if (validate_ship_placement(x - 1, y - 1, direction, ship_size, grid))
            {
                if (direction == 'H')
                {
                    for (int i = x - 1; i < x - 1 + ship_size; i++)
                    {
                        grid[y - 1][i] = 'S';
                    }
                }
                else
                {
                    for (int i = y - 1; i < y - 1 + ship_size; i++)
                    {
                        grid[i][x - 1] = 'S';
                    }
                }
                cout << "Ship placed successfully." << endl;
                print_grid(grid);
                if (ship_size == 4)
                {
                    break;
                }
                else
                {
                    ship_size++;
                }
            }
            else
            {
                cout << "Invalid ship placement. Try again." << endl;
            }
        }
        write(p_to_c[1], &placed, sizeof(placed));

        char data_passed;
        while (true)
        {
            read(c_to_p[0], &x, sizeof(x));
            read(c_to_p[0], &y, sizeof(y));
            if (grid[y - 1][x - 1] == 'S')
            {
                grid[y - 1][x - 1] = 'X';
                data_passed = 'H';
            }
            else
            {
                data_passed = 'M';
            }
            write(p_to_c[1], &data_passed, sizeof(data_passed));
            if (check_death(grid))
            {
                data_passed = 'D'; // death
                write(p_to_c[1], &data_passed, sizeof(data_passed));
                wait(NULL);
                exit(0);
            }
            else // death not achieved
            {
                data_passed = 'N'; // not dead
                write(p_to_c[1], &data_passed, sizeof(data_passed));
            }
            read(c_to_p[0], &data_passed, sizeof(data_passed)); // change turn

            cout << "Player 2 turn: " << endl;
            print_both_grids(grid, opponent_grid);
            cout << "Enter x: ";
            cin >> x;
            while (x < 1 || x > 5)
            {
                cout << "Invalid input. Enter x: ";
                cin >> x;
            }
            cout << "Enter y: ";
            cin >> y;
            while (y < 1 || y > 5)
            {
                cout << "Invalid input. Enter y: ";
                cin >> y;
            }
            write(p_to_c[1], &x, sizeof(x));
            write(p_to_c[1], &y, sizeof(y));
            read(c_to_p[0], &data_passed, sizeof(data_passed)); // H or M for hit or miss

            if (data_passed == 'H')
            {
                cout << "Hit!" << endl;
                opponent_grid[y - 1][x - 1] = 'X';
            }
            else
            {
                cout << "Miss!" << endl;
                opponent_grid[y - 1][x - 1] = 'O';
            }
            read(c_to_p[0], &data_passed, sizeof(data_passed)); // D for death
            if (data_passed == 'D')
            {
                cout << "Player 2 wins!" << endl;
                print_both_grids(grid, opponent_grid);
                wait(NULL);
                exit(0);
            }
            data_passed = 'C'; // change turn
            write(p_to_c[1], &data_passed, sizeof(data_passed));
        }
    }
    return 0;
}