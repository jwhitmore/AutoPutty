#ifndef MANAGELAYOUTS_H
#define MANAGELAYOUTS_H

#include <QDialog>

#include <docklayout.h>

namespace Ui {
class manageLayouts;
}

class manageLayouts : public QDialog
{
  Q_OBJECT

public:
  explicit manageLayouts(QWidget *parent = 0);
  ~manageLayouts();

  void setLayouts(dockLayout* dl);


private:
  Ui::manageLayouts *ui;
  dockLayout* layouts;

  void refresh();

private slots:
  void on_listLayouts_currentTextChanged(const QString& currentText);

  void on_btnRename_clicked();
  void on_btnDelete_clicked();


};

#endif // MANAGELAYOUTS_H
