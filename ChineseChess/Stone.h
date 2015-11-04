#ifndef STONE_H
#define STONE_H

#include <QRect>
#include <QPainter>

class Stone
{
public:
    Stone();
    ~Stone();

//后续优化3：数据结构的松散，导致程序运行效率的降低；
    enum TYPE{CHE, MA, PAO, BING, JIANG, SHI, XIANG};

    void init(int id);

    //棋子的属性
    int _row;
    int _col;
    TYPE _type;   //棋子的类型——枚举类型
    bool _dead;
    bool _red;
    int _id;

    QString name();

    void rotate();   //旋转棋盘
};

#endif // STONE_H
