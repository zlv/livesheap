#include "lobby.h"
#include "ui_lobby.h"
#include "chooseuser.h"
#include "managerthread.h"
#include <QtNetwork>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QCloseEvent>

namespace livesheap{
Lobby::Lobby(QWidget *parent) : QDialog(parent), ui(new Ui::Lobby), bLoggedIn_(0), serverUrl(/*"http://livesheap.appspot.com/"*/"http://localhost:8080"), exitTimerCount_(0)
{
    ui->setupUi(this);

    //ищем сервер в файле
    QFile file("server.cfg");
    char c;
    file.getChar(&c);
    if (file.canReadLine())
        serverUrl = QUrl(file.readLine());

    pManagerThread = new ManagerThread();

    if (!bLoggedIn_)
    {
        ChooseUser *user = new ChooseUser(this);
        user->show();
        bLoggedIn_ = 1;
    }
    connect(this,SIGNAL(postSignal(const QNetworkRequest&, const QByteArray&)),pManagerThread,SLOT(post(const QNetworkRequest&, const QByteArray&)));
}

Lobby::~Lobby()
{
    delete ui;
    delete pManagerThread;
}

void Lobby::start()
{
    QNetworkRequest request;
    request.setUrl(serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    connect(pManagerThread, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
    //отправляем / send it
    emit postSignal(request, QString("type=greetings&username=%1").arg(username).toUtf8());
}

class UserItem
{
public:
    QString name;
    bool bImage;
    QIcon image;
};

void Lobby::reaction(const QString& s)
{
   QStringList list = s.split(" ");
   int i=0;
   if (list[i++]=="200")
   {
       QStandardItemModel *model = new QStandardItemModel(ui->listView);
       id_ = list[i++].toInt();
       int size = list[i++].toDouble();
       for (int j=0; j<size; j++)
       {
           UserItem nextItem;
           while (list[i]!="$")
           {
               if (i!=1)
                   nextItem.name+=" ";
               nextItem.name+=list[i++];
           }
           i++;
           if (list[i++]=="0")
               nextItem.bImage = 0;
           else
               nextItem.bImage = 1;
           model->appendRow(new QStandardItem(nextItem.name));
           /*else
           {
               nextItem.bImage = 1;
               nextItem = QImage.fromData(  );
           }*/
       }
       ui->listView->setModel(model);
   }
   else
   {
       QMessageBox::about(this,tr("Something went wrong"),tr("Unknown error"));
   }
   disconnect(pManagerThread, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
}

void Lobby::closeEvent(QCloseEvent* e)
{
    if (pManagerThread->bCanQuit)
    {
        e->accept();
    }
    else
    {
        QNetworkRequest request;
        request.setUrl(serverUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
        //отправляем / send it
        pManagerThread->emitPost(request, QString("type=bye&id=%1").arg(id_).toUtf8());
        connect(pManagerThread, SIGNAL(finished(QNetworkReply*)), pManagerThread, SLOT(replyCanQuit(QNetworkReply*)));
        timer_ = new QTimer(this);
        connect(timer_, SIGNAL(timeout()), this, SLOT(exitTimerUpdate()));
        timer_->start(50);
        e->ignore();
    }
}

void Lobby::reply(QNetworkReply* r){
    reaction(r->readAll());
}

void Lobby::exitTimerUpdate()
{
    if (exitTimerCount_++ >= exitTimerMaxCount || pManagerThread->bCanQuit)
        QApplication::quit();
}
}
