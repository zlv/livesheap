#include "newuser.h"
#include "ui_newuser.h"
#include <QtNetwork>
#include <QMessageBox>

namespace livesheap{
NewUser::NewUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewUser)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(ok()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(close()));
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
    QUrl serverUrl("http://localhost:8080");
    QNetworkRequest request;
    QUrl params;
    params.addQueryItem("version","1.0");
    params.addQueryItem("siteid","12345");
    params.addQueryItem("recordid","12345");
    request.setUrl(serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    pManager->post(request, QString("username=%1&password=%2").arg(ui->lineEdit->text()).arg(ui->lineEdit_2->text()).toUtf8());
}

void NewUser::reply(QNetworkReply* r){
    QMessageBox::about(0,"",r->readAll());
}
}
