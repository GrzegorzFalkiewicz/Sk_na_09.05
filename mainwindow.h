#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCharts>
#include "symulator.h"
#include "dialog_arx.h"
#include "dialog_sieciowy.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void advance();
    void on_ustawA_valueChanged(double arg1);
    void on_ustawP_valueChanged(double arg1);
    void on_ustawT_valueChanged(int arg1);
    void on_ustawKpid_valueChanged(double arg1);
    void on_ustawTi_valueChanged(double arg1);
    void on_ustawTd_valueChanged(double arg1);
    void on_Spbox_inter_valueChanged(double arg1);
    void on_StartStop_clicked();
    void on_Reset_clicked();
    void on_ustaw_S_valueChanged(double arg1);
    void on_Sygnal_currentTextChanged(const QString &arg1);
    void on_chboxCalka_stateChanged();
    void on_pidReset_clicked();
    void on_edytujARX_clicked();
    void Pobiezdane_ARX();
    void usunSerie();
    void odebranoGotowosc();
    void zdalnieStartuj();
    void zdalnieStopuj();
    void odebranoY(double y);
    void odebranoU(double u);
    void odebranoKonfiguracja(KonfiguracjaSieciowa konf);
    void zerwanoPolaczenie();
    void on_btnNetwork_clicked();
    void ustawTrybSieciowy(bool trybSieciowy, bool jestRegulatorem);
private:
    Ui::MainWindow *ui;
    QTimer *timer;
    dialog_sieciowy *okno_sieci;
    Symulator sym;
    Dialog_ARX *edit_ARX;
    bool working = false;
    QChart *chart1 = new QChart();
    QChart *chart2 = new QChart();
    QChart *chart3 = new QChart();
    QLineSeries *series, *series2, *series3, *series4, *series5, *series6, *series7;
    int x = 0;
    double maks_y1, maks_y2, maks_y3;
    double min_y1, min_y2, min_y3;
    void sig();
    void dodajSerie();
    void utworzSerie();
    void utworzOsie();
    void resetMaksMin();
    void dodacDoSerii();
    void ustawMin();
    void ustawMax();
    void ZakresWykresu(double &y, QLineSeries * &seria, bool czy_min = true);
    void usun_charty();
    void ustawNazwy();
    void ustawZakres();
    void wyslijKonfiguracjeSieciowa();
};
#endif // MAINWINDOW_H
