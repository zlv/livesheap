/*
Другой из диалогов / just that another one
copyright (c) 2013 Евгений Лежнин
  */
#ifndef NEWUSER_H
#define NEWUSER_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class NewUser;
}

class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;
namespace livesheap{
class Lobby;
class NewUser : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewUser(Lobby*, QWidget *parent = 0);
    ~NewUser();

private:
    Lobby *lobby_;
    Ui::NewUser *ui;
    QUrl serverUrl; //сохраняется в самом начале

private:
    void reaction(const QString&); //реакция на сообщение от сервера / ... on server message
public slots:
    void ok();
    void reply(QNetworkReply*); //сообщение от сервера / ... from server
signals:
    void postSignal(const QNetworkRequest&, const QByteArray&);
};
}

#endif // NEWUSER_H
