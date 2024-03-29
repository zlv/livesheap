/*
Один из диалогов
copyright (c) 2013 Евгений Лежнин
  */
#ifndef CHOOSEUSER_H
#define CHOOSEUSER_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class ChooseUser;
}

class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;
namespace livesheap{
class Lobby;
class ChooseUser : public QDialog
{
    Q_OBJECT
    
public:
    explicit ChooseUser(QWidget *parent = 0);
    ~ChooseUser();
    
private:
    Lobby *lobby_;
    Ui::ChooseUser *ui;
    QUrl serverUrl; //сохраняется в самом начале

private:
    void reaction(const QString&); //реакция на сообщение от сервера / ... on server message
public slots:
    void newUser(); //вызвать ещё один диалог / call another one
    void ok();
    void reply(QNetworkReply*); //сообщение от сервера / ... from server
signals:
    void postSignal(const QNetworkRequest&, const QByteArray&);
};
}

#endif // CHOOSEUSER_H
