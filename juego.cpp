#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

const int ROWS = 6;
const int COLS = 7;

char board[ROWS][COLS];

void initializeBoard()
{
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS; ++j)
        {
            board[i][j] = ' ';
        }
    }
}

void printBoard()
{
    std::cout << " ---------------\n";
    for (int i = 0; i < ROWS; ++i)
    {
        std::cout << i + 1 << " |";
        for (int j = 0; j < COLS; ++j)
        {
            std::cout << board[i][j] << "|";
        }
        std::cout << "\n";
    }
    std::cout << " ---------------\n";
    std::cout << " 1 2 3 4 5 6 7\n";
}

bool isBoardFull()
{
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS; ++j)
        {
            if (board[i][j] == ' ')
            {
                return false;
            }
        }
    }
    return true;
}

bool checkWin(char player)
{
    // Comprobar filas
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS - 3; ++j)
        {
            if (board[i][j] == player && board[i][j + 1] == player && board[i][j + 2] == player && board[i][j + 3] == player)
            {
                return true;
            }
        }
    }
    // Comprobar columnas
    for (int i = 0; i < ROWS - 3; ++i)
    {
        for (int j = 0; j < COLS; ++j)
        {
            if (board[i][j] == player && board[i + 1][j] == player && board[i + 2][j] == player && board[i + 3][j] == player)
            {
                return true;
            }
        }
    }
    // Comprobar diagonales descendentes
    for (int i = 0; i < ROWS - 3; ++i)
    {
        for (int j = 0; j < COLS - 3; ++j)
        {
            if (board[i][j] == player && board[i + 1][j + 1] == player && board[i + 2][j + 2] == player && board[i + 3][j + 3] == player)
            {
                return true;
            }
        }
    }
    // Comprobar diagonales ascendentes
    for (int i = 3; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS - 3; ++j)
        {
            if (board[i][j] == player && board[i - 1][j + 1] == player && board[i - 2][j + 2] == player && board[i - 3][j + 3] == player)
            {
                return true;
            }
        }
    }
    return false;
}

bool dropPiece(int col, char player)
{
    for (int i = ROWS - 1; i >= 0; --i)
    {
        if (board[i][col] == ' ')
        {
            board[i][col] = player;
            return true;
        }
    }
    return false; // La columna está llena
}

int getCPUMove()
{
    // Verificar si la CPU puede ganar en el próximo turno
    for (int j = 0; j < COLS; ++j)
    {
        for (int i = ROWS - 1; i >= 0; --i)
        {
            if (board[i][j] == ' ')
            {
                board[i][j] = 'O'; // Intentar colocar la ficha de la CPU
                if (checkWin('O'))
                {
                    board[i][j] = ' '; // Quitar la ficha de la CPU
                    return j;
                }
                board[i][j] = ' '; // Quitar la ficha de la CPU
                break;
            }
        }
    }
    // Verificar si el jugador puede ganar en el próximo turno y bloquear ese movimiento
    for (int j = 0; j < COLS; ++j)
    {
        for (int i = ROWS - 1; i >= 0; --i)
        {
            if (board[i][j] == ' ')
            {
                board[i][j] = 'X'; // Intentar colocar la ficha del jugador
                if (checkWin('X'))
                {
                    board[i][j] = ' '; // Quitar la ficha del jugador
                    return j;
                }
                board[i][j] = ' '; // Quitar la ficha del jugador
                break;
            }
        }
    }
    // Si no se encontró ninguna jugada ganadora o bloqueadora, elegir una columna al azar
    return rand() % COLS;
}

void startGame()
{
    initializeBoard();
    srand(time(0));

    char currentPlayer = (rand() % 2 == 0) ? 'X' : 'O'; // Inicialización aleatoria del primer jugador
    std::cout << "Bienvenido al juego 4 en linea\n";
    std::cout << "Juegas como '" << currentPlayer << "'\n";

    while (true)
    {
        printBoard();

        int column;
        if (currentPlayer == 'X')
        {
            std::cout << "Es tu turno. Ingresa el numero de columna donde deseas colocar tu ficha (1-7): ";
            std::cin >> column;
            if (std::cin.fail() || column < 1 || column > 7)
            {
                std::cout << "Entrada invalida! Por favor ingresa un numero de columna valido (1-7).\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            column--;
        }
        else
        {
            std::cout << "Turno de la máquina...\n";
            column = getCPUMove();
        }

        if (!dropPiece(column, currentPlayer))
        {
            std::cout << "La columna esta llena! Escoge otra.\n";
            continue;
        }

        if (checkWin(currentPlayer))
        {
            printBoard();
            if (currentPlayer == 'X')
            {
                std::cout << "Felicidades, has ganado!\n";
            }
            else
            {
                std::cout << "La CPU ha ganado!\n";
            }
            break;
        }

        if (isBoardFull())
        {
            printBoard();
            std::cout << "El juego termino en empate!\n";
            break;
        }

        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
}
