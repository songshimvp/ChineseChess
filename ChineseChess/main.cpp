#include <QApplication>
#include "ChooseDlg.h"
#include "MainWnd.h"
#include <QTime>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QTime t = QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);//取时间作为产生随机数的种子

    ChooseDlg dlg;  //选择游戏类型对话框
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    MainWnd wnd(dlg._selected);
    wnd.show();

    return app.exec(); //进入消息循环，等待用户输入
}
