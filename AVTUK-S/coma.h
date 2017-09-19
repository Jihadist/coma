#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QResizeEvent>
#include "../config/confdialog.h"
#include "../mainwindow.h"
#include "../publicclass.h"

class Coma : public MainWindow
{
    Q_OBJECT

public:
    Coma(QWidget *parent = 0);
    ~Coma();

signals:

public slots:

private slots:
    void Emul2x();

private:
    void SetupUI();
    void AddActionsToMenuBar(QMenuBar *menubar);
    void Stage3();
};

#endif // COMA_H
