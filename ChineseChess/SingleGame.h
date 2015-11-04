#ifndef SINGLEGAME_H
#define SINGLEGAME_H

#include "Board.h"

class SingleGame : public Board
{
    Q_OBJECT
public:
    explicit SingleGame(QWidget *parent = 0);
    ~SingleGame();

    int _level;

    void click(int id, int row, int col);   //在此虚函数（从Board继承而来）中实现，用户走，电脑随之走

    void getAllPossibleMove(QVector<Step*>& steps);
    int getMinScore(int level, int curMin);
    int getMaxScore(int level, int curMax);
    int calcScore();
    Step* getBestMove();

    void fakeMove(Step* step);
    void unfakeMove(Step* step);

    void back();
signals:

public slots:
    void computerMove();
};

#endif // SINGLEGAME_H
