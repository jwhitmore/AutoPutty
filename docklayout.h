#ifndef DOCKLAYOUT_H
#define DOCKLAYOUT_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QVector>

struct SessionCfg {
  SessionCfg() {sessionConfigId = -1;}
  SessionCfg(QString LayoutObjectName, QString PuttySessionName, QString PuttyObjectName, int configID = -1)
    {
      layoutObjectName  = LayoutObjectName;
      sessionName       = PuttySessionName;
      sessionObjectName = PuttyObjectName;
      sessionConfigId   = configID;
    }
  QString layoutObjectName;
  QString sessionName;
  QString sessionObjectName;
  int sessionConfigId;
};
typedef SessionCfg SessionCfg;

class dockLayout
{
public:

  dockLayout(QString orgName, QString appName);

  QStringList getLayoutNames() const;
  QStringList getLayoutSessionNames(QString layout) const;
  QVector<SessionCfg> getLayoutSessions(QString layout);
  QByteArray getLayoutGeometry(QString layout) const;
  QByteArray getLayoutState(QString layout) const;
  QString getLayoutObjectName(QString layout);

  bool exists(QString layout);

  void deleteLayout(QString layout);
  void renameLayout(QString layout, QString newName);

private:
  QSettings *settings;

};

#endif // DOCKLAYOUT_H
