#ifndef STEP_H
#define STEP_H

#include <QObject>

//用于人工智能的最优走法  和  悔棋操作
class Step
{
    //Q_OBJECT
public:
    explicit Step();
    ~Step();

    //从哪里来到哪里去
    int _moveid;

    int _rowFrom;
    int _colFrom;
    int _rowTo;
    int _colTo;

    int _killid;

signals:

public slots:
};

#endif // STEP_H
