#include "ChooseDlg.h"
#include <QVBoxLayout>

ChooseDlg::ChooseDlg(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->addWidget(_buttons[0] = new QPushButton("人机对战"));
    lay->addWidget(_buttons[1] = new QPushButton("人人对战"));
    lay->addWidget(_buttons[2] = new QPushButton("网络对战(服务端)"));
    lay->addWidget(_buttons[3] = new QPushButton("网络对战(客户端)"));

    for(int i=0; i<4; ++i)
    {
        connect(_buttons[i], SIGNAL(clicked()), this, SLOT(slotClicked()));
    }
}

void ChooseDlg::slotClicked()
{
    QObject* s = sender();
    for(int i=0; i<4; ++i)
    {
        if(_buttons[i] == s)
        {
            this->_selected = i;   //在槽函数中，根据按钮完成对_selectid的赋值，在main函数中传递给MainWnd，以便启动相应的游戏类型
            break;
        }
    }
    accept();
}

ChooseDlg::~ChooseDlg()
{

}

