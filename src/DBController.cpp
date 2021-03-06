// Local
#include "DBController.h"

// Qt
#include <QDebug>
#include <QFile>
#include <QVariantMap>
#include <QStandardPaths>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace
{
  const char* const dataBaseName = "PersonalFinances.db";
}

DBController::DBController(const QString& dbFileName, QObject *parent)
  : QObject(parent)
  , m_dbWorker(new DBWorker(dbFileName))
{
  m_thread = new QThread;
  m_thread->start();

  m_dbWorker->moveToThread(m_thread);
  connect(m_dbWorker, SIGNAL(resultProcess(bool,QString)), SIGNAL(resultProcess(bool,QString)));
  connect(m_dbWorker, SIGNAL(getStatistic(QVariantList)), SIGNAL(getStatistic(QVariantList)));

  loadFromFile(dbFileName);
}


DBController::~DBController()
{
  m_thread->quit();
  m_thread->wait();
  delete m_dbWorker;
  delete m_thread;
}


void DBController::loadFromFile(const QString& dbFileName)
{
  QString dbName;
  // Get name
  if (dbFileName.isEmpty() ||
        dbFileName.isNull() ||
          !QFile(dbFileName).exists())
  {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (!QDir(dataPath).exists())
      QDir().mkpath(dataPath);

    dbName = dataPath + QDir::separator() + dataBaseName;
  }
  else
  {
    dbName = dbFileName;
  }

  qDebug() << "Load database from file:" << dbName;
  //if (!QFile(dbName).exists())
  //  _firstLaunch = true;


  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbName);
  if (!db.open())
    qFatal("%s", db.lastError().text().toStdString().c_str());

  QString script(
    "CREATE TABLE IF NOT EXISTS PersonalFinances \
     ( \
       id INTEGER PRIMARY KEY AUTOINCREMENT, \
       summa INTEGER, \
       comment TEXT, \
       date INTEGER \
     );");

  QSqlQuery query;
  query.prepare(script);
  if (!query.exec())
    qCritical("%s", query.lastError().text().toStdString().c_str());
}


void DBController::insert(const QVariantMap& data)
{
  QMetaObject::invokeMethod(m_dbWorker, "insert", Qt::QueuedConnection, Q_ARG(QVariantMap, data));
}


void DBController::statistic(const QDate& date)
{
  QMetaObject::invokeMethod(m_dbWorker, "statistic", Qt::QueuedConnection, Q_ARG(QDate, date));
}
