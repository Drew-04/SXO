#ifndef ARTIFICIALINTELLIGENCE_H
#define ARTIFICIALINTELLIGENCE_H

#include <vector>


// Структура, описывающая текущее состояние игры
struct GameState {
    int** board;           // Игровое поле 9x9
    bool isAICross;   // Ход компьютера крестиком или ноликом
    int evaluation;         // Оценка текущего состояния
    int lastMoveRow;
    int lastMoveCol;
};

// Структура, описывающая узел дерева для поиска
struct TreeNode {
    GameState currentState; // Текущее состояние игрового поля
    TreeNode* parent;       // Указатель на предка
    int depth;              // Глубина текущего узла (0 для корня)
};


class ArtificialIntelligence {
public:
    TreeNode currentTreeNode;
    int maxDepth = 3;
    float importanceFactor = 1.5;

    int evaluateBoard(GameState currentState);
    int checkBlockWinner(int** result, int blockRow, int blockCol, int computerSymbol, int playerSymbol);
    void toggleCellLock(int** board, int lastRow, int lastCol);
    std::vector<GameState> getAllPossibleStates(GameState currentState);
    void findBestMove(GameState currentState, int& bestRow, int& bestCol);
};

#endif // ARTIFICIALINTELLIGENCE_H
