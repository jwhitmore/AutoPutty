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
  QStringList rtn = settings->childKeys();
  settings->endGroup();
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
QVector<SessionObjectName> dockLayout::getLayoutSessions(QString layout)
{
  FUNC_DEBUG;
  DEBUG << "Layout: " << layout;

  settings->beginGroup(QString("layout/%1/sessions").arg(layout));
  QStringList id = settings->childGroups();
  settings->endGroup();

  QVector<SessionObjectName> rtn;

  for(int i = 0; i < id.count(); i++) {
    SessionObjectName son(
          settings->value(QString("layout/%1/sessions/%2/sessionName").arg(layout).arg(id.at(i))).toString(),
          settings->value(QString("layout/%1/sessions/%2/objectName").arg(layout).arg(id.at(i))).toString()
          );
    rtn.push_back(son);
    DEBUG << "Added Session " << (i+1) << ": " << son.session << " -- " << son.objName;
  }

  DEBUG << "Session Count: " << rtn.size();

  return rtn;
}

