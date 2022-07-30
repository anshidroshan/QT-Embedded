#ifndef DATABASE_H
#define DATABASE_H


#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>


class database
{
public:

    QSqlDatabase db;
    void db_disconnect()
    {
        db.close();
        db = QSqlDatabase();
        db.removeDatabase(QSqlDatabase::defaultConnection);
        //qDebug()<<("Dis-Connected......");
    }

    bool db_connect()
    {

        db = QSqlDatabase::addDatabase("QSQLITE");
       // db.setDatabaseName("/mnt/jffs2/29042021120633SAMPSAMP.db");

        db.setDatabaseName("/home/weirdo/dairypos/29042021120633SAMPSAMP.db");
        return db.open();

        if(!db.open())
        {
            qDebug()<<("Failed to open the database");
            QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().text());
            return false;
        }
        else
        {
            qDebug()<<("Connected......");
            return true;
        }


        QFile db_check;

        db_check.setFileName("/home/weirdo/dairypos/29042021120633SAMPSAMP.db");
        //db_check.setFileName("/mnt/jffs2/29042021120633SAMPSAMP.db");


        if(db_check.exists() == true)
        {
            qDebug()<<"Database Found";



        }

        else

            qDebug()<<"Database Connection Successful";
    }




};
#endif // DATABASE_H
