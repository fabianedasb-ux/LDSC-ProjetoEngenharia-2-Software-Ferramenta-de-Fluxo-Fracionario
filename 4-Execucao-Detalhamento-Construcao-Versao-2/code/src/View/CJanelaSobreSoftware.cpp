#include "CJanelaSobreSoftware.h"
#include "ui_CJanelaSobreSoftware.h"

CJanelaSobreSofware::CJanelaSobreSofware(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CJanelaSobreSofware)
{
    ui->setupUi(this);
    ui->textEditSobreSoftware->setReadOnly(true);
    ui->textEditSobreLDSC->setReadOnly(true);
    ui->textEditSobreDesenvolvimento->setReadOnly(true);
    ui->textEditSobrePlataforma->setReadOnly(true);
}

CJanelaSobreSofware::~CJanelaSobreSofware()
{
    delete ui;
}
