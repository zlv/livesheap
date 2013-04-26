#include "chatstartwindow.h"
#include "ui_chatstartwindow.h"

namespace livesheap{
ChatStartWindow::ChatStartWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatStartWindow), currentPixmap_(0)
{
    ui->setupUi(this);
}

ChatStartWindow::~ChatStartWindow()
{
    delete ui;
}

void ChatStartWindow::changePixmap()
{
    currentPixmap_ = (currentPixmap_+1)%pixmaps;
    ui->label->setPixmap(QString(":/livesheap/data/loading/%1.png").arg(currentPixmap_));
}
}
