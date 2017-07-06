#include <QMessageBox>

#include "debug.h"

#include "managelayouts.h"
#include "ui_managelayouts.h"



//=============================================================================
//=============================================================================
manageLayouts::manageLayouts(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::manageLayouts)
{
  ui->setupUi(this);
}

//=============================================================================
//=============================================================================
manageLayouts::~manageLayouts()
{
  delete ui;
}

//=============================================================================
//=============================================================================
void manageLayouts::setLayouts(dockLayout* dl)
{
  FUNC_DEBUG;

  this->layouts = dl;
  refresh();
}


//=============================================================================
//=============================================================================
void manageLayouts::on_listLayouts_currentTextChanged(const QString& currentText)
{
  FUNC_DEBUG;
  DEBUG << "Layout Selection Changed to [" << currentText << "]";

  ui->listSessions->clear();
  ui->listSessions->addItems(this->layouts->getLayoutSessionNames(currentText));
}

//=============================================================================
//=============================================================================
void manageLayouts::refresh()
{
  FUNC_DEBUG;

  if ( this->layouts == NULL ) {
    DEBUG << "dockLayout object is null";
    return;
  }

  DEBUG << "count: " << this->layouts->getLayoutNames().count();
  DEBUG << "layouts: " << this->layouts->getLayoutNames();

  ui->listLayouts->clear();

  QStringList list = layouts->getLayoutNames();
  QStringList::iterator itr;

  for (itr = list.begin(); itr != list.end(); itr++) {
    DEBUG << "itr [" << (*itr) << "]";
    ui->listLayouts->addItem(*itr);
  }
}
//=============================================================================
//=============================================================================
void manageLayouts::on_btnRename_clicked()
{
  FUNC_DEBUG;
}

//=============================================================================
//=============================================================================
void manageLayouts::on_btnDelete_clicked()
{
  FUNC_DEBUG;

  QMessageBox msg;
  msg.setText(QString("Delete %1 -- Are you sure?").arg(ui->listLayouts->currentItem()->text()));
  msg.setWindowTitle("Confirm Layout Delete");
  msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msg.setDefaultButton(QMessageBox::No);
  int rtn = msg.exec();

  if (rtn == QMessageBox::No) return;

  this->layouts->deleteLayout(ui->listLayouts->currentItem()->text());

  refresh();

}

