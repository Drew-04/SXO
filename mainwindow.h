#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <vector>
#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow: public QMainWindow
{
    Q_OBJECT

    public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private:
    Ui::MainWindow *ui;
    QGridLayout *mainGridLayout;
    QPushButton *button;
    QPushButton *indicatorButton; // Для индикации игрока
    QPushButton *resetButton; // Кнопка "Начать заново"
    bool isCrossTurn; //true - крестик, false - нолик
    QIcon crossIcon;
    QIcon circleIcon;
    QIcon drawIcon;
    std::vector<QGridLayout*> blockLayouts;
    std::vector<std::vector<QPushButton*>> buttons;
    int nextBlockRow;
    int nextBlockCol;
    int numberOfBlocks = 3;
    bool isAgainstComputer;
    bool computerFirst;

    struct Block {
        QGridLayout *layout;
        QPushButton *buttons[3][3];
        bool hasWinner;
        QPushButton *winnerButton = nullptr;
    };

    Block blocks[3][3];

    int** convertBlocksToArray();
    void debugArrayOutput();
    int evaluateBoard(int** result, bool isComputerCross);
    int checkBlockWinner(int** result, int blockRow, int blockCol, int computerSymbol, int playerSymbol);
    void toggleCellLock(int** board, int lastRow, int lastCol);

    struct GameState {
        int** board;
        bool isComputerCross;
        int evaluation;
    };

    GameState currentState;

    struct TreeNode {
        //GameState currentState; // Текущее состояние игрового поля
        TreeNode* parent; // Указатель на предка
        int depth; // Глубина текущего узла
        int moveRow; // Координата строки для хода, ведущего к этому состоянию
        int moveCol; // Координата столбца для хода, ведущего к этому состоянию
    };

    std::vector<TreeNode*> generateChildren(TreeNode* node, bool isComputerTurn);

    void createField();
    void checkForVictory(Block &block);
    void setBlockEnabled(int blockRow, int blockCol, bool enabled);
    void updateGameState(QPushButton *button);
    void createWinnerButton(Block &block, const QIcon &winnerIcon);
    bool areAllButtonsWithIconsOrHasWinner(const Block &block);
    void checkForOverallVictory();
    void showVictoryMessage(const QIcon &winnerIcon);
    void resetGame();
    void updateIndicatorButton();
    void showInitialDialog();
    void computerClickedButton();

    private slots:
    void buttonClick();
};

#endif // MAINWINDOW_H
