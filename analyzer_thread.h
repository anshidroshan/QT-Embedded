#ifndef ANALYZER_THREAD_H
#define ANALYZER_THREAD_H

#include <QObject>
#include<QtCore>
#include<QDebug>

class Analyzer_thread : public QObject
{
    Q_OBJECT
public:
    explicit Analyzer_thread(QObject *parent = 0);

    void requestWork();

    void abort();

    QString output;

private:

    bool _abort;

    bool _working;

    QMutex mutex;

signals:

    void workRequested();

    void valueChanged(const QString &value);

    void finished();

    void error(QString err);

public slots:

    void doWork();
};

#endif // ANALYZER_THREAD_H
