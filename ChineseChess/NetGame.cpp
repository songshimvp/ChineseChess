#include "NetGame.h"

NetGame::NetGame(bool server, QWidget *parent) : Board(parent)
{
    _server = NULL;
    _socket = NULL;
    _bServer = server;   //判断是否作为服务器端启动

    if(_bServer)   //服务端
    {
        _server = new QTcpServer(this);
        _server->listen(QHostAddress::Any, 9899);//监听
        connect(_server, SIGNAL(newConnection()),this, SLOT(slotNewConnection()));
    }
    else   //客户端
    {
        _socket = new QTcpSocket(this);
        _socket->connectToHost(QHostAddress("127.0.0.1"), 9899);   //连接到服务器
        connect(_socket, SIGNAL(readyRead()),this, SLOT(slotDataArrive()));
    }
}

void NetGame::slotNewConnection()
{
    if(_socket) return;//即使有新的连接请求（第三方），也不接受！

    _socket = _server->nextPendingConnection();
    connect(_socket, SIGNAL(readyRead()), this, SLOT(slotDataArrive()));    //无论是服务端给客户端，还是客户端给服务端，数据都能接收到

    /* 产生随机数来决定谁走红色 */
    bool bRedSide = qrand()%2>0;
    init(bRedSide);

    /* 服务器 把选择的“先手信息”发送给 客户端 */
    //发送三个信息：（1）先手信息；（2）点击信息；（3）悔棋信息
    //“先手信息”：————两个字节
    //第一个字节固定是1；
    //第二个字节是0或者1，1表示接收方走红棋，0表示走黑棋
    QByteArray buf;
    buf.append(1);
    buf.append(bRedSide ? 0 : 1);

    _socket->write(buf);    //服务器端socket发送
}

void NetGame::click(int id, int row, int col)  //重载了click()函数
{
    if(_bRedTurn != _bSide) //红方只能走红棋，黑方只能走黑棋
        return;

    Board::click(id, row, col);   //首先调用父类Board的click，让棋子动起来

    /* 然后把“点击的事件”发送给对方 */
    //发送三个信息：（1）先手信息；（2）点击信息；（3）悔棋信息
    //“点击信息”————四个字节
    //第一个字节固定是2，第二字解是点击的棋子id，第三个字节是row，第四个字节是col
    QByteArray buf;
    buf.append(2);
    buf.append(id);
    buf.append(row);
    buf.append(col);

    _socket->write(buf);  //服务器端socket发送
}

void NetGame::slotDataArrive()
{
    QByteArray buf = _socket->readAll();
    switch (buf.at(0))
    {
        case 1:  //首位是1，说明是“先手信息”
            initFromNetwork(buf);
            break;
        case 2:  //首位是2，说明是“点击信息”
            clickFromNetwork(buf);
            break;
        case 3:  //首位是3，说明是“悔棋信息”
            backFromNetwork(buf);
            break;
        default:
            break;
    }
}

void NetGame::initFromNetwork(QByteArray buf)
{
    bool bRedSide = buf.at(1) > 0 ? true : false;   //先初始化“先手信息”
    init(bRedSide);  //利用上面的“先手信息” 初始化棋盘：Board::init(bool bRedSide)
}

void NetGame::clickFromNetwork(QByteArray buf)
{
    //buf[1]存的是点击的id，二这个id是NetGame::click(int id, int row, int col)中来的，其实是从Board::click(QPoint pt)中的getStoneId(row, col)中来的
    Board::click(buf[1], 9-buf[2], 8-buf[3]);  //因为有棋盘的旋转，甲方点了一个棋子，在对方的棋盘上显示的坐标并不是完全一样的，是需要转化一下的
}

void NetGame::backFromNetwork(QByteArray)
{
    backOne();
    backOne();
}
void NetGame::back()
{
    if(_bRedTurn != _bSide)
        return;
    backOne();
    backOne();

    //发送三个信息：（1）先手信息；（2）点击信息；（3）悔棋信息
    //发送“悔棋信息”
    QByteArray buf;
    buf.append(3);
    _socket->write(buf);
}

NetGame::~NetGame()
{

}

