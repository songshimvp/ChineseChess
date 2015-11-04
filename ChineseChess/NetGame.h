#ifndef NETGAME_H
#define NETGAME_H

#include "Board.h"
#include <QTcpServer>     //网络相关头文件
#include <QTcpSocket>     //网络相关头文件

/*
 * 报文格式定义
 * 1.初始化    两个字节
 * 2.鼠标点击   四个字节
 * 3.悔棋      一个字节
*/

/*
#include <QMessageBox>
QMessageBox::StandardButton ret;
ret = QMessageBox::question(NULL,"server or client","作为服务器启动")；
if(ret == QMessageBox::Yes)
{

}
*/

class NetGame : public Board
{
    Q_OBJECT
public:
    explicit NetGame(bool server, QWidget *parent = 0);
    ~NetGame();

    //该程序的服务器端和客户端对等，也就是说没有专门的服务器
    bool _bServer;
    QTcpServer* _server;
    QTcpSocket* _socket;

    void click(int id, int row, int col); //重载了click()函数,click()函数会在Board中的mouseReleaseEvent()中调用

    void initFromNetwork(QByteArray buf);
    void clickFromNetwork(QByteArray buf);

    void backFromNetwork(QByteArray buf);
    void back();

signals:

public slots:
    void slotNewConnection();
    void slotDataArrive();
};

#endif // NETGAME_H
