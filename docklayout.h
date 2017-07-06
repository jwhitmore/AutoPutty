#ifndef DOCKLAYOUT_H
#define DOCKLAYOUT_H

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QVector>

struct SessionObjectName {
  SessionObjectName() {}
  SessionObjectName(QString s, QString n) {session = s; objName = n;}
  QString session;
  QString objName;
};
typedef SessionObjectName SessionObjectName;

class dockLayout
{
public:

  dockLayout(QString orgName, QString appName);

  QStringList getLayoutNames() const;
  QStringList getLayoutSessionNames(QString layout) const;
  QVector<SessionObjectName> getLayoutSessions(QString layout);
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
