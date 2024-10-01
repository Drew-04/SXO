#ifndef AI_H
#define AI_H


// Структура, описывающая текущее состояние игры
struct GameState {
    int** board;           // Игровое поле 9x9
    bool isComputerCross;   // Ход компьютера крестиком или ноликом
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

#endif // AI_H
