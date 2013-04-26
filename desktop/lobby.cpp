#include "lobby.h"
#include "ui_lobby.h"
#include "chooseuser.h"
#include "managerthread.h"
#include "chat.h"
#include "chatstartwindow.h"
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
    pConstantManager = new ManagerThread();

    pConstantManager->allOk = 1;

    show();

    chatStartWaitWindow_ = new ChatStartWindow(this);
    if (!bLoggedIn_)
    {
        ChooseUser *user = new ChooseUser(this);
        user->show();
    }
    connect(this,SIGNAL(postSignal(const QNetworkRequest&, const QByteArray&)),pManagerThread,SLOT(post(const QNetworkRequest&, const QByteArray&)));
    connect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(userSelected(QModelIndex)));
}

Lobby::~Lobby()
{
    delete ui;
    delete pManagerThread;
}

void Lobby::start()
{
    bLoggedIn_ = 1;
    QNetworkRequest request;
    request.setUrl(serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    connect(pManagerThread, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
    //отправляем / send it
    emit postSignal(request, QString("type=greetings&username=%1").arg(username).toUtf8());

    howAreYouTimer_ = new QTimer(this);
    connect(howAreYouTimer_, SIGNAL(timeout()), this, SLOT(howAreYou()));
    howAreYouTimer_->start(howAreYouTimeout);
    connect(pConstantManager, SIGNAL(finished(QNetworkReply*)), pConstantManager, SLOT(replyHowAreYou(QNetworkReply*)));
    pConstantManager->bDisconnected = pConstantManager->bSomeoneWantToPlay = 0;
}

void Lobby::reaction(const QString& s)
{
    QStringList list = s.split(" ");
    int i=0;
    if (list[i++]=="200")
    {
        users_.clear();
        QStandardItemModel *model = new QStandardItemModel(ui->listView);
        id_ = list[i++].toInt();
        int size = list[i++].toDouble();
        for (int j=0; j<size; j++)
        {
            UserItem * item = new UserItem();
            int starti = i;
            while (list[i]!="$")
            {
                if (i!=starti)
                    item->name+=" ";
                item->name+=list[i++];
            }
            i++;
            item->ip=list[i++];
            item->id=list[i++].toInt();
            if (list[i++]=="0")
                item->bImage = 0;
            else
                item->bImage = 1;
            model->appendRow(new QStandardItem(item->name));
            users_.push_back(item);
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
    if (!bLoggedIn_)
    {
        e->accept();
        return;
    }
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
        exiTimer_ = new QTimer(this);
        connect(exiTimer_, SIGNAL(timeout()), this, SLOT(exitTimerUpdate()));
        exiTimer_->start(normalTimeout);
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

void Lobby::userSelected(QModelIndex index)
{
    QNetworkRequest request;
    request.setUrl(serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    //отправляем / send it
    if (users_[index.row()])
        pManagerThread->emitPost(request, QString("type=wannaChatWith&id=%1&secondid=%2").arg(id_).arg(users_[index.row()]->id).toUtf8());
    else
        return;
    connect(pManagerThread, SIGNAL(finished(QNetworkReply*)), pManagerThread, SLOT(replyChatStatus(QNetworkReply*)));
    chaTimer_ = new QTimer(this);
    waitingId_ = index.row();
    connect(chaTimer_, SIGNAL(timeout()), this, SLOT(waitForAnswerStartChat()));
    chaTimer_->start(normalTimeout);
    chatStartWaitWindow_->show();
    pManagerThread->bCanStartChat = pManagerThread->bChatRejected = pManagerThread->bGainId = bStartedAskingIsSheAnswered_ = 0;
}

void Lobby::waitForAnswerStartChat()
{
    chatStartWaitWindow_->changePixmap();
    if (!pManagerThread->bGainId)
        return;
    if (pManagerThread->bCanStartChat || pManagerThread->bChatRejected)
    {
        chatStartWaitWindow_->hide();
        disconnect(chaTimer_, SIGNAL(timeout()), this, SLOT(waitForAnswerStartChat()));
    }
    else if (pManagerThread->bCanStartChat)
    {
        startChat();
    }
    else if (!bStartedAskingIsSheAnswered_)
    {
        QNetworkRequest request;
        request.setUrl(serverUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
        pManagerThread->emitPost(request, QString("type=isSheAnswered&id=%1").arg(pManagerThread->chatId).toUtf8());
        connect(pManagerThread, SIGNAL(finished(QNetworkReply*)), pManagerThread, SLOT(replyWaitChatStatus(QNetworkReply*)));
        bStartedAskingIsSheAnswered_ = 1;
    }
}

void Lobby::startChat()
{
    Chat *chat = new Chat(*users_[waitingId_],this);
    chat->show();
}

void Lobby::clear()
{
    ui->listView->model()->removeRows(0,ui->listView->model()->rowCount());
}

void Lobby::howAreYou()
{
    if (pConstantManager->allOk && !pConstantManager->bDisconnected && !pConstantManager->bSomeoneWantToPlay)
    {
        pConstantManager->allOk = 0;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->listView->model());
        QStringList list = pConstantManager->okSplit;
        if (list.size())
        {
            bool action;
            int i=1;
            int id;
            qDebug() << pConstantManager->okSplit.size() << endl;
            int size = list[i++].toDouble();
            for (int j=0; j<size; j++)
            {
                action = list[i++].toInt();
                id = list[i++].toInt();
                if (action)
                {
                    UserItem *item = new UserItem();
                    int starti = i;
                    while (list[i]!="$")
                    {
                        if (i!=starti)
                            item->name+=" ";
                        item->name+=list[i++];
                    }
                    i++;
                    item->ip=list[i++];
                    item->id=id;
                    if (list[i++]=="0")
                        item->bImage = 0;
                    else
                        item->bImage = 1;
                    model->appendRow(new QStandardItem(item->name));
                    users_.push_back(item);
                }
                else
                {
                    for (int i=0; i<users_.size(); i++)
                        if (users_[i]->id==id)
                        {
                            users_.erase(users_.begin()+i);
                            model->removeRow(i);
                        }
                }
            }
        }
        QNetworkRequest request;
        request.setUrl(serverUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
        pConstantManager->emitPost(request, QString("type=howAreYou&id=%1").arg(id_).toUtf8());
    }
    else if (pConstantManager->bDisconnected)
    {
        disconnect(howAreYouTimer_, SIGNAL(timeout()), this, SLOT(howAreYou()));
        QMessageBox::about(this,tr("Disconnected"),tr("server rejected connection"));
        clear();
    }
    else if (pConstantManager->bSomeoneWantToPlay)
    {
        QMessageBox::Button b = QMessageBox::information(this,tr("Invitation"),tr("%1 wanna play with you. Accept?").arg(users_[pConstantManager->otherUserId]->name),QMessageBox::Ok,QMessageBox::Cancel);
        if (b==QMessageBox::Ok)
        {
            waitingId_ = pConstantManager->otherUserId;
            startChat();
        }
        pConstantManager->allOk = 1;
    }
}
}
