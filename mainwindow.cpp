#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <cstdlib>
#include <ctime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , crossIcon("D:/Qt_project/SXO/Icon/Cross.png")
    , circleIcon("D:/Qt_project/SXO/Icon/Circle.png")
    , drawIcon("D:/Qt_project/SXO/Icon/Draw.png")
    , nextBlockRow(-1)
    , nextBlockCol(-1)  // Инициализация для следующего блока
    , isAgainstComputer(false)  // Инициализация флага игры с компьютером
    , computerFirst(false)
{
    ui->setupUi(this);

    // Инициализация генератора случайных чисел
    std::srand(std::time(nullptr));

    isCrossTurn = true;

    // Заголовок окна
    setWindowTitle("Супер крестики-нолики");

    // Grid Layout для создания игрового поля (сетки)
    mainGridLayout = new QGridLayout(ui->centralwidget);
    // Выравнивание главного Grid Layout по верхнему и левому краям
    mainGridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Фиксация размера главного окна
    this->setFixedSize(600, 435);

    // Размещение сетки в центральный виджет
    ui->centralwidget->setLayout(mainGridLayout);

    // Создание кнопки индикации текущего игрока
    indicatorButton = new QPushButton(this);  //+
    indicatorButton->setFixedSize(130, 125);  //+
    indicatorButton->setEnabled(false);  //+
    indicatorButton->move(445, 10);  // Установка кнопки по заданным координатам
    updateIndicatorButton(); // Обновление кнопки-индикатора

    // Создание кнопки "Начать заново"
    resetButton = new QPushButton("Начать заново", this);
    resetButton->setFixedSize(130, 40);
    resetButton->move(445, 145); // Установка координат под индикатором
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetGame); // Связывание кнопки с методом resetGame

    // Вызов функции выбора режима игры
    showInitialDialog();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showInitialDialog()
{
    // Создание диалогового окна
    QMessageBox initialBox;
    initialBox.setWindowTitle("Выберите режим игры");
    initialBox.setText("Хотите играть с другом или с компьютером?");

    // Установка кнопок
    QPushButton *twoPlayersButton = initialBox.addButton("2 игрока", QMessageBox::AcceptRole);
    QPushButton *crossButton = initialBox.addButton("Крестик", QMessageBox::AcceptRole);
    QPushButton *circleButton = initialBox.addButton("Нолик", QMessageBox::AcceptRole);

    // Настройка размера диалогового окна
    initialBox.resize(400, 200);
    initialBox.setFixedSize(initialBox.size());

    // Показ диалогового окна
    initialBox.exec();

    QPushButton *clickedButton = qobject_cast<QPushButton*>(initialBox.clickedButton());
    if (clickedButton == twoPlayersButton) {
        isAgainstComputer = false;  // Игра против другого игрока
        createField();
    } else if (clickedButton == crossButton) {
        isAgainstComputer = true;  // Игра против компьютера за крестик
        computerFirst = false;
        isCrossTurn = true;  // Игрок ходит первым
        createField();
    } else if (clickedButton == circleButton) {
        isAgainstComputer = true;  // Игра против компьютера за нолик
        isCrossTurn = true;  // Компьютер ходит первым
        createField();
        computerFirst = true;
        computerClickedButton();  // Компьютер делает первый ход
    }
}

void MainWindow::createField()
{
    // Перебор блоков 3x3
    for (int blockRow = 0; blockRow < 3; ++blockRow)
    {
        for (int blockCol = 0; blockCol < 3; ++blockCol)
        {
            // Создание нового Grid Layout для каждого блока 3x3
            QGridLayout *blockLayout = new QGridLayout;

            // Создание кнопок в каждый блок 3x3
            for (int row = 0; row < 3; ++row)
            {
                for (int col = 0; col < 3; ++col)
                {
                    button = new QPushButton(this);
                    button->setFixedSize(40, 40);
                    blockLayout->addWidget(button, row, col);
                    // Сохранение блока в массив
                    blocks[blockRow][blockCol].buttons[row][col] = button;
                    // Подключение сигнала нажатия к слоту buttonClick()
                    connect(button, &QPushButton::clicked, this, &MainWindow::buttonClick);
                }
            }

            // Инициализация состояния победы
            blocks[blockRow][blockCol].layout = blockLayout;
            blocks[blockRow][blockCol].hasWinner = false;

            // Указание расстояния между кнопками внутри блока 3x3
            blockLayout->setSpacing(5);

            // Добавление блока 3x3 в основной Grid Layout
            mainGridLayout->addLayout(blockLayout, blockRow, blockCol);
        }
    }
    // Указание расстояния между блоками 3x3
    mainGridLayout->setSpacing(15);
}

void MainWindow::buttonClick()
{
    // Кнопка вызвавшая сигнал
    button = qobject_cast<QPushButton *>(sender());

    if (button)
    {
        // Установка иконки в зависимости от текущего хода
        if (isCrossTurn)
        {
            button->setIcon(crossIcon);
            button->setIconSize(QSize(30, 30));
        }
        else
        {
            button->setIcon(circleIcon);
            button->setIconSize(QSize(30, 30));
        }

        // Переход хода
        isCrossTurn = !isCrossTurn;
        updateIndicatorButton();

        // Отключение кнопки
        button->setEnabled(false);

        updateGameState(button);

        // Проверка победы внутри блока
        for (int blockRow = 0; blockRow < 3; ++blockRow)
        {
            for (int blockCol = 0; blockCol < 3; ++blockCol)
            {
                checkForVictory(blocks[blockRow][blockCol]);
            }
        }

        // Обновление состояния игры после проверки победы в блоке
        if (blocks[nextBlockRow][nextBlockCol].hasWinner || areAllButtonsWithIconsOrHasWinner(blocks[nextBlockRow][nextBlockCol]))
        {
            // Разблокировка всех блоков без победителя
            for (int blockRow = 0; blockRow < 3; ++blockRow)
            {
                for (int blockCol = 0; blockCol < 3; ++blockCol)
                {
                    if (!blocks[blockRow][blockCol].hasWinner)
                    {
                        setBlockEnabled(blockRow, blockCol, true);
                    }
                }
            }
        }
        else
        {
            // Блокировка всех блоков
            for (int blockRow = 0; blockRow < 3; ++blockRow)
            {
                for (int blockCol = 0; blockCol < 3; ++blockCol)
                {
                    setBlockEnabled(blockRow, blockCol, false);
                }
            }

            // Разблокировка только нужного блока
            setBlockEnabled(nextBlockRow, nextBlockCol, true);
        }

        // Проверка победы по всей игре
        checkForOverallVictory();
        debugArrayOutput();

        //qDebug()<<isAgainstComputer<<isCrossTurn<<computerFirst;
        // Ход компьютера, если необходимо
        if (isAgainstComputer && !isCrossTurn && !computerFirst)
        {
            computerClickedButton();
        }
        if (isAgainstComputer && isCrossTurn && computerFirst)
        {
            computerClickedButton();
        }
    }
}


int** MainWindow::convertBlocksToArray() {
    // Создание массива 9x9
    int** result = new int*[9];
    for (int i = 0; i < 9; i++) {
        result[i] = new int[9];
        for (int j = 0; j < 9; j++) {
            result[i][j] = 0; // Инициализация с пустыми клетками
        }
    }

    // Преобразование состояния из blocks[3][3] в массив 9x9
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            Block &block = blocks[blockRow][blockCol];
            for (int cellRow = 0; cellRow < 3; cellRow++) {
                for (int cellCol = 0; cellCol < 3; cellCol++) {
                    QPushButton *button = block.buttons[cellRow][cellCol];
                    int globalRow = blockRow * 3 + cellRow;
                    int globalCol = blockCol * 3 + cellCol;

                    if (button->icon().isNull()) {
                        result[globalRow][globalCol] = 0; // Пустая клетка
                    } else if (button->icon().cacheKey() == crossIcon.cacheKey()) {
                        result[globalRow][globalCol] = 1; // Крестик
                    } else if (button->icon().cacheKey() == circleIcon.cacheKey()) {
                        result[globalRow][globalCol] = -1; // Нолик
                    }

                    // Проверка, заблокирована ли клетка
                    if (result[globalRow][globalCol] != 1 && result[globalRow][globalCol] != -1 && (block.hasWinner || button->isEnabled() == false)) {
                        result[globalRow][globalCol] = 2; // Заблокированная клетка
                    }
                }
            }
        }
    }
    return result;
}


void MainWindow::debugArrayOutput() {
    // Вызов вашей функции преобразования.
    currentState.board = convertBlocksToArray();
    currentState.isComputerCross = true;
    currentState.evaluation = evaluateBoard(currentState.board, currentState.isComputerCross);
    qDebug() << "Оценка текущего состояния игрового поля:" << currentState.evaluation;

    // Функция для печати массива.
    auto printArray = [](int** array, int rows, int cols) {
        for (int i = 0; i < rows; ++i) {
            QString row;
            for (int j = 0; j < cols; ++j) {
                row += QString::number(array[i][j]) + " ";
            }
            qDebug() << row.trimmed(); // Выводим каждую строку массива.
        }
    };

    // Печать массива 9x9.
    printArray(currentState.board, 9, 9);
}


int MainWindow::evaluateBoard(int** result, bool isComputerCross) {
    int evaluation = 0;

    // Перебираем каждый блок 3x3 на поле 9x9
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            int computerSymbol = isComputerCross ? 1 : -1;
            int playerSymbol = isComputerCross ? -1 : 1;

            // Проверяем победу в каждом блоке 3x3
            int blockScore = checkBlockWinner(result, blockRow, blockCol, computerSymbol, playerSymbol);
            evaluation += blockScore;
        }
    }

    return evaluation;
}


// Функция для проверки победителя в блоке 3x3
int MainWindow::checkBlockWinner(int** result, int blockRow, int blockCol, int computerSymbol, int playerSymbol) {
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


void MainWindow::toggleCellLock(int** board, int lastRow, int lastCol){
    // Поиск блока и ячейки внутри блока, в которые был сделан последний ход
    int blockRow = lastRow % 3; // Координата строки блока
    int blockCol = lastCol % 3; // Координата столбца блока

    // Обход по всем ячейкам игрового поля
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            // Нахождение координат блока для текущей ячейки
            int currentBlockRow = row / 3;
            int currentBlockCol = col / 3;

            // Если текущая ячейка находится не в нужном блоке и не занята, заблокировать её
            if (currentBlockRow == blockRow && currentBlockCol == blockCol) {
                if (board[row][col] == 0) {
                    board[row][col] = 2; // Блокировка клетки (ставим 2 вместо 0)
                } else if (board[row][col] == 2) {
                    board[row][col] = 0; // Разблокировка клетки (ставим 0 вместо 2)
                }
            }
            // Если блок уже выигран или ничья, разблокировать все клетки
            else if (board[row][col] == 2) {
                board[row][col] = 0; // Разблокировка клетки (ставим 0 вместо 2)
            }
        }
    }
}





void MainWindow::computerClickedButton()
{
    // Генерация случайного хода для компьютера
    bool moveMade = false;

    while (!moveMade)
    {
        int blockRow = std::rand() % 3;
        int blockCol = std::rand() % 3;
        int row = std::rand() % 3;
        int col = std::rand() % 3;

        if (!blocks[blockRow][blockCol].hasWinner && blocks[blockRow][blockCol].buttons[row][col]->isEnabled())
        {
            QPushButton *button = blocks[blockRow][blockCol].buttons[row][col];
            button->click();  // Компьютер делает ход
            moveMade = true;
        }
    }
}


void MainWindow::setBlockEnabled(int blockRow, int blockCol, bool enabled)
{
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            QPushButton *currentButton = blocks[blockRow][blockCol].buttons[row][col];
            if (currentButton->icon().isNull())  // Если иконка отсутствует
            {
                currentButton->setEnabled(enabled);
            }
            else  // Если иконка присутствует
            {
                currentButton->setEnabled(false);
            }
        }
    }
}


void MainWindow::updateGameState(QPushButton *button)
{
    int row = -1, col = -1;
    bool found = false;

    // Поиск нажатой кнопки
    for (int br = 0; br < 3 && !found; ++br)
    {
        for (int bc = 0; bc < 3 && !found; ++bc)
        {
            for (int r = 0; r < 3 && !found; ++r)
            {
                for (int c = 0; c < 3 && !found; ++c)
                {
                    if (blocks[br][bc].buttons[r][c] == button)
                    {
                        row = r;
                        col = c;
                        found = true;
                    }
                }
            }
        }
    }

    nextBlockRow = row;
    nextBlockCol = col;

    // Если текущий блок уже имеет победителя, все блоки должны быть разблокированы
    bool isNextBlockFull = areAllButtonsWithIconsOrHasWinner(blocks[nextBlockRow][nextBlockCol]);

    if (blocks[nextBlockRow][nextBlockCol].hasWinner || isNextBlockFull)
    {
        // Разблокировка всех блоков без победителя
        for (int br = 0; br < 3; ++br)
        {
            for (int bc = 0; bc < 3; ++bc)
            {
                if (!blocks[br][bc].hasWinner)
                {
                    setBlockEnabled(br, bc, true);
                }
            }
        }
    }
    else
    {
        // Блокировка всех блоков
        for (int br = 0; br < 3; ++br)
        {
            for (int bc = 0; bc < 3; ++bc)
            {
                setBlockEnabled(br, bc, false);
            }
        }

        // Разблокировка соответствующего блока
        setBlockEnabled(nextBlockRow, nextBlockCol, true);
    }
}


void MainWindow::checkForVictory(Block &block)
{
    auto checkLine = [](QPushButton *btn1, QPushButton *btn2, QPushButton *btn3) {
        return btn1->icon().cacheKey() == btn2->icon().cacheKey() &&
               btn2->icon().cacheKey() == btn3->icon().cacheKey() &&
               !btn1->icon().isNull();
    };

    if (block.winnerButton != nullptr) {
        return;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (checkLine(block.buttons[i][0], block.buttons[i][1], block.buttons[i][2]))
        {
            createWinnerButton(block, block.buttons[i][0]->icon());
            block.hasWinner = true;
            return;
        }

        if (checkLine(block.buttons[0][i], block.buttons[1][i], block.buttons[2][i]))
        {
            createWinnerButton(block, block.buttons[0][i]->icon());
            block.hasWinner = true;
            return;
        }
    }

    if (checkLine(block.buttons[0][0], block.buttons[1][1], block.buttons[2][2]))
    {
        createWinnerButton(block, block.buttons[0][0]->icon());
        block.hasWinner = true;
        return;
    }

    if (checkLine(block.buttons[0][2], block.buttons[1][1], block.buttons[2][0]))
    {
        createWinnerButton(block, block.buttons[0][2]->icon());
        block.hasWinner = true;
        return;
    }
}


void MainWindow::createWinnerButton(Block &block, const QIcon &winnerIcon)
{
    if (block.winnerButton != nullptr) {
        return;
    }

    // Создание большой кнопки, представляющей победителя блока
    QPushButton *winnerButton = new QPushButton(this);
    winnerButton->setIcon(winnerIcon);
    winnerButton->setFixedSize(130, 125); // Размер, чтобы заполнить весь блок
    winnerButton->setIconSize(QSize(120, 115));

    //Отключение большой кнопки
    winnerButton->setEnabled(false);

    //Определение цвета фона на основе иконки
    if (winnerIcon.cacheKey() == crossIcon.cacheKey())
    {
        winnerButton->setStyleSheet("background-color: rgba(255, 255, 0, 0.5);");
    }
    else if (winnerIcon.cacheKey() == circleIcon.cacheKey())
    {
        winnerButton->setStyleSheet("background-color: rgba(0, 255, 0, 0.5);");
    }

    //Добавление большой кнопки поверх блока
    block.layout->addWidget(winnerButton, 0, 0, 3, 3); // Занимает все место блока

    block.winnerButton = winnerButton;

    //Отключение всех кнопок в блоке
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            QPushButton *currentButton = block.buttons[row][col];
            currentButton->setEnabled(false);
        }
    }
}


bool MainWindow::areAllButtonsWithIconsOrHasWinner(const Block &block)
{
    if (block.hasWinner) {
        return true; // Если в блоке уже есть победитель, считаем его полным
    }

    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            if (block.buttons[row][col]->icon().isNull()) // Если иконка отсутствует, значит кнопка не была нажата
            {
                return false;
            }
        }
    }
    return true; // Все кнопки имеют иконку
}


void MainWindow::checkForOverallVictory()
{
    auto checkLine = [](QPushButton *btn1, QPushButton *btn2, QPushButton *btn3) {
        // Проверяем, что все кнопки не равны nullptr и имеют одинаковую иконку
        return btn1 && btn2 && btn3 &&
               btn1->icon().cacheKey() == btn2->icon().cacheKey() &&
               btn2->icon().cacheKey() == btn3->icon().cacheKey() &&
               !btn1->icon().isNull();
    };

    // Проверка всех горизонтальных и вертикальных линий
    for (int i = 0; i < 3; ++i)
    {
        if (checkLine(blocks[i][0].winnerButton, blocks[i][1].winnerButton, blocks[i][2].winnerButton))
        {
            showVictoryMessage(blocks[i][0].winnerButton->icon());
            return;
        }

        if (checkLine(blocks[0][i].winnerButton, blocks[1][i].winnerButton, blocks[2][i].winnerButton))
        {
            showVictoryMessage(blocks[0][i].winnerButton->icon());
            return;
        }
    }

    // Проверка диагоналей
    if (checkLine(blocks[0][0].winnerButton, blocks[1][1].winnerButton, blocks[2][2].winnerButton))
    {
        showVictoryMessage(blocks[0][0].winnerButton->icon());
        return;
    }

    if (checkLine(blocks[0][2].winnerButton, blocks[1][1].winnerButton, blocks[2][0].winnerButton))
    {
        showVictoryMessage(blocks[0][2].winnerButton->icon());
        return;
    }

    // Проверка на ничью
    bool allBlocksFull = true;
    for (int blockRow = 0; blockRow < 3; ++blockRow)
    {
        for (int blockCol = 0; blockCol < 3; ++blockCol)
        {
            if (!areAllButtonsWithIconsOrHasWinner(blocks[blockRow][blockCol]))
            {
                allBlocksFull = false;
                break;
            }
        }
        if (!allBlocksFull) break;
    }

    if (allBlocksFull)
    {
        QMessageBox victoryMessage;
        victoryMessage.setWindowTitle("Игра окончена");
        victoryMessage.setText("Ничья");

        // Установка иконки ничьи
        victoryMessage.setIconPixmap(drawIcon.pixmap(64, 64));

        // Отображение сообщения о ничье
        victoryMessage.exec();

        resetGame();
    }
}


void MainWindow::showVictoryMessage(const QIcon &winnerIcon)
{
    QString winnerText;
    if (winnerIcon.cacheKey() == crossIcon.cacheKey())
    {
        winnerText = "Победил игрок X";
    }
    else if (winnerIcon.cacheKey() == circleIcon.cacheKey())
    {
        winnerText = "Победил игрок O";
    }

    QMessageBox victoryMessage;
    victoryMessage.setWindowTitle("Игра окончена");
    victoryMessage.setText(winnerText);

    // Установка иконки победителя
    victoryMessage.setIconPixmap(winnerIcon.pixmap(64, 64));

    // Отображение сообщения о победе
    victoryMessage.exec();

    resetGame();
}


void MainWindow::resetGame()
{
    // Удаление всех больших кнопок победителей и их очистка
    for (int blockRow = 0; blockRow < 3; ++blockRow)
    {
        for (int blockCol = 0; blockCol < 3; ++blockCol)
        {
            Block &block = blocks[blockRow][blockCol];

            // Удаление всех кнопок победителей
            if (block.winnerButton != nullptr)
            {
                delete block.winnerButton;
                block.winnerButton = nullptr;
            }

            // Удаление всех кнопок внутри блока
            while (block.layout->count() > 0)
            {
                QLayoutItem *item = block.layout->takeAt(0);
                if (item->widget())
                {
                    delete item->widget();
                }
                delete item;
            }

            // Сброс состояния блоков
            block.hasWinner = false;

            // Создание нового блока
            QGridLayout *blockLayout = new QGridLayout;
            block.layout = blockLayout;
            blockLayout->setSpacing(5);
            mainGridLayout->addLayout(blockLayout, blockRow, blockCol);

            // Создание новых кнопок в блоке
            for (int row = 0; row < 3; ++row)
            {
                for (int col = 0; col < 3; ++col)
                {
                    button = new QPushButton(this);
                    button->setFixedSize(40, 40);
                    blockLayout->addWidget(button, row, col);
                    block.buttons[row][col] = button;
                    connect(button, &QPushButton::clicked, this, &MainWindow::buttonClick);
                }
            }
        }
    }

    // Сброс состояния игры
    isCrossTurn = true;
    // isCrossTurn = std::rand() % 2 == 0; // Случайный выбор начального символа
    nextBlockRow = -1;
    nextBlockCol = -1;

    // Разблокировка всех блоков в начале игры
    for (int blockRow = 0; blockRow < 3; ++blockRow)
    {
        for (int blockCol = 0; blockCol < 3; ++blockCol)
        {
            setBlockEnabled(blockRow, blockCol, true);
        }
    }
    updateIndicatorButton();
    showInitialDialog();
}


void MainWindow::updateIndicatorButton() {
    if (isCrossTurn) {
        indicatorButton->setIcon(crossIcon);
        indicatorButton->setIconSize(QSize(120, 115));
        indicatorButton->setStyleSheet("background-color: yellow;");
    } else {
        indicatorButton->setIcon(circleIcon);
        indicatorButton->setIconSize(QSize(120, 115));
        indicatorButton->setStyleSheet("background-color: lime;");
    }
}


