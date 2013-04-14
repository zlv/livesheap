#include "chooseuser.h"
#include "ui_chooseuser.h"
#include "newuser.h"

namespace livesheap{
ChooseUser::ChooseUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseUser)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(newUser()));
}

ChooseUser::~ChooseUser()
{
    delete ui;
}

void ChooseUser::newUser()
{
    NewUser *user = new NewUser(this);
    user->show();
}
}
