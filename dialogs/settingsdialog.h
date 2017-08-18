#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

signals:

public slots:

private:
    void SetupUI();
    void Fill();

private slots:
    void AcceptSettings();
    void SetHomeDir();
};

#endif // SETTINGSDIALOG_H
