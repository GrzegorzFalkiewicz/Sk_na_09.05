#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , timer(new QTimer(this))
{
    ui->setupUi(this);
    okno_sieci = new dialog_sieciowy(this);
    okno_sieci->setAttribute(Qt::WA_DeleteOnClose, false);
    okno_sieci->setModal(false);
    ustawStatusPolaczenia("Brak połączenia", Qt::gray);
    utworzSerie();
    sym.Setup();
    connect(timer, SIGNAL(timeout()), this, SLOT(advance()));
    on_Spbox_inter_valueChanged(ui->Spbox_inter->value());
    on_ustawA_valueChanged(ui->ustawA->value());
    on_ustawP_valueChanged(ui->ustawP->value());
    on_ustawT_valueChanged(ui->ustawT->value());
    on_ustawKpid_valueChanged(ui->ustawKpid->value());
    on_ustawTi_valueChanged(ui->ustawTi->value());
    on_ustawTd_valueChanged(ui->ustawTd->value());
    sym.set_a1(-0.4);
    sym.set_a2(0.2);
    sym.set_a3(0.0);
    sym.set_b1(0.6);
    sym.set_b2(0.3);
    sym.set_b3(0.0);
    sym.set_arx_k(1);
    sig();
    edit_ARX = new Dialog_ARX;
    ustawNazwy();
    dodajSerie();
    utworzOsie();
    ui->ChartUchyb->setChart(chart1);
    ui->Chartwartosci->setChart(chart2);
    ui->ChartSterowanie->setChart(chart3);
    connect(edit_ARX, SIGNAL(accepted()), this, SLOT(Pobiezdane_ARX()));
    connect(okno_sieci, &dialog_sieciowy::sygnalY, this, &MainWindow::odebranoY);
    connect(okno_sieci, &dialog_sieciowy::sygnalU, this, &MainWindow::odebranoU);
    connect(okno_sieci, &dialog_sieciowy::odebranoKonfiguracje, this, &MainWindow::odebranoKonfiguracja);
    connect(okno_sieci, &dialog_sieciowy::rozlaczenieSieciowe, this, &MainWindow::zerwanoPolaczenie);
    connect(okno_sieci, &dialog_sieciowy::otrzymanoGotowosc, this, &MainWindow::odebranoGotowosc);
    connect(okno_sieci, &dialog_sieciowy::startSymulacjiZdalnej, this, &MainWindow::zdalnieStartuj);
    connect(okno_sieci, &dialog_sieciowy::stopSymulacjiZdalnej, this, &MainWindow::zdalnieStopuj);
    connect(okno_sieci, &dialog_sieciowy::polaczono, this, [this](bool jestSerwerem) {
        ustawTrybSieciowy(true, jestSerwerem);
        ustawStatusPolaczenia("Połączono", Qt::green);
    });

    connect(okno_sieci, &dialog_sieciowy::rozlaczono, this, [this]() {
        ustawTrybSieciowy(false, false);
        ustawStatusPolaczenia("Rozłączono", Qt::red);
    });
    connect(okno_sieci, &dialog_sieciowy::zmienGUI, this, [=](bool jestemRegulatorem) {
        ustawTrybSieciowy(true, jestemRegulatorem); // blokuj GUI natychmiast
    });
}
MainWindow::~MainWindow()
{
    delete ui;
    delete edit_ARX;
    usunSerie();
    usun_charty();
}
void MainWindow::sig()
{
    sygnal newsig;
    string text=ui->Sygnal->currentText().toStdString();
    if(text=="Pros")newsig=sygnal::syg_prost;
    else if(text=="Sinusoida")newsig=sygnal::syg_sin;
    else newsig=sygnal::sk_jed;
    sym.set_syg(newsig);
}
void MainWindow::advance()
{
    if (!sym.get_start()) return;

    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        sym.symulacja();
        okno_sieci->wyslijU(sym.get_ster());

        // ✅ Sprawdzamy "wyrabianie się" tylko gdy symulacja trwa
        if (working) {
            if (!odebranoY_w_takcie) {
                ustawStatusWyrabiania(false); // 🔴 nie dotarło na czas
            } else {
                ustawStatusWyrabiania(true);  // 🟢 dotarło OK
            }
            odebranoY_w_takcie = false; // reset na kolejną iterację
        }
    }
    else if (okno_sieci->getTryb() == TrybPracySieciowej::Brak) {
        sym.symulacja();
    }

    if (sym.get_ite() > 42.0) {
        x = (sym.get_ite() - 38.0);
        series->remove(0);
        series2->remove(0);
        series3->remove(0);
        series4->remove(0);
        series5->remove(0);
        series6->remove(0);
        series7->remove(0);
    }

    dodacDoSerii();
    resetMaksMin();
    ustawMin();
    ustawMax();
    ustawZakres();
}
void MainWindow::on_ustawA_valueChanged(double arg1)
{
    sym.set_zad(arg1);
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::on_ustawP_valueChanged(double arg1)
{
    sym.set_p(arg1);
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::on_ustawT_valueChanged(int arg1)
{
    sym.set_T(arg1);
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::on_ustawKpid_valueChanged(double arg1)
{
    sym.set_pid_k(arg1);
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::on_ustawTi_valueChanged(double arg1)
{
    sym.set_pid_Ti(arg1);
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::on_ustawTd_valueChanged(double arg1)
{
    sym.set_pid_Td(arg1);
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::dodajSerie()
{
    chart1->addSeries(series);
    chart2->addSeries(series2);
    chart2->addSeries(series3);
    chart3->addSeries(series4);
    chart3->addSeries(series5);
    chart3->addSeries(series6);
    chart3->addSeries(series7);
}
void MainWindow::usunSerie()
{
    delete series;
    delete series2;
    delete series3;
    delete series4;
    delete series5;
    delete series6;
    delete series7;
}
void MainWindow::utworzSerie()
{
    series = new QLineSeries();
    series2 = new QLineSeries();
    series3 = new QLineSeries();
    series4 = new QLineSeries();
    series5 = new QLineSeries();
    series6 = new QLineSeries();
    series7 = new QLineSeries();
}
void MainWindow::resetMaksMin()
{
    maks_y1=0.00001;
    maks_y2=0.00001;
    maks_y3=0.00001;
    min_y1=-0.00001;
    min_y2=-0.00001;
    min_y3=-0.00001;
}
void MainWindow::utworzOsie()
{
    chart1->createDefaultAxes();
    chart2->createDefaultAxes();
    chart3->createDefaultAxes();

    if (!chart1->axes(Qt::Horizontal).isEmpty())
        chart1->axes(Qt::Horizontal).first()->setTitleText("iteracja");
    if (!chart1->axes(Qt::Vertical).isEmpty())
        chart1->axes(Qt::Vertical).first()->setTitleText("Odp");

    if (!chart2->axes(Qt::Horizontal).isEmpty())
        chart2->axes(Qt::Horizontal).first()->setTitleText("iteracja");
    if (!chart2->axes(Qt::Vertical).isEmpty())
        chart2->axes(Qt::Vertical).first()->setTitleText("Odp");

    if (!chart3->axes(Qt::Horizontal).isEmpty())
        chart3->axes(Qt::Horizontal).first()->setTitleText("iteracja");
    if (!chart3->axes(Qt::Vertical).isEmpty())
        chart3->axes(Qt::Vertical).first()->setTitleText("Odp");
}
void MainWindow::ustawNazwy()
{
    series->setName("Uchyb");
    series2->setName("Zadana");
    series3->setName("Regulowana");
    series4->setName("P");
    series5->setName("I");
    series6->setName("D");
    series7->setName("sterowanie");
}
void MainWindow::ustawZakres()
{
    chart1->axes(Qt::Horizontal).first()->setRange(x,sym.get_ite());
    chart1->axes(Qt::Vertical).first()->setRange(min_y1-abs(0.2*min_y1),maks_y1*1.5);
    chart2->axes(Qt::Vertical).first()->setRange(min_y2-abs(0.2*min_y2),maks_y2*1.5);
    chart2->axes(Qt::Horizontal).first()->setRange(x,sym.get_ite());
    chart3->axes(Qt::Horizontal).first()->setRange(x,sym.get_ite());
    chart3->axes(Qt::Vertical).first()->setRange(min_y3 - abs(0.2*min_y3),maks_y3*1.5);
}
void MainWindow::dodacDoSerii()
{
    series->append(sym.get_ite(),sym.get_u());
    series2->append(sym.get_ite(),sym.get_Zad());
    series3->append(sym.get_ite(),sym.get_Y());
    series4->append(sym.get_ite(),sym.get_P());
    series5->append(sym.get_ite(),sym.get_I());
    series6->append(sym.get_ite(),sym.get_D());
    series7->append(sym.get_ite(),sym.get_ster());
}
void MainWindow::ustawMin()
{
    if(min_y1<0.0001 && min_y1>-0.0001)
        min_y1=-0.0001;
    ZakresWykresu(min_y1,series);
    ZakresWykresu(min_y2,series2);
    ZakresWykresu(min_y2,series3);
    ZakresWykresu(min_y3,series4);
    ZakresWykresu(min_y3,series5);
    ZakresWykresu(min_y3,series6);
    ZakresWykresu(min_y3,series7);
}
void MainWindow::ustawMax()
{
    if(maks_y1<0.0001 && maks_y1>-0.0001)
        maks_y1=0.0001;
    ZakresWykresu(maks_y1,series,false);
    ZakresWykresu(maks_y2,series2,false);
    ZakresWykresu(maks_y2,series3,false);
    ZakresWykresu(maks_y3,series4,false);
    ZakresWykresu(maks_y3,series5,false);
    ZakresWykresu(maks_y3,series6,false);
    ZakresWykresu(maks_y3,series7,false);
}
void MainWindow::ZakresWykresu(double &y, QLineSeries * &seria, bool czy_min)
{
    int j=min(38,sym.get_ite());
    if(czy_min)
    {
        for(int i = 0; i<j; i++)
        {y=min(seria->at(i).y(),y);}
    }
    else
    {
        for(int i = 0; i<j; i++)
        {y=max(seria->at(i).y(),y);}
    }
}
void MainWindow:: usun_charty()
{
    delete chart1;
    delete chart2;
    delete chart3;
}
void MainWindow::on_Spbox_inter_valueChanged(double arg1)
{
    timer->setInterval(arg1*1000);
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::Pobiezdane_ARX()
{
    sym.set_a1(edit_ARX->a1);
    sym.set_a2(edit_ARX->a2);
    sym.set_a3(edit_ARX->a3);
    sym.set_b1(edit_ARX->b1);
    sym.set_b2(edit_ARX->b2);
    sym.set_b3(edit_ARX->b3);
    sym.set_arx_k(edit_ARX->K);
    sym.Set_Odch(edit_ARX->odchyl);
}
void MainWindow::on_StartStop_clicked()
{
    if (okno_sieci->getTryb() == TrybPracySieciowej::Klient)
        return;
    sym.StartStop();
    working = !working;

    if (working) {
        timer->start();
        if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer)
            okno_sieci->wyslijStartStop(true);
    } else {
        timer->stop();
        if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer)
            okno_sieci->wyslijStartStop(false);
    }
}
void MainWindow::on_Reset_clicked()
{
    if(working)on_StartStop_clicked();
    if(!working)timer->stop();
    sym.Setup();
    sym.reset();
    x=0;
    usunSerie();
    utworzSerie();
    ustawNazwy();
    resetMaksMin();
    dodajSerie();
    utworzOsie();
    ui->ChartUchyb->setChart(chart1);
    ui->Chartwartosci->setChart(chart2);
    ui->ChartSterowanie->setChart(chart3);
}
void MainWindow::on_ustaw_S_valueChanged(double arg1)
{
    sym.set_stala(arg1);
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::on_Sygnal_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    sig();
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::on_chboxCalka_stateChanged()
{
    sym.set_tryb(ui->chboxCalka->isChecked());
    if (okno_sieci->getTryb() == TrybPracySieciowej::Serwer) {
        wyslijKonfiguracjeSieciowa();
    }
}
void MainWindow::on_pidReset_clicked()
{
    sym.reset();
}
void MainWindow::on_edytujARX_clicked()
{
    edit_ARX->Set_A1(sym.Get_A_ARX(0));
    edit_ARX->Set_A2(sym.Get_A_ARX(1));
    edit_ARX->Set_A3(sym.Get_A_ARX(2));
    edit_ARX->Set_B1(sym.Get_B_ARX(0));
    edit_ARX->Set_B2(sym.Get_B_ARX(1));
    edit_ARX->Set_B3(sym.Get_B_ARX(2));
    edit_ARX->Set_K(sym.Get_K_ARX());
    edit_ARX->Set_Odchyl(sym.Get_odchyl_Arx());
    edit_ARX->show();
}
void MainWindow::odebranoY(double y)
{
    sym.set_Y(y);
    odebranoY_w_takcie = true;
}
void MainWindow::odebranoU(double u)
{
    qDebug() << "ODEBRANO U:" << u;

    // 🔁 Uruchom symulację tylko przy pierwszym U
    if (!working) {
        sym.StartStop();    // wewnętrzny przełącznik symulatora
        working = true;
        timer->start();     // uruchom timer dla advance()
    }

    sym.symulujARX(u);
    double y = sym.get_Y();
    qDebug() << "WYSYŁAM Y:" << y;
    okno_sieci->wyslijY(y);
}
void MainWindow::odebranoKonfiguracja(KonfiguracjaSieciowa konf)
{
    ui->Spbox_inter->setValue(konf.interwalMs / 1000.0);
    ui->ustawKpid->setValue(konf.k);
    ui->ustawTi->setValue(konf.Ti);
    ui->ustawTd->setValue(konf.Td);
    ui->chboxCalka->setChecked(konf.trybCalki);
    ui->ustawA->setValue(konf.zadana);
    ui->ustaw_S->setValue(konf.stala);
    ui->ustawT->setValue(konf.T);
    ui->ustawP->setValue(konf.p);
    if(konf.typSygnalu == 0) ui->Sygnal->setCurrentText("Skok");
    else if(konf.typSygnalu == 1) ui->Sygnal->setCurrentText("Pros");
    else ui->Sygnal->setCurrentText("Sinusoida");
}
void MainWindow::zerwanoPolaczenie()
{
    QMessageBox::warning(this, "Sieć", "Połączenie sieciowe zostało zerwane.\nTryb lokalny aktywowany.");
}
void MainWindow::odebranoGotowosc()
{
    QMessageBox::information(this, "Sieć", "Obiekt potwierdził gotowość.");
}
void MainWindow::zdalnieStartuj()
{
    if (!working) {
        sym.StartStop();
        working = true;
        timer->start();
    }
}
void MainWindow::zdalnieStopuj()
{
    if (working) {
        sym.StartStop();
        working = false;
        timer->stop();
    }
}
void MainWindow::wyslijKonfiguracjeSieciowa()
{
    if (okno_sieci->getTryb() != TrybPracySieciowej::Serwer) {
        return;
    }
    KonfiguracjaSieciowa konf;
    konf.interwalMs = ui->Spbox_inter->value() * 1000;
    konf.k = ui->ustawKpid->value();
    konf.Ti = ui->ustawTi->value();
    konf.Td = ui->ustawTd->value();
    konf.trybCalki = ui->chboxCalka->isChecked();

    if(ui->Sygnal->currentText() == "Skok") {
        konf.typSygnalu = 0;
    } else if(ui->Sygnal->currentText() == "Pros") {
        konf.typSygnalu = 1;
    } else {
        konf.typSygnalu = 2;
    }
    konf.zadana = ui->ustawA->value();
    konf.stala = ui->ustaw_S->value();
    konf.T = ui->ustawT->value();
    konf.p = ui->ustawP->value();
    okno_sieci->wyslijKonfiguracje(konf);
}
void MainWindow::on_btnNetwork_clicked()
{
    okno_sieci->show();
    okno_sieci->activateWindow();
}
void MainWindow::ustawTrybSieciowy(bool trybSieciowy, bool jestRegulatorem)
{
    ui->frame->setEnabled(true);         // parametry sygnału
    ui->frame_2->setEnabled(true);       // PID
    ui->frame_3->setEnabled(true);       // ARX
    ui->StartStop->setEnabled(true);
    ui->Reset->setEnabled(true);
    ui->Spbox_inter->setEnabled(true);
    ui->btnNetwork->setEnabled(true);
    ui->edytujARX->setEnabled(true);

    if (trybSieciowy) {
        if (jestRegulatorem) {
            ui->frame_3->setEnabled(false);     // zablokuj ARX u regulatora
            ui->edytujARX->setEnabled(false);
        } else {
            ui->frame->setEnabled(false);       // zablokuj sygnał zadany
            ui->frame_2->setEnabled(false);     // zablokuj PID
            ui->StartStop->setEnabled(false);   // zablokuj start/stop
            ui->Reset->setEnabled(false);       // zablokuj reset
            ui->Spbox_inter->setEnabled(false); // zablokuj interwał
            ui->frame_3->setEnabled(true);      // ARX aktywny
            ui->edytujARX->setEnabled(true);    // ARX aktywny
        }
    }

    qApp->processEvents();
}

void MainWindow::ustawStatusPolaczenia(QString tekst, QColor kolor)
{
    QPalette paleta = ui->lblSiec->palette();
    paleta.setColor(QPalette::Window, kolor);
    ui->lblSiec->setAutoFillBackground(true);
    ui->lblSiec->setPalette(paleta);
    ui->lblSiec->update();
    ui->label_13->setText(tekst);
}

void MainWindow::ustawStatusWyrabiania(bool ok)
{
    QPalette paleta = ui->lampkaWyrabiania->palette();
    paleta.setColor(QPalette::Window, ok ? Qt::green : Qt::red);
    ui->lampkaWyrabiania->setAutoFillBackground(true);
    ui->lampkaWyrabiania->setPalette(paleta);
    ui->lampkaWyrabiania->update();
}
