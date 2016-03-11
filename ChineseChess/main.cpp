#include <QApplication>
#include "ChooseDlg.h"
#include "MainWnd.h"
#include <QTime>

//因为是QT的图形界面程序，并且还涉及到网络功能，所以要先在.pro文件中加入QT += widgets gui network
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);     //QApplication抽象了QT的应用程序

    QTime t = QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);  //取时间作为产生随机数的种子

    ChooseDlg dlg;    //选择游戏类型对话框
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    MainWnd wnd(dlg._selected);
    wnd.show();

    return app.exec(); //进入消息循环，等待用户输入
}
