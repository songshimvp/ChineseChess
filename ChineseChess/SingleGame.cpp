#include "SingleGame.h"
#include <QTimer>
#include <QDebug>

SingleGame::SingleGame(QWidget *parent) : Board(parent)
{
    _level = 4;       //最大最小算法考虑的层数
}

SingleGame::~SingleGame()
{

}

/*
人机对战：
1、看看有哪些步骤可以走；
2、试着走一下；
3、评估走的结果；
4、取最好的结果作为参考；
 */

void SingleGame::click(int id, int row, int col)   //在此处是一个虚函数
{
    if(_bRedTurn)  //只有红方（用户，默认设定）动了以后，电脑才走
    {
        Board::click(id, row, col);   //用户还是照常走

        //用户移动完0.1秒以后，电脑再思考
        if(!_bRedTurn)
        {            
            //因为电脑思考还是很慢的（很多步骤！），就会把主进程阻塞！阻塞以后，界面就没法得到响应！
            //启动定时器，在0.01秒后电脑再思考。就是这0.01秒，就可以完成界面的刷新！
            //可以try一下直接computerMove()是什么效果。
            QTimer::singleShot(10, this, SLOT(computerMove()));   //槽函数
        }
    }
}

void SingleGame::computerMove()
{
    /*
     * 1、看看有哪些步骤可以走；
     * 2、试着走一下；
     * 3、评估走的结果；
     * 4、取最好的结果作为参考；
    */
    Step* step = getBestMove();
    moveStone(step->_moveid, step->_killid, step->_rowTo, step->_colTo);
    delete step;

    update();
}


//后续优化1：给所有的可能的走法排个序
void SingleGame::getAllPossibleMove(QVector<Step *> &steps)
{
    //设定min和max以后，既可以得到红棋的可能步骤，也能得到黑棋的可能步骤
    //为什么要获取红方（用户）的所有可能步骤？用处是：在下面最小值最大值算法中电脑需要判定用户的局面分
    int min, max;
    if(this->_bRedTurn)  //用户方
    {
        min = 0, max = 16;
    }
    else   //电脑方
    {
        min = 16, max = 32;
    }

    //遍历所有的棋子，找到那些可能走的棋子
    for(int i=min;i<max; i++)
    {
        if(this->_s[i]._dead)
            continue;//抠掉死棋，不然已经被吃掉的棋子还会继续发挥作用

        for(int row = 0; row<=9; ++row)
        {
            for(int col=0; col<=8; ++col)
            {
                int killid = this->getStoneId(row, col);
                if(sameColor(i, killid))
                    continue;

                if(canMove(i, killid, row, col))
                {
                    saveStep(i, killid, row, col, steps);
                }
            }
        }
    }
}

//getBestMove----->getMinScore------>_level-1------>getMaxScore
Step* SingleGame::getBestMove()
{
    Step* ret = NULL;
    //1、看看有哪些步骤可以走；
    QVector<Step*> steps;
    getAllPossibleMove(steps);

    int maxInAllMinScore = -300000;

    //2、试着走一下；
    //3、评估走的结果；
    while(steps.count())    //遍历所有的步骤
    {
        Step* step = steps.last();
        steps.removeLast();

        //fakeMove(step);和unfakeMove(step);————这两个动作其实和人类似，
        //人在走棋时，也会拿起棋试着放到想走的地方看一看，但是并不落子。
        fakeMove(step);
        int minScore = getMinScore(this->_level-1, maxInAllMinScore);
        unfakeMove(step);

        if(minScore > maxInAllMinScore)
        {
            if(ret) delete ret;

            ret = step;
            maxInAllMinScore = minScore;
        }
        else
        {
            delete step;   //释放那些可能步骤，避免内存泄露
        }
    }
    //4、取最好的结果作为参考；
    return ret;
}


//后续优化2：此处使用的是静态局面分，另外此处的局面分是按照自认为的棋子类型的重要性进行划分的
//试着走一步以后，就要评价局面分：——这一步是最复杂的
//计算局面分————黑棋（电脑）作为主角
int SingleGame::calcScore()
{
    //enum TYPE{CHE, MA, PAO, BING, JIANG, SHI, XIANG};
    //此处简单的进行给定评价分——按照棋子类型的重要性
    int chessScore[] = {1000, 499, 501, 200, 15000, 100, 100};
    int scoreBlack = 0;
    int scoreRed = 0;

    for(int i=0; i<16; ++i)  //黑棋（电脑）总分
    {
        if(_s[i]._dead)
            continue;
        scoreRed += chessScore[_s[i]._type];
    }
    for(int i=16; i<32; ++i) //红棋（人）的总分
    {
        if(_s[i]._dead)
            continue;
        scoreBlack += chessScore[_s[i]._type];
    }
    //根据 （黑棋分的总数 - 红棋分的总数） 进行此时局面分的评定
    return scoreBlack - scoreRed;
}

/*
最小值最大值算法：
电脑假走一步，人再假走一步，电脑通过计算人假走一步以后得到的最低分（getMinScore），
然后在这些最低分中取一个最高分（getMaxScore），作为最后此时的局面分。
因为很显然，电脑要走的最好那步是 让用户 局面分 最低的那步。

也就是说，电脑在和人博弈时，电脑想想走局面分最高的那一步，但是走完这一步以后有可能会让
用户有更高局面分的选择，所以就要把 下下步用户最低局面分 作为 自己的下步最高局面分。


详解：
                                     level（1）：第一局面
                                                                            --->取 “最大值”
             level（2）：第二局面（1）            第二局面（2）    第二局面（3）
                                                                            --->取 “最小值”
level（3）：第二局面（1—1，10分）、第二局面（1—2，100分）     第二局面（3—1，20分）第二局面（3—1，30分）

电脑走，假设会出现以下三个局面，在第二局面中，局面分是由分支传上来的，电脑需要在这些局面分中取 “最大值”
上面电脑走完以后，人走，可能出现以下局面，这就需要在所有子树中求取 “最小值”，作为上层父节点的局面分
  比如：你发现了一个小偷偷东西（假设偷了三个包裹），小偷和你达成一个协议（你和小偷博弈）让你放他走，协议是：这三个包裹的东西你都可以看见，
一号包：1元、100元，二号包：20元、30元，三号包：5元，60元，然后你选一个包，但是由小偷给你从包里拿东西。很明显，你会选择二号包，
也很明显，小偷会选择给你那个20元！   也就是说，在象棋博弈时，假设人也很聪明，当电脑走完以后，人会走让电脑局面分低的那步。
比如上面图解情况：
在（1）的子树下1-1和1-2是10分、100分，取10分作为父节点（1）的分数；
在（3）的子树下3-1和3-2是20分、 30分，取20分作为父节点（3）的分数；
然后在（1）和（3）中取最大值20分，所以电脑走的是（3）这个分支！也就是说，人你再聪明，电脑还是的20分。很简单，如果电脑走（1），那么，人就会
走给它10分的那条路，显然，这不是getBestMove！
*/
/*
剪枝优化：将同级的前一分支的结果（curMin、curMax）传到下一分支，以判断是否还需要继续计算比较！
比如上例：计算完level（2）的（1）以后得到的结果是10分，所以在计算level（2）的（3）的第一分支时得到20，以后就不再计算，直接删掉此路。
 */
int SingleGame::getMinScore(int level, int curMin)
{
    if(level == 0)
        return calcScore();    //间接递归的结束条件，并计算一个局面分

    QVector<Step*> steps;
    getAllPossibleMove(steps);   //红棋的getAllPossibleMove

    int minInAllMaxScore = 300000;

    while(steps.count())
    {
        Step* step = steps.last();
        steps.removeLast();

        fakeMove(step);
        //传入minInAllMaxScore，进行剪枝
        int maxScore = getMaxScore(level-1, minInAllMaxScore);//getMinScore调用getMaxScoreget——间接递归
        unfakeMove(step);
        delete step;

        //剪枝优化
        if(maxScore <= curMin)    //添加 = 以后的提升程序的效率！原因：在象棋博弈的时候，很少有吃棋的动作，基本上都是走棋！所以局面分相等的情况很多！
        {
            while(steps.count())  //避免内存泄露
            {
                Step* step = steps.last();
                steps.removeLast();
                delete step;
            }

            return maxScore;      //当在下层后面分支（分支的分支）出现更小的值时（必然不可能走），直接return！后面的分支不用再计算，提高速度！
        }

        if(maxScore < minInAllMaxScore)
        {
            minInAllMaxScore = maxScore;
        }

    }
    return minInAllMaxScore;
}
int SingleGame::getMaxScore(int level, int curMax)  //level控制间接递归的层数
{
    if(level == 0)
        return calcScore();   //间接递归的结束条件

    QVector<Step*> steps;
    getAllPossibleMove(steps);

    int maxInAllMinScore = -300000;

    while(steps.count())
    {
        Step* step = steps.last();
        steps.removeLast();

        fakeMove(step);
        //传入minInAllMaxScore，进行剪枝
        int minScore = getMinScore(level-1, maxInAllMinScore);//getMaxScoreget调用getMinScore
        unfakeMove(step);
        delete step;

        //剪枝优化
        if(minScore >= curMax)   //添加 = 以后的提升程序的效率!原因：在象棋博弈的时候，很少有吃棋的动作，基本上都是走棋！所以局面分相等的情况很多！
        {
            while(steps.count()) //避免内存泄露
            {
                Step* step = steps.last();
                steps.removeLast();
                delete step;
            }

            return minScore;     //当后面分支出现较大值时（必然不可能走），直接return！后面的分支不用再计算，提高速度！
        }

        if(minScore > maxInAllMinScore)
        {
            maxInAllMinScore = minScore;
        }
    }
    return maxInAllMinScore;
}

//虚假移动，以便计算局面分
void SingleGame::fakeMove(Step *step)
{
    killStone(step->_killid);   //杀死
    moveStone(step->_moveid, step->_rowTo, step->_colTo);
}

void SingleGame::unfakeMove(Step *step)
{
    reliveStone(step->_killid);  //复活
    moveStone(step->_moveid, step->_rowFrom, step->_colFrom);
}

void SingleGame::back()
{
    if(_bRedTurn)
    {
        backOne();
        backOne();
    }
}
