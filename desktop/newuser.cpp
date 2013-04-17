//copyright (c) 2013 Evgenii Lezhnin
#include "newuser.h"
#include "ui_newuser.h"
#include "lobby.h"
#include "managerthread.h"
#include <QtNetwork>
#include <QMessageBox>
#include <QDebug>

namespace livesheap{
NewUser::NewUser(Lobby *l, QWidget *parent) : QDialog(parent), lobby_(l), ui(new Ui::NewUser), serverUrl("http://localhost:8080")
{
    ui->setupUi(this);

    serverUrl = lobby_->serverUrl;

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(ok()));
    connect(this,SIGNAL(postSignal(const QNetworkRequest&, const QByteArray&)),lobby_,SIGNAL(postSignal(const QNetworkRequest&, const QByteArray&)));
}

NewUser::~NewUser()
{
    delete ui;
}

void NewUser::ok()
{
    QNetworkRequest request;
    request.setUrl(serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    //пароли разные
    if (ui->lineEdit_2->text()!=ui->lineEdit_3->text())
    {
        QMessageBox::warning(this,tr("Passwords didn't match!"), tr("Passwords didn't match! Please reenter passwords!"));
        return;
    }
    connect(lobby_->pManagerThread, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
    //отправляем / send it
    emit postSignal(request, QString("username=%1&password=%2").arg(ui->lineEdit->text()).arg(ui->lineEdit_2->text()).toUtf8());
}

void NewUser::reaction(const QString& s)
{
   if (s=="200")
   {
       QMessageBox::about(this,tr("Success!"),tr("Your account was successfully registered on server!"));
   }
   else
   {
       QStringList list = s.split(" ");
       qDebug() << s;
       if (list[0]=="400")
           QMessageBox::about(this,tr("Failed!"),tr("Sorry, account %1 already exists :( Try another name").arg(list[1]));
       else
           QMessageBox::about(this,tr("Something went wrong"),tr("Unknown error"));
   }
   disconnect(lobby_->pManagerThread, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
}

void NewUser::reply(QNetworkReply* r){
    reaction(r->readAll());
}
}
