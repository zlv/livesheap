//copyright (c) 2013 Evgenii Lezhnin
#include "chooseuser.h"
#include "ui_chooseuser.h"
#include "newuser.h"
#include "lobby.h"
#include "managerthread.h"
#include <QtNetwork>
#include <QMessageBox>

namespace livesheap{
ChooseUser::ChooseUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseUser)
{
    ui->setupUi(this);

    if (dynamic_cast<Lobby*>(parent))
    {
        lobby_ = dynamic_cast<Lobby*>(parent);
        serverUrl = lobby_->serverUrl;
    }

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(newUser()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(ok()));
    connect(this,SIGNAL(postSignal(const QNetworkRequest&, const QByteArray&)),lobby_,SIGNAL(postSignal(const QNetworkRequest&, const QByteArray&)));
}

ChooseUser::~ChooseUser()
{
    delete ui;
}

void ChooseUser::newUser()
{
    NewUser *user = new NewUser(lobby_,this);
    user->show();
}

void ChooseUser::ok()
{
    QNetworkRequest request;
    request.setUrl(serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    //отправляем / send it
    connect(lobby_->pManagerThread, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
    emit postSignal(request, QString("username=%1&password=%2&type=login").arg(ui->lineEdit->text()).arg(ui->lineEdit_2->text()).toUtf8());
}

void ChooseUser::reaction(const QString& s)
{
   QStringList list = s.split(" ");
   if (list[0]=="200")
   {
       lobby_->username = list[1];
       disconnect(lobby_->pManagerThread, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
       lobby_->start();
       close();
   }
   else
   {
       if (list[0]=="400")
           if (list[1]=="0")
               QMessageBox::about(this,tr("Error"),tr("Account %1 doesn't exists").arg(list[2]));
           else if (list[1]=="2")
               QMessageBox::about(this,tr("Error"),tr("You are already there with nick %1").arg(list[2]));
           else
               QMessageBox::about(this,tr("Error"),tr("Wrong password"));
       else
           QMessageBox::about(this,tr("Something went wrong"),tr("Unknown error"));
       disconnect(lobby_->pManagerThread, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
   }
}

void ChooseUser::reply(QNetworkReply* r){
    reaction(r->readAll());
}
}
