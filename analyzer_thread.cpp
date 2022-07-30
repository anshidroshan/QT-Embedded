#include "analyzer_thread.h"
extern "C"
{
#include"read_analyzer.h"
}


Analyzer_thread::Analyzer_thread(QObject *parent) :
    QObject(parent)
{
    _working =false;
    _abort = false;
}

void Analyzer_thread::requestWork()
{
    mutex.lock();
    _working = true;
    _abort = false;
    qDebug()<<"Test Requested in Thread "<<thread()->currentThreadId();
    mutex.unlock();

    emit workRequested();
}

void Analyzer_thread::abort()
{
    mutex.lock();
    if (_working) {
        _abort = true;
        qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
    }
    mutex.unlock();
}

void Analyzer_thread::doWork()
{
    qDebug()<<"Thread started in "<<thread()->currentThreadId();

    qDebug()<<"Analyzer Test Started";

    int ret;

    char recv[1024]="";

    memset(recv,0x00,1024);

    ret = open_AN();

    if (ret != 0)
    {
        printf("Opening Port Failed\n");

    }
    while (1)
    {
        int ret1;
        ret1 = data_onport();

        if (ret1 > 0)

            break;

        sleep(1);
    }

    qDebug()<<"Indefinite while loop exited";

    read_data_an(recv);

    qDebug()<<"Raw data from analyzer is :"<<recv;

    output = recv;

    qDebug()<<"Broadcasted value from thread is : "<<output;

    emit valueChanged(output);


    // Set _working to false, meaning the process can't be aborted anymore.

    mutex.lock();
    _working = false;
    mutex.unlock();

    qDebug()<<"Test Completed in Thread "<<thread()->currentThreadId();

    emit finished();
}
