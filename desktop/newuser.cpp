//copyright (c) 2013 Evgenii Lezhnin
#include "newuser.h"
#include "ui_newuser.h"
#include <QtNetwork>
#include <QMessageBox>
#include <QDebug>

namespace livesheap{
NewUser::NewUser(QWidget *parent) :
    QDialog(parent), ui(new Ui::NewUser), serverUrl("http://localhost:8080")
{
    ui->setupUi(this);

    //ищём сервер в файле
    QFile file("server.cfg");
    if (file.canReadLine())
        serverUrl = QUrl(file.readLine());

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(ok()));
    pManager = new QNetworkAccessManager(this);
    connect(pManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
}

NewUser::~NewUser()
{
    delete ui;
    delete pManager;
}

void NewUser::ok()
{
    QNetworkRequest request;
    QUrl params;
    //ненужная информация
    params.addQueryItem("version","1.0");
    params.addQueryItem("siteid","12345");
    params.addQueryItem("recordid","12345");
    request.setUrl(serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    //пароли разные
    if (ui->lineEdit_2->text()!=ui->lineEdit_3->text())
    {
        QMessageBox::warning(this,tr("Passwords didn't match!"), tr("Passwords didn't mutch! Please reenter passwords!"));
        return;
    }
    //отправляем / send it
    pManager->post(request, QString("username=%1&password=%2").arg(ui->lineEdit->text()).arg(ui->lineEdit_2->text()).toUtf8());
}

QString NewUser::reaction(const QString& s)
{
   if (s=="200") return tr("Your account was successfully registered on server!");
   else
   {
       QStringList list = s.split(" ");
       qDebug() << s;
       if (list[0]=="400")
          return tr("Sorry, account %1 already exists :( Try another name").arg(list[1]);
   }
   return tr("Unknown error");
}

void NewUser::reply(QNetworkReply* r){
    QMessageBox::about(this,"",reaction(r->readAll()));
}
}
