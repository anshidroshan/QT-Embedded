#ifndef DPOS_H
#define DPOS_H

#include <QWidget>
#include <QMainWindow>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>
#include<QMessageBox>
#include <database.h>
#include <database_master.h>.h>
#include<QDebug>
#include<iostream>
#include<vector>
#include<sstream>
#include<QTimer>
#include <QWidget>
#include <QXmlStreamWriter>
#include<iomanip>
#include <QKeyEvent>


using namespace std;

namespace Ui {
class dpos;
}

class dpos : public QWidget
{
    Q_OBJECT

public:
    explicit dpos(QWidget *parent = 0);
    ~dpos();
    database db_detail_access;
    database_master db_master_access;


signals:

    void ui_filled();


protected:
    void keyPressEvent(QKeyEvent *event_quit);


private:
    Ui::dpos *ui;
    QTimer *timer;

    string quantity_string;
    int agent_id;
    int center_id;
    int MCID;
    QString center_name;
    QString agent_Name;
    QString mc_id,dcs_id;
    QString report_date,report_shift;
    QString user_id;
    QString quantitys;
    QString total_qty;
    QString total_amnt;
    QString data;
    QString analyzer_name;
    QString ID,quantity,clr,fat,snf,rate,amount;
    QString proc_time;
    QString proc_date;
    QString dateStr;
    QString timeStr;
    QString raw_pro_db_name;
    QString pro_db_id;
    QString pro_db_snf;
    QString pro_db_fat;
    QString pro_db_clr, pro_db_rate, pro_db_quantity, pro_db_date, pro_db_amount, pro_db_time;
    double fatf,snff,clrf,quantityf,ratef,amountf,round_fat,round_quantity,round_snf,round_clr,round_rate,round_amount;
    QString sales_id,sales_name;
    double sales_qty,sale_rate,sales_amount;
    QString sale_qty,sale_amount,sale_type,sales_qty_string,rate_string,customer_item__type;

private slots:

    void gettext(QString);

    void UISettings();

    void analyzer_test_start();

    void set_analyzer_ui();

    void user_login();

    void agent_name_display();

    void center_name_display();

    bool save_agent();

    int check();

    void clear_ui();

    void analyzer_parameter();

    int start_WS_test();

    bool save_report();

    void clock();

   // void report_print();

    //void result_print();

    void move_home();

    void move_user_login();

    void move_agent_selection();

    void move_procurement();

    void move_report();

    void move_sales();

    void move_admin_login();

    void move_config_analyzer();

    void move_config_weighscale();

    void move_procurement_rate();

    void move_procurement_rate2();

    bool sales_check();

    void sales_calculation();

    //void sales_print();

    void datasync();

    void errorString(QString string);

    void enable_button();


};

#endif // DPOS_H
