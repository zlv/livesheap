#include "chat.h"
#include "ui_chat.h"

namespace livesheap{
Chat::Chat(UserItem& u, QWidget *parent) : QDialog(parent), ui(new Ui::Chat), userItem_(u)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}
}
