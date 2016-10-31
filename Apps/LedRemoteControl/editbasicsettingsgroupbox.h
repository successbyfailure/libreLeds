#ifndef EDITBASICSETTINGSGROUPBOX_H
#define EDITBASICSETTINGSGROUPBOX_H

#include <QGroupBox>
#include <QComboBox>
#include "protocolclient.h"

namespace Ui {
class editBasicSettingsGroupBox;
}

class editBasicSettingsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit editBasicSettingsGroupBox(QWidget *parent = 0);
    ~editBasicSettingsGroupBox();
    void setClient(protocolClient *c);

protected:
    Ui::editBasicSettingsGroupBox *ui;
    protocolClient*                m_client;

    void populateComboBoxes();

signals:

public slots:
    void loadSettings();
    void loadSettings(basicSettings s);
    void sendSettings();
};

#endif // EDITBASICSETTINGSGROUPBOX_H
