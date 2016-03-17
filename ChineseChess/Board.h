#ifndef BOARD_H
#define BOARD_H

#include <QFrame>
#include "Stone.h"
#include "Step.h"
#include <QVector>

class Board : public QFrame
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = 0);
    ~Board();

    /* game data */
    Stone _s[32];
    int _r;
    QPoint _off;      //棋盘原点 和 屏幕原点 的之间的偏离
    bool _bSide;

    QVector<Step*> _steps;    //以便于悔棋的操作

    /* game status */
    int _selectid;
    bool _bRedTurn;           //控制谁走
    void init(bool bRedSide); //初始化，摆棋

    /* 绘制draw functions */
    void paintEvent(QPaintEvent *);
    void drawPlate(QPainter& p);   //绘制棋盘
    void drawPlace(QPainter& p);   //绘制九宫
    void drawInitPosition(QPainter& p);
    void drawInitPosition(QPainter& p, int row, int col);   //上下两者结合，绘制“炮”、“兵”的特殊位置标记
    void drawStone(QPainter& p);   //QPainter&：因为这时调用该函数是要把所绘制的棋子和棋盘关联起来；如果是QPainter，则是传值，不好！
    void drawStone(QPainter& p, int id);   //绘制棋子

    /* 坐标转换function for coordinate */
    QPoint center(int row, int col);
    QPoint center(int id);      //行列 转换为 屏幕坐标
    QPoint topLeft(int row, int col);
    QPoint topLeft(int id);     //行列 转换为 中心点的屏幕坐标
    QRect cell(int row, int col);
    QRect cell(int id);         //获得格子

    bool getClickRowCol(QPoint pt, int& row, int& col);  //获得鼠标点击的行和列，也就是通过屏幕像素坐标点获得棋盘的行列

    /* help function */
    QString name(int id);  //根据id获得棋子的名字
    bool red(int id);
    bool sameColor(int id1, int id2);
    int getStoneId(int row, int col);
    void killStone(int id);
    void reliveStone(int id);
    void moveStone(int moveid, int row, int col);//移动位置
    bool isDead(int id);

    /* move stone */
    void mouseReleaseEvent(QMouseEvent *);
    void click(QPoint pt);
    virtual void click(int id, int row, int col);
    void trySelectStone(int id);
    void tryMoveStone(int killid, int row, int col);
    void moveStone(int moveid, int killid, int row, int col); //吃棋子
    void saveStep(int moveid, int killid, int row, int col, QVector<Step*>& steps);//便于悔棋操作
    void backOne();
    void back(Step* step);  //悔棋
    virtual void back();

//后续优化4：各类型棋子的canMove()函数会被调用很多次；
    /* rule 各种类型的棋子的移动规则*/
    bool canMove(int moveid, int killid, int row, int col);
    bool canMoveChe(int moveid, int killid, int row, int col);
    bool canMoveMa(int moveid, int killid, int row, int col);
    bool canMovePao(int moveid, int killid, int row, int col);
    bool canMoveBing(int moveid, int killid, int row, int col);
    bool canMoveJiang(int moveid, int killid, int row, int col);
    bool canMoveShi(int moveid, int killid, int row, int col);
    bool canMoveXiang(int moveid, int killid, int row, int col);

    bool canSelect(int id);   //设定棋子被选择的规则

    /* rule helper */
    int getStoneCountAtLine(int row1, int col1, int row2, int col2);//查看这两个坐标点之间有多少个棋子
    int relation(int row1, int col1, int row, int col);//查看这两个坐标点之间的关系
    bool isBottomSide(int id);

signals:

public slots:
    void slotBack();

};

#endif // BOARD_H
