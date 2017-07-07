#include <QVariant>

#include "debug.h"
#include "docklayout.h"

//=============================================================================
//=============================================================================
dockLayout::dockLayout(QString orgName, QString appName)
{
  this->settings = new QSettings(orgName, appName);
}

//=============================================================================
//=============================================================================
void dockLayout::deleteLayout(QString layout)
{
  FUNC_DEBUG;
  this->settings->remove(QString("layout/%1").arg(layout));
}

//=============================================================================
//=============================================================================
void dockLayout::renameLayout(QString layout, QString newName)
{
  FUNC_DEBUG;
  DEBUG << "Renaming " << layout << " to " << newName;

  QVariant layGeometry;
  QVariant layObjectName;
  QVariant layWindowState;

  QVector<SessionCfg> vSessions = getLayoutSessions(layout);

  settings->beginGroup(QString("layout/%1").arg(layout));
  layGeometry = settings->value("geometry");
  layObjectName = settings->value("objectName");
  layWindowState = settings->value("windowState");
  settings->endGroup();

  settings->remove(QString("layout/%1").arg(layout));

  settings->setValue(QString("layout/%1/geometry").arg(newName), layGeometry);
  settings->setValue(QString("layout/%1/objectName").arg(newName), layObjectName);
  settings->setValue(QString("layout/%1/windowState").arg(newName), layWindowState);

  for (int i = 0; i < vSessions.size(); i++ ) {
    const SessionCfg qv = vSessions.at(i);

    settings->setValue(QString("layout/%1/sessions/%2/objectName").arg(newName).arg(qv.sessionConfigId), qv.sessionObjectName);
    settings->setValue(QString("layout/%1/sessions/%2/sessionName").arg(newName).arg(qv.sessionConfigId), qv.sessionName);
  }

}

//=============================================================================
//=============================================================================
QStringList dockLayout::getLayoutNames() const
{
  FUNC_DEBUG;

  settings->beginGroup("layout");
  QStringList rtn = settings->childGroups();
  settings->endGroup();

  return rtn;
}

//=============================================================================
//=============================================================================
QStringList dockLayout::getLayoutSessionNames(QString layout) const
{
  FUNC_DEBUG;
  DEBUG << "Layout: " << layout;

  settings->beginGroup(QString("layout/%1/sessions").arg(layout));
  QStringList id = settings->childGroups();
  settings->endGroup();

  QStringList rtn;

  for(int i = 0; i < id.count(); i++) {
    rtn << settings->value(QString("layout/%1/sessions/%2/sessionName").arg(layout).arg(id.at(i))).toString();
  }

  DEBUG << "Session Count: " << rtn.size();

  return rtn;
}

//=============================================================================
//=============================================================================
QByteArray dockLayout::getLayoutGeometry(QString layout) const
{
  FUNC_DEBUG;
  DEBUG << "Layout: " << layout;
  return settings->value(QString("layout/%1/geometry").arg(layout)).toByteArray();
}

//=============================================================================
//=============================================================================
QByteArray dockLayout::getLayoutState(QString layout) const
{
  FUNC_DEBUG;
  DEBUG << "Layout: " << layout;
  return settings->value(QString("layout/%1/windowState").arg(layout)).toByteArray();
}

//=============================================================================
//=============================================================================
bool dockLayout::exists(QString layout)
{
  FUNC_DEBUG;
  DEBUG << "Layout: " << layout;
  return settings->contains(QString("layout/%1").arg(layout));
}

//=============================================================================
//=============================================================================
QString dockLayout::getLayoutObjectName(QString layout)
{
  FUNC_DEBUG;
  DEBUG << "Layout: " << layout;

  QString rtn = "";

  if (settings->contains(QString("layout/%1/objectName").arg(layout))) {
    rtn = settings->value(QString("layout/%1/objectName").arg(layout)).toString();
    DEBUG << "Found Object Name for Layout " << layout << ": " << rtn;
    return rtn;
  } else {
    DEBUG << "Object Name Not Found for Layout " << layout;
  }
  return rtn;
}

//=============================================================================
//=============================================================================
QVector<SessionCfg> dockLayout::getLayoutSessions(QString layout)
{
  FUNC_DEBUG;
  DEBUG << "Layout: " << layout;

  settings->beginGroup(QString("layout/%1/sessions").arg(layout));
  QStringList id = settings->childGroups();
  settings->endGroup();

  QVector<SessionCfg> rtn;

  for(int i = 0; i < id.count(); i++) {
    SessionCfg scfg(
          settings->value(QString("layout/%1/objectName").arg(layout)).toString(),
          settings->value(QString("layout/%1/sessions/%2/sessionName").arg(layout).arg(id.at(i))).toString(),
          settings->value(QString("layout/%1/sessions/%2/objectName").arg(layout).arg(id.at(i))).toString(),
          id.at(i).toInt()
          );
    rtn.push_back(scfg);
    DEBUG << "Added Session " << id.at(i) << ": " << scfg.sessionName << " -- " << scfg.sessionObjectName;
  }

  DEBUG << "Session Count: " << rtn.size();

  return rtn;
}
