#ifndef EDITEXTRASETTINGSGROUPBOX_H
#define EDITEXTRASETTINGSGROUPBOX_H

#include <QGroupBox>
#include <QComboBox>
#include "ledcontroller.h"

#define A0GPIO   17
#define D0GPIO   16
#define D1GPIO   5
#define D2GPIO   4
#define D3GPIO   0
#define D4GPIO   2
#define D5GPIO   14
#define D6GPIO   12
#define D7GPIO   13
#define D8GPIO   15

static qint8 pin2GPIO(QString pinName)
{
    qint8 result = -1;
    if      (pinName == "None")
        result = -1;
    else if (pinName == "A0")
        result = A0GPIO;
    else if(pinName == "D0")
        result = D0GPIO;
    else if(pinName == "D1")
        result = D1GPIO;
    else if(pinName == "D2")
        result = D2GPIO;
    else if(pinName == "D3")
        result = D3GPIO;
    else if(pinName == "D4")
        result = D4GPIO;
    else if(pinName == "D5")
        result = D5GPIO;
    else if(pinName == "D6")
        result = D6GPIO;
    else if(pinName == "D7")
        result = D7GPIO;
    else if(pinName == "D8")
        result = D8GPIO;

    return result;
}

static QString GPIO2Pin(qint8 gpio)
{
    QString result;

    if     (gpio == -1)
        result = "None";
    else if(gpio == A0GPIO)
        result = "A0";
    else if(gpio == D0GPIO)
        result = "D0";
    else if(gpio == D1GPIO)
        result = "D1";
    else if(gpio == D2GPIO)
        result = "D2";
    else if(gpio == D3GPIO)
        result = "D3";
    else if(gpio == D4GPIO)
        result = "D4";
    else if(gpio == D5GPIO)
        result = "D5";
    else if(gpio == D6GPIO)
        result = "D6";
    else if(gpio == D7GPIO)
        result = "D7";
    else if(gpio == D8GPIO)
        result = "D8";

    return result;
}

static int getComboBoxItemIndex(QComboBox* c,QString pinName)
{
    for(int i = 0 ; i < c->count() ; i++)
    {
        if(c->itemText(i) == pinName)
        return i;
    }
    return 0;
}


namespace Ui {
class editExtraSettingsGroupBox;
}

class editExtraSettingsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit editExtraSettingsGroupBox(QWidget *parent = 0);
    ~editExtraSettingsGroupBox();
    void setClient(protocolClient *c);


protected:
    Ui::editExtraSettingsGroupBox *ui;
    ledController*                m_client;

    void populateComboBoxes();

signals:

protected slots:
    void updateLedCount();
    void enableMatrix(int index);
public slots:
    void loadSettings();
    void loadSettings(extraSettings s);
    void sendSettings();
};

#endif // EDITEXTRASETTINGSGROUPBOX_H
