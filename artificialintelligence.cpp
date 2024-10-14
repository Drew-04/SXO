#include "ArtificialIntelligence.h"

#include <ctime>
#include <stack>
#include <QString>
#include <QDebug>


int ArtificialIntelligence::evaluateBoard(GameState currentState) {
    int evaluation = 0;

    // Перебираем каждый блок 3x3 на поле 9x9
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            int computerSymbol = currentState.isAICross ? 1 : -1;
            int playerSymbol = currentState.isAICross ? -1 : 1;

            // Проверяем победу в каждом блоке 3x3
            int blockScore = checkBlockWinner(currentState.board, blockRow, blockCol, computerSymbol, playerSymbol);
            evaluation += blockScore;
        }
    }

    return evaluation;
}


int ArtificialIntelligence::checkBlockWinner(int** result, int blockRow, int blockCol, int computerSymbol, int playerSymbol) {
    // Начальные координаты блока в массиве result
    int startRow = blockRow * 3;
    int startCol = blockCol * 3;

    // Проверяем строки и столбцы блока
    for (int i = 0; i < 3; i++) {
        // Проверка строки
        if (result[startRow + i][startCol] == result[startRow + i][startCol + 1] &&
            result[startRow + i][startCol] == result[startRow + i][startCol + 2]) {
            if (result[startRow + i][startCol] == computerSymbol) return 1; // Победа компьютера
            if (result[startRow + i][startCol] == playerSymbol) return -1; // Победа игрока
        }

        // Проверка столбца
        if (result[startRow][startCol + i] == result[startRow + 1][startCol + i] &&
            result[startRow][startCol + i] == result[startRow + 2][startCol + i]) {
            if (result[startRow][startCol + i] == computerSymbol) return 1; // Победа компьютера
            if (result[startRow][startCol + i] == playerSymbol) return -1; // Победа игрока
        }
    }

    // Проверка диагоналей блока
    if (result[startRow][startCol] == result[startRow + 1][startCol + 1] &&
        result[startRow][startCol] == result[startRow + 2][startCol + 2]) {
        if (result[startRow][startCol] == computerSymbol) return 1; // Победа компьютера
        if (result[startRow][startCol] == playerSymbol) return -1; // Победа игрока
    }

    if (result[startRow + 2][startCol] == result[startRow + 1][startCol + 1] &&
        result[startRow + 2][startCol] == result[startRow][startCol + 2]) {
        if (result[startRow + 2][startCol] == computerSymbol) return 1; // Победа компьютера
        if (result[startRow + 2][startCol] == playerSymbol) return -1; // Победа игрока
    }

    return 0; // Никто не выиграл в этом блоке
}


void ArtificialIntelligence::toggleCellLock(int** board, int lastRow, int lastCol) {
    // Вычисление индексов целевого блока, куда был сделан последний ход
    int blockRow = lastRow % 3;  // Индекс строки внутри целевого блока
    int blockCol = lastCol % 3;  // Индекс столбца внутри целевого блока

    // Проверяем, выигран ли целевой блок
    bool targetBlockWon = checkBlockWinner(board, blockRow, blockCol, 1, -1) != 0;

    // Обход по всем ячейкам игрового поля
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            // Нахождение координат блока для текущей ячейки
            int currentBlockRow = row / 3;
            int currentBlockCol = col / 3;

            // Проверка, выигран ли текущий блок
            bool currentBlockWon = checkBlockWinner(board, currentBlockRow, currentBlockCol, 1, -1) != 0;

            // Если ячейка находится в целевом блоке и этот блок не выигран
            if (currentBlockRow == blockRow && currentBlockCol == blockCol && !targetBlockWon) {
                // Разблокируем свободные клетки в целевом блоке
                if (board[row][col] == 2) {
                    board[row][col] = 0;  // Разблокируем клетку (ставим 0)
                }
            }
            // Если блок не целевой или выигран
            else {
                // Все другие клетки блокируем, если они свободны и блок не выигран
                if (!currentBlockWon && board[row][col] == 0) {
                    board[row][col] = 2;  // Блокируем клетку (ставим 2)
                }
            }
        }
    }

    // Проверка заполненности целевого блока
    bool isTargetBlockFilled = true;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[blockRow * 3 + i][blockCol * 3 + j] == 0) {
                isTargetBlockFilled = false; // Найдена пустая клетка
                break;
            }
        }
        if (!isTargetBlockFilled) break; // Выход из внешнего цикла, если найдена пустая клетка
    }

    // Если целевой блок выигран или заполнен, разблокируем все незанятые клетки, кроме выигранных блоков
    if (targetBlockWon || isTargetBlockFilled) {
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                int currentBlockRow = row / 3;
                int currentBlockCol = col / 3;

                // Если блок не выигран, разблокируем клетки
                if (checkBlockWinner(board, currentBlockRow, currentBlockCol, 1, -1) != 0 && board[row][col] == 2) {
                    board[row][col] = 0;  // Разблокируем клетку
                }
            }
        }
    }
}


std::vector<GameState> ArtificialIntelligence::getAllPossibleStates(GameState currentState) {
    std::vector<GameState> possibleStates;

    // Проходим по всем клеткам 9x9
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (currentState.board[row][col] == 0) {  // Если клетка пуста (доступна для хода)

                // Копируем текущее состояние в новый массив
                int** newBoard = new int*[9];
                for (int i = 0; i < 9; ++i) {
                    newBoard[i] = new int[9];
                    for (int j = 0; j < 9; ++j) {
                        newBoard[i][j] = currentState.board[i][j]; // Копируем текущее состояние доски
                    }
                }

                // Делаем ход в эту клетку (1 - крестик, -1 - нолик)
                newBoard[row][col] = currentState.isAICross ? 1 : -1;

                // Создаем новое состояние
                GameState newState;
                newState.board = newBoard;
                newState.isAICross = currentState.isAICross;  // Указываем, кто ходил

                // Сохраняем координаты последнего хода
                newState.lastMoveRow = row;
                newState.lastMoveCol = col;

                // Обновляем блокировки клеток после хода
                toggleCellLock(newBoard, row, col);

                // Оцениваем новое состояние
                newState.evaluation = evaluateBoard(newState);

                // Добавляем новое состояние в список
                possibleStates.push_back(newState);
            }
        }
    }

    return possibleStates;
}


void ArtificialIntelligence::findBestMove(GameState currentState, int& bestRow, int& bestCol) {
    bestRow = -1;
    bestCol = -1;
    float bestEvaluation = -9999;  // Инициализация самой низкой оценки

    // Стек для итеративного поиска в глубину (LIFO)
    std::stack<TreeNode*> stack;

    // Создаем корневой узел
    TreeNode* root = new TreeNode{currentState, nullptr, 0};
    //bool computerSymbol = currentState.isAICross;

    // Помещаем корневой узел в стек
    stack.push(root);

    // Переменные для отслеживания наилучшего хода
    int firstMoveBestRow = -1;
    int firstMoveBestCol = -1;

    QString bestDebugStr;

    // Поиск в глубину
    while (!stack.empty()) {
        TreeNode* node = stack.top();
        stack.pop();

        // Если глубина узла 3, мы находимся на 3 уровне (после хода компьютера)
        if (node->depth == maxDepth) {
            QString debugStr;
            // Подсчитываем суммарную оценку для текущей ветки
            float totalEvaluation = 0;
            TreeNode* current = node;
            while (current->parent != nullptr) {
                totalEvaluation += evaluateBoard(current->currentState);
                //if (current->currentState.isAICross == computerSymbol)
                //    totalEvaluation += evaluateBoard(current->currentState);
                //else
                //    totalEvaluation -= evaluateBoard(current->currentState);
                //totalEvaluation /= importanceFactor;
                debugStr = "(" + QString::number(current->currentState.lastMoveRow) + " " + QString::number(current->currentState.lastMoveCol) + ") " + debugStr;
                current = current->parent;
            }

            // Проверяем, улучшает ли эта ветка общую оценку
            if (totalEvaluation > bestEvaluation) {
                bestEvaluation = totalEvaluation;
                // Сохраняем координаты первого хода
                TreeNode* rootChild = node;
                while (rootChild->parent && rootChild->parent->depth > 0) {
                    rootChild = rootChild->parent;
                }
                firstMoveBestRow = rootChild->currentState.lastMoveRow;
                firstMoveBestCol = rootChild->currentState.lastMoveCol;
                bestDebugStr = debugStr;
            }
        }
        else {
            // Генерация дочерних узлов для текущего состояния
            std::vector<GameState> possibleMoves = getAllPossibleStates(node->currentState);

            // Для каждого возможного состояния создаем новый узел и добавляем в стек
            for (GameState& move : possibleMoves) {
                //if(node->depth > 1){
                //    move.isAICross = !move.isAICross;
                //}
                TreeNode* childNode = new TreeNode{move, node, node->depth + 1};
                stack.push(childNode);
            }
        }
    }

    // Если наилучшего хода не нашлось и оценка равна 0, делаем случайный ход
    if (bestEvaluation == 0) {
        std::srand(static_cast<unsigned int>(std::time(0)));
        std::vector<GameState> firstMoves = getAllPossibleStates(currentState);
        int randomIndex = std::rand() % firstMoves.size();
        bestRow = firstMoves[randomIndex].lastMoveRow;
        bestCol = firstMoves[randomIndex].lastMoveCol;
    } else {
        // Возвращаем координаты наилучшего первого хода
        bestRow = firstMoveBestRow;
        bestCol = firstMoveBestCol;
        qDebug() << bestDebugStr << bestEvaluation;
    }

    // Освобождаем память, удаляя дерево решений
    while (!stack.empty()) {
        TreeNode* node = stack.top();
        stack.pop();
        delete node;
    }
}
