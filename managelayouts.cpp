#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>

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
    // pointer supposed to be set by caller.
    // something didn't happen as it should've.
    return;
  }

  DEBUG << "count: " << this->layouts->getLayoutNames().count();
  DEBUG << "layouts: " << this->layouts->getLayoutNames();

  ui->listLayouts->clear();
  ui->listLayouts->addItems(layouts->getLayoutNames());

  if ( ui->listLayouts->count() > 0 ) {
    ui->listLayouts->setCurrentRow(0);
  }
}

//=============================================================================
//=============================================================================
void manageLayouts::on_btnRename_clicked()
{
  FUNC_DEBUG;

  QString originalName = ui->listLayouts->currentItem()->text();

  bool ok;
  QString newName = QInputDialog::getText(this, "Rename Layout", QString("Enter a new name for layout '%1':").arg(originalName), QLineEdit::Normal, "", &ok);
  if (ok && !newName.isEmpty()) {

    if ( this->layouts->exists(newName)) {
      QMessageBox err;
      err.setWindowTitle("Error");
      err.setText(QString("A layout already exists with the name '%1'").arg(newName));
      err.setStandardButtons(QMessageBox::Ok);
      err.exec();
      return;
    }

    this->layouts->renameLayout(originalName, newName);

    refresh();
  }
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

