#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
#include "useritem.h"

namespace Ui {
class Chat;
}

namespace livesheap{
class Chat : public QDialog
{
    Q_OBJECT
    
public:
    explicit Chat(UserItem&, QWidget *parent = 0);
    ~Chat();
    
private:
    Ui::Chat *ui;
    UserItem& userItem_;
};
}

#endif // CHAT_H
