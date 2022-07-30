#include "dpos.h"
#include "ui_dpos.h"
#include "database.h"
#include"analyzer_thread.h"
#include<QMessageBox>
#include<QDebug>
#include<QFile>
#include<QRegExp>
#include<QRegExpValidator>
#include<QValidator>
#include<QString>
#include<QtGlobal>
#include<QCommonStyle>
#include<QDateTime>

extern "C"
{
#include"read_weighscale.h"
#include"gl11.h"
}



dpos::dpos(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dpos)
{
    ui->setupUi(this);


    //Shell Commands
    QProcess process_Sync;
    process_Sync.startDetached("/bin/sh", QStringList()<< "/root/dairypos.sh");


    //Timer Setup
    QDateTime dtime=QDateTime::currentDateTime();
    dateStr=dtime.toString("dd-MMM-yyyy [ddd]");
    timeStr=dtime.toString("hh:mm:ss AP");
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(clock()));
    timer->start();


    //Signals and Database Connectivity
    setWindowFlags(Qt::FramelessWindowHint);
    UISettings();
    move_user_login();





}

dpos::~dpos()
{
    delete ui;
}


void dpos::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        QCoreApplication::quit();
    }
    else if (event->key()==Qt::Key_End)
    {
        QCoreApplication::quit();
    }
    else if (event->key()==Qt::Key_F1)
    {
        move_home();
    }
    else
        QWidget::keyPressEvent(event);

}

void dpos::UISettings()
{

    //Collection Menu

    connect(ui->toolButton_searchID,SIGNAL(clicked()),SLOT(check()));
    connect(ui->lineEdit_farmerID,SIGNAL(returnPressed()),SLOT(check()));
    connect(ui->lineEdit_farmerID,SIGNAL(returnPressed()),ui->lineEdit_farmer_name,SLOT(setFocus()));
    connect(ui->toolButton_searchID,SIGNAL(clicked()),SLOT(analyzer_test_start()));
    connect(ui->lineEdit_farmerID,SIGNAL(returnPressed()),SLOT(analyzer_test_start()));
    connect(ui->pushButton_save,SIGNAL(clicked()),SLOT(save_report()));
    connect(ui->pushButton_clear,SIGNAL(clicked()),SLOT(clear_ui()));
    connect(ui->lineEdit_amount,SIGNAL(returnPressed()),SLOT(save_report()));
    connect(ui->toolButton_procurement_back,SIGNAL(clicked()),SLOT(move_home()));

    //main menu

    connect(ui->pushButton_procurement,SIGNAL(clicked()),SLOT(move_procurement()));
    connect(ui->pushButton_sales,SIGNAL(clicked()),SLOT(move_sales()));
    connect(ui->pushButton_report,SIGNAL(clicked()),SLOT(move_report()));
    connect(ui->pushButton_admin,SIGNAL(clicked()),this,SLOT(move_admin_login()));


    //login_menu

    connect(ui->pushButton_user_login,SIGNAL(clicked()),SLOT(user_login()));
    connect(ui->lineEdit_user_pswrd,SIGNAL(returnPressed()),SLOT(user_login()));


    //agent_menu

    connect(ui->center_name_combo_,SIGNAL(activated(int)),SLOT(enable_button()));
    connect(ui->save_agent_button,SIGNAL(clicked(bool)),SLOT(save_agent()));


    //report print menu

    connect(ui->pushButton_print,SIGNAL(clicked()),SLOT(report_print()));
    connect(ui->toolButton_report_back,SIGNAL(clicked()),SLOT(move_home()));


    //Sales Menu

    connect(ui->sales_combo,SIGNAL(activated(int)),SLOT(sales_check()));
    connect(ui->pushButton_sales_print,SIGNAL(clicked()),SLOT(sales_print()));
    connect(ui->lineEdit_sales_qty,SIGNAL(returnPressed()),SLOT(sales_calculation()));
    connect(ui->lineEdit_sales_amount,SIGNAL(returnPressed()),SLOT(sales_print()));
    connect(ui->toolButton_sales_back,SIGNAL(clicked()),SLOT(move_home()));


    //admin login

    connect(ui->pushButton_admin_login,SIGNAL(clicked()),this,SLOT(move_config_analyzer()));
    connect(ui->pushButton_config_analyzer,SIGNAL(clicked()),this,SLOT(move_config_weighscale()));
    connect(ui->pushButton_config_weigh,SIGNAL(clicked()),this,SLOT(move_procurement_rate()));
    connect(ui->pushButton_pro_rate1,SIGNAL(clicked()),this,SLOT(move_procurement_rate2()));
    connect(ui->pushButton_pro_rate2,SIGNAL(clicked()),this,SLOT(move_home()));



    //Data Sync

    connect(ui->pushButton_data_sync,SIGNAL(clicked()),this,SLOT(datasync()));


}



void dpos::clock()
{
    QDateTime time = QDateTime::currentDateTime();
    QString flow_time = time.toString("hh:mm:ss AP");
    QString flow_date = time.toString("dd-MMM-yyyy [ddd]");
    ui->label_date->setText(flow_date);
    ui->label_time->setText(flow_time);
    ui->label_sales_shift->setText(flow_time);
    ui->label_sales_date->setText(flow_date);
}



void dpos::datasync()
{
    QProcess process2;
    process2.startDetached("/bin/sh", QStringList()<< "/root/sdcard.sh");

    QProcess process3;
    process3.startDetached("/bin/sh", QStringList()<< "/home/weirdo/test.sh");

    QMessageBox *msgd=new QMessageBox(this);
    msgd->setText ("<font size = 10 color = red >Data Sync Successful </font>");
    msgd->exec();

    ui->stackedWidget->setCurrentIndex(9);


}



void dpos::move_user_login()  //USER LOGIN Page Configuration
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit_username->setFocusPolicy(Qt::StrongFocus);
    ui->lineEdit_username->setFocus();
    ui->lineEdit_username->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_user_pswrd->QLineEdit::setAlignment(Qt::AlignCenter);
    connect(ui->lineEdit_username,SIGNAL(returnPressed()),ui->lineEdit_user_pswrd,SLOT(setFocus()));
}



void dpos::user_login()   // USER Login Database Validation
{

    if(db_master_access.db_connect())
    {
        QSqlQuery qry,qry1;
        QString username,password =QString("%1").arg(QString(QCryptographicHash::hash(ui->lineEdit_user_pswrd->text().toUtf8(),QCryptographicHash::Md5).toHex()));;
        username = ui->lineEdit_username->text();

        qry.prepare("SELECT user_id FROM `users` where user_id='"+username+"' AND login_pwd='"+password+"'");
        if(qry.exec())
        {
            if(qry.next())
            {
                user_id=qry.value(0).toString();

                qry1.prepare("SELECT `dcs_id` FROM `users` WHERE 1");
                if(qry1.exec()){
                    if(qry1.next()){
                        dcs_id=qry1.value(0).toString();

                    }
                }

                db_master_access.db_disconnect();
                move_agent_selection();

            }
            else
            {
                ui->label_status->setText(QString("Invalid Credentials !!!"));
                ui->lineEdit_username->setText("");
                ui->lineEdit_user_pswrd->setText("");
                ui->lineEdit_username->setFocus();

            }

        }else
            QMessageBox::warning(0, QObject::tr("DiaryPOS"),qry.lastError().text());

    }

    db_master_access.db_disconnect();

}



void dpos::move_agent_selection()  //AGENT Selection WINDOW CONFIG
{

    ui->stackedWidget->setCurrentIndex(1);
    db_master_access.db_connect();


    QSqlQueryModel *model=new QSqlQueryModel();
    QSqlQuery qry,qry2,qry3,qry4;
    qry.prepare("select name FROM agent");
    qry.exec();
    model->setQuery(qry);
    ui->agent_combo->setModel(model);
    ui->agent_combo->setCurrentIndex(-1);
    connect(ui->agent_combo,SIGNAL(currentIndexChanged(QString)),SLOT(agent_name_display()));
    db_master_access.db_disconnect();

}



void dpos::agent_name_display()  //Fetching Agent Name & ID
{

    db_master_access.db_connect();
    ui->save_agent_button->setEnabled(false);

    agent_Name = ui->agent_combo->currentText();
    QSqlQuery qry;
    qry.prepare("select ca_id from agent WHERE name='"+agent_Name+"'");
    if(qry.exec())
    {
        while (qry.next())
        {
            agent_id=qry.value(0).toInt();
        }
    }
    connect(ui->agent_combo,SIGNAL(activated(QString)),SLOT(center_name_display()));
    db_master_access.db_disconnect();
}



void dpos::center_name_display()  //Fetching Center Name & ID
{

    db_master_access.db_connect();
    QSqlQueryModel *model=new QSqlQueryModel();
    QSqlQuery qry;
    qry.prepare("select center_name from center");
    qry.exec();
    model->setQuery(qry);
    ui->center_name_combo_->setModel(model);
    ui->center_name_combo_->setCurrentIndex(-1);

    db_master_access.db_disconnect();

}


void dpos::enable_button()
{
    ui->save_agent_button->setEnabled(true);
    db_master_access.db_connect();
    center_name = ui->center_name_combo_->currentText();
    QSqlQuery qry;
    qry.prepare("select cc_id FROM center WHERE center_name='"+center_name+"'");
    if(qry.exec())
    {
        while (qry.next())
        {
            center_id = qry.value(0).toInt();
            qDebug()<<"DCS_ID is:"<<dcs_id;
            qDebug()<<"USER_ID is:"<<user_id;
            qDebug()<<"Center ID"<<center_id;
            qDebug()<<"Center Name"<<center_name;
            qDebug()<<"Agent ID"<<agent_id;
            qDebug()<<"Agent Name"<<agent_Name;
        }
    }
    db_master_access.db_disconnect();

}



bool dpos::save_agent()   //Saving Master Details
{
    db_master_access.db_connect();
    QSqlQuery query,qry2,qry4;
    // GENERATING MCID


    qry4.prepare("SELECT mc_id FROM milk_master WHERE cc_id='"+QString::number(center_id)+"' AND ca_id='"+QString::number(agent_id)+"'");
    if(qry4.exec()) // CHECKING FOR EXISTING MC_ID
    {
        if(qry4.next())
        {
            MCID = qry4.value(0).toInt();
        }
        else // CREATING NEW MC_ID
        {
            qry2.prepare("SELECT MAX(mc_id) as mc_id from milk_master");
            if(qry2.exec())
            {
                while (qry2.next())
                {
                    if(!NULL)
                    {
                        MCID= qry2.value(0).toInt();
                        MCID=MCID+1;
                    }
                    else //CREATING NEW MC_ID
                    {
                        MCID=1;
                    }}qry2.clear();
            }}}

    mc_id = QString::number(MCID);
    qDebug()<<"MC_ID is "<<MCID;

    QDateTime agent_date=QDateTime::currentDateTime();
    QString login_date=agent_date.toString("yyyy-MM-dd");
    QString login_time=agent_date.toString("AP");


    query.exec("INSERT INTO milk_master(dcs_id,mc_id,cc_id,ca_id,mc_date,mc_time,shift_id,user_id,col_total,local_sales,union_sales,excess,shortage,fin_year_id,sync) VALUES('"+dcs_id+"','"+mc_id+"','"+QString::number(center_id)+"','"+QString::number(agent_id)+"','"+login_date+"','"+login_time+"',2,'"+user_id+"',100.00,0,100.00,0,0,22,'N')");
    if(true)
        qDebug()<<"Master Data Saved Successfully";
    else
        QMessageBox::warning(0, QObject::tr("DiaryPOS"),query.lastError().text());

    move_home();
    db_master_access.db_disconnect();
}



void dpos::move_home() //Home Navigation
{
    ui->stackedWidget->setCurrentIndex(2);

}


void dpos::move_procurement() //PROCUREMENT NAVIGATION
{
    ui->stackedWidget->setCurrentIndex(8);
}



int dpos::check() // Validating Producer
{
    db_master_access.db_connect();
    QDateTime dtime=QDateTime::currentDateTime();
    proc_date=dtime.toString("yyyy-MM-dd");
    proc_time=dtime.toString("AP");

    QSqlQuery query1;
    ID= ui->lineEdit_farmerID->text();
    query1.prepare("SELECT pro_name FROM producers WHERE producer_id="+ID);
    if(query1.exec())
    {
        while (query1.next())
        {
            qDebug()<<"Validating User Data";
            qDebug()<<"MCID"<<MCID;
            ui->lineEdit_farmer_name->setText((query1.value(0).toString()));
        }
    }

    db_master_access.db_disconnect();
}



void dpos::analyzer_test_start() // Initializing thread
{
    QThread* thread = new QThread;
    Analyzer_thread* analyzer_thread = new Analyzer_thread();
    analyzer_thread->moveToThread(thread);
    connect(analyzer_thread, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(analyzer_thread, SIGNAL(valueChanged(QString)), this,SLOT(gettext(QString)));
    connect(thread, SIGNAL(started()), analyzer_thread, SLOT(doWork()));
    connect(analyzer_thread, SIGNAL(finished()), thread, SLOT(quit()));
    connect(analyzer_thread, SIGNAL(finished()), analyzer_thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(analyzer_thread, SIGNAL(finished()),this,SLOT(set_analyzer_ui()));
    connect(analyzer_thread, SIGNAL(finished()), this, SLOT(start_WS_test()));
    analyzer_thread->requestWork();
    thread->start();
}



void dpos::gettext(QString output)  // PASS DATA BETWEEN THREAD AND MAINWINDOW
{
    //Receive data from thread
    data = output;
    qDebug()<<"Received signal from thread with data : "<<data;
}



void dpos::errorString(QString string) //PASSING ERROR FROM THREAD
{
    qDebug() << "The error from the threaded process is: " << string;
}



void dpos::set_analyzer_ui() //PASSING DATA TO PROCUREMENT UI FROM THREAD OUTPUT
{
    string my_str = data.toStdString();
    vector<string> result;
    stringstream s_stream(my_str); //create string stream from the string

    while(s_stream.good())
    {
        string substr;
        getline(s_stream, substr, ' '); //get first string delimited by comma
        result.push_back(substr);
    }

    string fat_raw = result.at(0);       //Fetching fat from analyzer
    string fat_cut = fat_raw.substr(1);         //Removing #from string
    double fatf = QString::fromStdString(fat_cut).toDouble();  // converting normal string to double

    round_fat=QString::number(fatf,'g',3).toDouble();
    fat = QString::number(round_fat);
    qDebug()<<"roundfat : "<<round_fat;

    double snff = QString::fromStdString(result.at(1)).toDouble();    //Fetching SNF as double from analyzer
    round_snf=QString::number(snff,'g',3).toDouble();
    qDebug()<<"roundsnf: "<<round_snf;
    snf = QString::number(round_snf);

    double fat_rate = 211.46;
    double snf_rate = 258.46;

    clrf = (4*snff) - (4*((0.2*fatf)+0.5)); // clr calculation
    round_clr= QString::number(clrf,'g',1).toDouble();
    clr = QString::number(round_clr);       // Converting double clrf to Qstring

    ratef = (((snff * snf_rate) + (fatf * fat_rate))/100); // rate calculation
    round_rate =QString::number(ratef,'g',4).toDouble();
    rate = QString::number(round_rate);         // Converting double ratef to Qstring

    qDebug()<<"rate"<<QString::number(round_rate);



    ui->lineEdit_farmerID->QLineEdit::setAlignment(Qt::AlignCenter); //aligning center
    ui->lineEdit_fat->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_snf->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_clr->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_quantity->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_rate->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_amount->QLineEdit::setAlignment(Qt::AlignCenter);


    ui->lineEdit_fat->setText(QString(fat)); //displaying data
    ui->lineEdit_snf->setText(QString(snf));
    ui->lineEdit_clr->setText(QString(clr));
    ui->lineEdit_rate->setText(QString(rate));

    qDebug()<<"Analyzer test completed";

    QTimer::singleShot(0, ui->lineEdit_amount, SLOT(setFocus()));

}



void dpos::clear_ui() // CLEARING PROCUREMENT UI
{


    ui->lineEdit_farmerID->setText("");
    ui->lineEdit_farmer_name->setText("");
    ui->lineEdit_fat->setText("");
    ui->lineEdit_snf->setText("");
    ui->lineEdit_clr->setText("");
    ui->lineEdit_quantity->setText("");
    ui->lineEdit_rate->setText("");
    ui->lineEdit_amount->setText("");
    ui->lineEdit_farmerID->setFocusPolicy(Qt::StrongFocus);
    ui->lineEdit_farmerID->setFocus();


}



int dpos::start_WS_test() // INITIALIZE WEIGHING SCALE CALCULATION
{
    qDebug()<<"Quantity Analysis Started";
    if(open_WS()==0)
    {
        int q=0;
        read_data_ws();
        for(q=0;q<11;q++)
        {
            quantity_string = quantity_string + weight_array[q];
        }
        string quantity_trim = quantity_string.substr(3,11);

        double weight= QString::fromStdString(quantity_trim).toDouble();
        qDebug()<<"Data"<<QString::number(weight);

        round_quantity=QString::number(weight,'g',2).toDouble();
        qDebug()<<"Data is "<<QString::number(round_quantity);

        quantitys = QString::number(round_quantity);

        qDebug()<<"rate new"<<QString::number(round_rate);

        amountf = round_quantity*round_rate;         // amount calculation
        qDebug()<<QString::number(amountf);

        round_amount = QString::number(amountf,'g',4).toDouble();
        qDebug()<<"round amount"<<QString::number(round_amount);
        amount = QString::number(round_amount);         // Converting double amountf to Qstring

        ui->lineEdit_quantity->setText(quantitys);
        ui->lineEdit_amount->setText(QString(amount));

        qDebug()<<"Quantity Captured !";
        emit ui_filled();
        return 0;
    }
    else {
        qDebug()<<"Port not open";
    }

}



bool dpos::save_report() // Saving PROCUREMENT Details
{
    db_master_access.db_connect();
    QSqlQuery query,qry1;
    query.prepare("INSERT INTO milk_detail(dcs_id,mc_id,producer_id,qty,clr,fat,snf,rate,amount,incentive,qty_mode,clr_mode,fat_mode,snf_mode,dev_SNF,source_id,source_mode,user_id) VALUES('"+dcs_id+"','"+mc_id+"','"+ID+"','"+quantitys+"','"+clr+"','"+fat+"','"+snf+"','"+rate+"','"+amount+"',0,'A','A','A','A',0,2,'S','"+user_id+"')"); //'"+proc_date+"','"+proc_time+"',

    if(query.exec())
    {

        QString row_id=query.lastInsertId().toString();
        qry1.prepare("SELECT m_producers.pro_name,procurement.* FROM procurement,m_producers WHERE m_producers.pro_id = procurement.producer_id AND procurement.row_id="+row_id);

        if (qry1.exec())
        {
            while (qry1.next())
            {


                raw_pro_db_name=qry1.value(0).toString();
                pro_db_id=qry1.value(11).toString();
                pro_db_fat=qry1.value(15).toString();
                pro_db_snf=qry1.value(16).toString();
                pro_db_clr=qry1.value(14).toString();
                pro_db_quantity=qry1.value(13).toString();
                pro_db_rate=qry1.value(17).toString();
                pro_db_amount=qry1.value(18).toString();
                pro_db_date=dateStr;
                pro_db_time=timeStr;

            }

        }

        QMessageBox msgBox;
        msgBox.setText("Details Saved Successfully");
        msgBox.setInformativeText("Continue?");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        //result_print();
        return true;
        db_master_access.db_disconnect();

    }
    else
    {
        db_master_access.db_disconnect();
        return false;
    }

}



//void dpos::result_print()
//{

//    QByteArray utf8BAString4 = timeStr.toUtf8();
//    QByteArray utf8BAString3 = dateStr.toUtf8();
//    char * dt = utf8BAString3.data();
//    char * tm = utf8BAString4.data();


//    int ret = prn_open();
//    switch(ret)
//    {
//    case 1:
//        qDebug("prn_open: SUCCESS ");
//        break;
//    default:
//        qDebug("prn_open: FAILURE ",ret);
//    }

//    if(prn_paperstatus()!=0)
//    {

//        qDebug("No paper !");

//        QMessageBox *msg1=new QMessageBox(this);
//        msg1->setText ("<font size = 10 color = yellow >No Paper! </font>");
//        msg1->exec();


//    }

//    std::string pro_name = raw_pro_db_name.toUtf8().constData();
//    const char* final_name = pro_name.c_str();;
//    std::string pro_ID = pro_db_id.toUtf8().constData();
//    const char* final_ID = pro_ID.c_str();
//    std::string pro_snf = pro_db_snf.toUtf8().constData();
//    const char* final_snf = pro_snf.c_str();
//    std::string pro_fat = pro_db_fat.toUtf8().constData();
//    const char* final_fat = pro_fat.c_str();
//    std::string pro_clr = pro_db_clr.toUtf8().constData();
//    const char* final_clr = pro_clr.c_str();
//    std::string pro_quantity = pro_db_quantity.toUtf8().constData();
//    const char* final_quantity = pro_quantity.c_str();
//    std::string pro_rate = pro_db_rate.toUtf8().constData();
//    const char* final_rate = pro_rate.c_str();
//    std::string pro_amount = pro_db_amount.toUtf8().constData();
//    const char* final_amount = pro_amount.c_str();

//    char print_select[300];

//    memset(print_select,0x00,300);
//    sprintf(print_select,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s","Producer_ID : ",final_ID,"\n\n","Producer_name : ",final_name,"\n\n","SNF : ",final_snf,"\n\n","FAT : ",final_fat,"\n\n","CLR : ",final_clr,"\n\n","Quantity : ",final_quantity," Ltrs\n\n","Rate : Rs. ",final_rate,"\n\n","Amount : Rs.",final_amount,"\n\n");
//    prn_write_text((unsigned char*)"****************************************",40,1);
//    prn_write_text((unsigned char*)" ** ABC SOCIETY ** \n\n",40,2);
//    prn_write_text((unsigned char*)"\n   ** Bill ** \n\n\n",40,2);

//    prn_write_text((unsigned char*)print_select,300,2);
//    prn_write_text((unsigned char*)"\n\n",20,2);


//    prn_write_text((unsigned char*)dt,strlen(dt),1);

//    prn_write_text((unsigned char*)tm,strlen(dt),1);

//    prn_write_text((unsigned char*)"****************************************",40,1);

//    prn_paper_feed(5);


//    prn_close();


//    ui->lineEdit_farmerID->setText("");
//    ui->lineEdit_farmer_name->setText("");
//    ui->lineEdit_fat->setText("");
//    ui->lineEdit_snf->setText("");
//    ui->lineEdit_clr->setText("");
//    ui->lineEdit_quantity->setText("");
//    ui->lineEdit_rate->setText("");
//    ui->lineEdit_amount->setText("");
//    ui->lineEdit_farmerID->setFocusPolicy(Qt::StrongFocus);
//    ui->lineEdit_farmerID->setFocus();

//}


void dpos::move_report()
{
    ui->stackedWidget->setCurrentIndex(11);
}



//void dpos::report_print()
//{
//    QByteArray utf8BAString4 = timeStr.toUtf8();
//    QByteArray utf8BAString3 = dateStr.toUtf8();
//    char * dt = utf8BAString3.data();
//    char * tm = utf8BAString4.data();


//    int ret = prn_open();
//    switch(ret)
//    {
//    case 1:
//        qDebug("prn_open: SUCCESS ");
//        break;
//    default:
//        qDebug("prn_open: FAILURE ",ret);
//    }

//    if(prn_paperstatus()!=0)
//    {

//        qDebug("No paper !");

//        QMessageBox *msg1=new QMessageBox(this);
//        msg1->setText ("<font size = 10 color = yellow >No Paper! </font>");
//        msg1->exec();


//    }

//    prn_write_text((unsigned char*)"*****************************************\n\n",40,1);
//    prn_write_text((unsigned char*)"  ABC MILK SOCIETY  ",40,2);
//    prn_write_text((unsigned char*)"-----------------------------------------\n\n",40,1);
//    prn_write_text((unsigned char*)"          **   PH:1234567890   ** \n\n",40,1);
//    prn_write_text((unsigned char*)"COLLECTION STATEMENT ",40,2);
//    prn_write_text((unsigned char*)"------------------------------------------ \n\n",40,1);
//    prn_write_text((unsigned char*)"#ID   NAME        QTY   SNF   FAT   AMT \n",40,1);
//    prn_write_text((unsigned char*)"---  ------      ----- ----- ----- ------ \n\n\n",40,1);



//    report_date = ui->dateEdit_report->date().toString("yyyy-MM-dd");

//    if(ui->radioButton_AM->isChecked())
//    {
//        report_shift = "AM";
//    }

//    else if(ui->radioButton_PM->isChecked())
//    {
//        report_shift = "PM";
//    }

//    qDebug()<<"Date is : "<<report_date;
//    qDebug()<<"Shift is :"<<report_shift;

//    QSqlQuery qry1,qry2,qry3;
//    qry1.prepare("SELECT A.producer_id,B.pro_name,A.qty,A.fat,A.snf,A.amount FROM procurement A, m_producers B WHERE A.producer_id=B.pro_id AND A.mc_date='"+report_date+"' AND mc_time='"+report_shift+"'");

//    if (qry1.exec())
//    {
//        while (qry1.next())
//        {


//            raw_pro_db_name=qry1.value(1).toString();
//            QString pro_db_name = raw_pro_db_name.left(9);

//            pro_db_id=qry1.value(0).toString();
//            pro_db_fat=qry1.value(3).toString();
//            pro_db_snf=qry1.value(4).toString();
//            pro_db_quantity=qry1.value(2).toString();
//            pro_db_amount=qry1.value(5).toString();





//            qDebug()<<qry1.value(0).toString();
//            qDebug()<<qry1.value(1).toString();
//            qDebug()<<qry1.value(2).toString();
//            qDebug()<<qry1.value(4).toString();
//            qDebug()<<qry1.value(3).toString();
//            qDebug()<<qry1.value(5).toString();



//            std::string pro_name = pro_db_name.toUtf8().constData();
//            const char* final_name = pro_name.c_str();;
//            std::string pro_ID = pro_db_id.toUtf8().constData();
//            const char* final_ID = pro_ID.c_str();
//            std::string pro_snf = pro_db_snf.toUtf8().constData();
//            const char* final_snf = pro_snf.c_str();
//            std::string pro_fat = pro_db_fat.toUtf8().constData();
//            const char* final_fat = pro_fat.c_str();
//            std::string pro_quantity = pro_db_quantity.toUtf8().constData();
//            const char* final_quantity = pro_quantity.c_str();
//            std::string pro_amount = pro_db_amount.toUtf8().constData();
//            const char* final_amount = pro_amount.c_str();


//            char print_reading[300];
//            memset(print_reading,0x00,300);
//            sprintf(print_reading,"%s%s%s%s%s%s%s%s%s%s%s%s",final_ID,"   ",final_name,"   ",final_quantity,"   ",final_snf,"  ",final_fat,"  ",final_amount,"\n\n");
//            prn_write_text((unsigned char*)print_reading,300,1);


//        }

//    }
//    qry2.prepare("SELECT SUM(qty) FROM procurement WHERE mc_date='"+report_date+"' AND mc_time='"+report_shift+"'");
//    if(qry2.exec())
//    {
//        while (qry2.next())
//        {
//            qry3.prepare("SELECT SUM(amount) FROM procurement WHERE mc_date='"+report_date+"' AND mc_time='"+report_shift+"'");

//            if(qry3.exec())
//            {
//                while (qry3.next())
//                {
//                    total_qty=qry2.value(0).toString();
//                    QString total_qty_cut = total_qty.left(7);
//                    total_amnt=qry3.value(0).toString();
//                    QString total_amt_cut = total_amnt.left(8);


//                    std::string pro_total_qty = total_qty_cut.toUtf8().constData();
//                    const char* final_total_qty = pro_total_qty.c_str();
//                    std::string pro_total_amnt = total_amt_cut.toUtf8().constData();
//                    const char* final_total_amnt = pro_total_amnt.c_str();


//                    char print_total[100];
//                    memset(print_total,0x00,300);
//                    sprintf(print_total,"%s%s%s%s%s%s","Total Qty: ",final_total_qty," Ltr","\n","Value: Rs. ",final_total_amnt,"\n");
//                    prn_write_text((unsigned char*)"----------------------------------------",40,1);

//                    prn_write_text((unsigned char*)print_total,100,2);
//                    prn_paper_feed(2);

//                    prn_write_text((unsigned char*)dt,strlen(dt),1);
//                    prn_write_text((unsigned char*)tm,strlen(dt),1);
//                    prn_write_text((unsigned char*)"****************************************",40,1);
//                    prn_paper_feed(5);
//                    prn_close();

//                }



//            }
//        }



//    }


//}


void dpos::move_sales()
{
    ui->stackedWidget->setCurrentIndex(10);
    ui->lineEdit_sales_rate->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_sales_qty->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_sales_amount->QLineEdit::setAlignment(Qt::AlignCenter);
    db_master_access.db_connect();

    ui->sales_combo->setFocusPolicy(Qt::StrongFocus);
    QSqlQueryModel *model=new QSqlQueryModel();
    QSqlQuery qry;
    qry.prepare("SELECT name FROM customer");
    qry.exec();
    model->setQuery(qry);
    ui->sales_combo->setModel(model);
    ui->sales_combo->setCurrentIndex(-1);

    db_master_access.db_disconnect();

}



bool dpos::sales_check()
{
    db_master_access.db_connect();

    sales_name=ui->sales_combo->currentText();
    qDebug()<<"Customer name is "<<sales_name;

    QSqlQuery qry,query,qry1,query1;
    query1.prepare("SELECT cust_id FROM customer WHERE name='"+sales_name+"'");
    if(query1.exec())
    {
        while (query1.next())
        {

            sales_id = query1.value(0).toString();
            qDebug()<<"Sales ID : "<<sales_id;

        }
    }
QSqlQueryModel *model=new QSqlQueryModel();
    qry.prepare("SELECT item_id FROM milk_sale_rate WHERE cust_id='"+sales_id+"'");
    if(qry.exec())
    {
        while (qry.next())
        {
            customer_item__type=qry.value(0).toString();
            qDebug()<<"Customer type="<<customer_item__type;

            query.prepare("SELECT item_name FROM milk_sale_item WHERE item_id='"+customer_item__type+"'");
            if(query.exec())
            {
                while (query.next())
                {
                    model->setQuery(query);


                }ui->sales_type_combo->setModel(model);
            }



        }

        //ui->sales_type_combo->setCurrentIndex(-1);

    }

    qry1.prepare("SELECT rate FROM milk_sale_rate WHERE cust_id='"+sales_id+"'AND item_id='"+customer_item__type+"'");
    if(qry1.exec())
    {
        while (qry1.next())
        {
            sale_rate=qry1.value(0).toDouble();
            rate_string=QString::number(sale_rate);
            ui->lineEdit_sales_rate->setText(rate_string);
        }
    }



    QTimer::singleShot(0, ui->lineEdit_sales_qty, SLOT(setFocus()));

}

void dpos::sales_calculation()
{
    db_master_access.db_connect();
    QSqlQuery qry;
    sales_qty_string = ui->lineEdit_sales_qty->text();
    sales_qty = sales_qty_string.toDouble();
    sale_qty = QString::number(sales_qty);

    QTimer::singleShot(0, ui->lineEdit_sales_amount, SLOT(setFocus()));

    sales_amount = sale_rate*sales_qty;
    sale_amount = QString::number(sales_amount);

    ui->lineEdit_sales_amount->setText(QString::number(sales_amount));

    qry.prepare("INSERT INTO milk_sales(dcs_id,mc_id,slno,cust_id,item_id,qty,rate,amount,user_id)VALUES('"+dcs_id+"','"+mc_id+"','1','"+sales_id+"','"+sale_type+"','"+sale_qty+"','"+rate_string+"','"+sale_amount+"','"+user_id+"')");
    qry.exec();
}

//void dpos::sales_print()
//{

//    QString sales_name_cut = sales_name.left(15);
//    qDebug()<<"Butchered Name : "<<sales_name_cut;




//    QByteArray utf8BAString4 = timeStr.toUtf8();
//    QByteArray utf8BAString3 = dateStr.toUtf8();
//    char * dt = utf8BAString3.data();
//    char * tm = utf8BAString4.data();


//    int ret = prn_open();
//    switch(ret)
//    {
//    case 1:
//        qDebug("prn_open: SUCCESS ");
//        break;
//    default:
//        qDebug("prn_open: FAILURE ",ret);
//    }

//    if(prn_paperstatus()!=0)
//    {

//        qDebug("No paper !");

//        QMessageBox *msg1=new QMessageBox(this);
//        msg1->setText ("<font size = 10 color = yellow >No Paper! </font>");
//        msg1->exec();


//    }

//    prn_write_text((unsigned char*)"*****************************************\n\n",40,1);
//    prn_write_text((unsigned char*)"  ABC MILK SOCIETY  ",40,2);
//    prn_write_text((unsigned char*)"-----------------------------------------\n\n",40,1);
//    prn_write_text((unsigned char*)"         **   PH:1234567890   ** \n\n",40,1);
//    prn_write_text((unsigned char*)"  SALES STATEMENT ",40,2);
//    prn_write_text((unsigned char*)"------------------------------------------ \n\n",40,1);
//    prn_write_text((unsigned char*)"#ID    NAME               QTY        AMT \n",40,1);
//    prn_write_text((unsigned char*)"---   ------             ----       ---- \n\n\n",40,1);



//    std::string str_sales_name = sales_name_cut.toUtf8().constData();
//    const char* final_sales_name = str_sales_name.c_str();;
//    std::string str_sales_ID = sales_id.toUtf8().constData();
//    const char* final_sales_ID = str_sales_ID.c_str();
//    std::string str_sales_qty = sale_qty.toUtf8().constData();
//    const char* final_sales_qty = str_sales_qty.c_str();
//    std::string str_sales_amount = sale_amount.toUtf8().constData();
//    const char* final_sales_amount = str_sales_amount.c_str();





//    char print_sales[300];
//    memset(print_sales,0x00,300);
//    sprintf(print_sales,"%s%s%s%s%s%s%s%s%s%s%s%s","Customer_ID : ",final_sales_ID,"\n\n","Customer_name : ",final_sales_name,"\n\n","Quantity : ",final_sales_qty," Ltrs\n\n","Amount : Rs.",final_sales_amount,"\n\n\n");
//    prn_write_text((unsigned char*)print_sales,300,2);


//    prn_write_text((unsigned char*)dt,strlen(dt),1);
//    prn_write_text((unsigned char*)tm,strlen(dt),1);
//    prn_write_text((unsigned char*)"****************************************",40,1);
//    prn_paper_feed(5);
//    prn_close();

//    ui->lineEdit_sales_ID->setText("");
//    ui->lineEdit_sales_qty->setText("");
//    ui->lineEdit_sales_amount->setText("");
//    ui->lineEdit_sales_ID->setFocusPolicy(Qt::StrongFocus);
//    ui->lineEdit_sales_ID->setFocus();

//}









void dpos::move_admin_login()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->lineEdit_admin_ID->QLineEdit::setAlignment(Qt::AlignCenter);
    ui->lineEdit_admin_password->QLineEdit::setAlignment(Qt::AlignCenter);
}



void dpos::move_config_analyzer()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void dpos::move_config_weighscale()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void dpos::move_procurement_rate()
{
    ui->stackedWidget->setCurrentIndex(6);
}

void dpos::move_procurement_rate2()
{
    ui->stackedWidget->setCurrentIndex(7);
}













void dpos::analyzer_parameter()
{

    QSqlQuery qry1;
    qry1.prepare("SELECT *FROM config_analyzer WHERE ana_short IN(SELECT ana_short FROM settings)");

    if (qry1.exec())
    {
        while (qry1.next())
        {
            // int baud_rate = qry1.value(3).toInt();
        }

    }
    else
        qDebug()<< "error" << qry1.lastError();

}


