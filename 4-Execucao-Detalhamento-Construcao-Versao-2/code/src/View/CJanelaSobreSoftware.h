#ifndef CJANELASOBRESOFTWARE_H
#define CJANELASOBRESOFTWARE_H

#include <QDialog>

namespace Ui {
class CJanelaSobreSofware;
}

class CJanelaSobreSofware : public QDialog
{
    Q_OBJECT

public:
    explicit CJanelaSobreSofware(QWidget *parent = nullptr);
    ~CJanelaSobreSofware();

private:
    Ui::CJanelaSobreSofware *ui;
};

#endif // CJANELASOBRESOFTWARE_H
