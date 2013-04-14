#ifndef CHOOSEUSER_H
#define CHOOSEUSER_H

#include <QDialog>

namespace Ui {
class ChooseUser;
}

namespace livesheap{
class ChooseUser : public QDialog
{
    Q_OBJECT
    
public:
    explicit ChooseUser(QWidget *parent = 0);
    ~ChooseUser();
    
private:
    Ui::ChooseUser *ui;

public slots:
    void newUser();
};
}

#endif // CHOOSEUSER_H