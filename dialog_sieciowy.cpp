
#include "dialog_sieciowy.h"
#include "ui_dialog_sieciowy.h"
#include <QMessageBox>
#include <QColor>
#include <QHostAddress>
#include <QDebug>

dialog_sieciowy::dialog_sieciowy(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_sieciowy)
{
    ui->setupUi(this);
    ui->btnReady->setVisible(false); // Domyślnie ukryte

    timerOczekiwania = new QTimer(this);
    timerOczekiwania->setInterval(1000);
    connect(timerOczekiwania, &QTimer::timeout, this, [=]() {
        ustawStatusPolaczenia("Brak odpowiedzi od obiektu", Qt::red);
        timerOczekiwania->stop();
    });
}

dialog_sieciowy::~dialog_sieciowy()
{
    if (gniazdo) gniazdo->disconnectFromHost();
    if (serwer) serwer->close();
    delete ui;
}
void dialog_sieciowy::nowePolaczenie()
{
    if (gniazdo) gniazdo->deleteLater();
    gniazdo = serwer->nextPendingConnection();
    connect(gniazdo, &QTcpSocket::readyRead, this, &dialog_sieciowy::odebranoDane);
    connect(gniazdo, &QTcpSocket::disconnected, this, &dialog_sieciowy::rozlaczono);
    QString ip = gniazdo->peerAddress().toString();
    ip = ip.replace("::ffff:", "");
    ustawStatusPolaczenia("Połączono z klientem: " + ip, Qt::green);
    ui->KontrolkaStatusu->setStyleSheet("background-color: green; border-radius: 8px;");
    emit polaczono(true);
}
void dialog_sieciowy::ustawStatusPolaczenia(const QString &tekst, const QColor &kolor)
{
    ui->labelStatus->setText(tekst);
    QString style = QString("background-color: %1; border-radius: 8px;")
                        .arg(kolor.name());
    ui->KontrolkaStatusu->setStyleSheet(style);
}
TrybPracySieciowej dialog_sieciowy::getTryb() const
{
    return tryb;
}
void dialog_sieciowy::ustawTryb(TrybPracySieciowej t)
{
    tryb = t;
    if (t == TrybPracySieciowej::Brak) {
        ustawStatusPolaczenia("Brak połączenia", Qt::gray);
    } else if (t == TrybPracySieciowej::Serwer) {
        ustawStatusPolaczenia("Serwer aktywny", QColor(255, 165, 0));
    } else if (t == TrybPracySieciowej::Klient) {
        ustawStatusPolaczenia("Klient aktywny", QColor(0, 255, 0));
    }
}
void dialog_sieciowy::on_btnPolacz_clicked()
{
    if (gniazdo && gniazdo->state() == QAbstractSocket::ConnectedState) {
        przejdzWLokalnyTryb();
        ui->btnPolacz->setText("Połącz");
        return;
    }

    int port = ui->linePort->text().toInt();
    if (port < 1024 || port > 49151) {
        QMessageBox::warning(this, "Błąd", "Port musi być w zakresie 1024-49151");
        return;
    }

    if (ui->radioSerwer->isChecked()) {
        ustawTryb(TrybPracySieciowej::Serwer);
        emit zmienGUI(true);  // Tylko zmiana GUI

        serwer = new QTcpServer(this);
        if (!serwer->listen(QHostAddress::Any, port)) {
            QMessageBox::critical(this, "Błąd", "Nie można uruchomić serwera!");
            ustawTryb(TrybPracySieciowej::Brak);
            return;
        }
        connect(serwer, &QTcpServer::newConnection, this, &dialog_sieciowy::nowePolaczenie);
        ui->btnPolacz->setText("Rozłącz");
    } else {
        ustawTryb(TrybPracySieciowej::Klient);
        emit zmienGUI(false);  // Tylko zmiana GUI

        gniazdo = new QTcpSocket(this);
        connect(gniazdo, &QTcpSocket::connected, this, [=]() {
            ui->btnPolacz->setText("Rozłącz");
            ui->btnReady->setVisible(true);
            ustawStatusPolaczenia("Połączono z serwerem", Qt::green);
            emit polaczono(false); // TUTAJ dopiero prawdziwe połączenie
        });
        connect(gniazdo, &QTcpSocket::readyRead, this, &dialog_sieciowy::odebranoDane);
        connect(gniazdo, &QTcpSocket::disconnected, this, &dialog_sieciowy::rozlaczono);
        gniazdo->connectToHost(ui->lineIP->text(), port);
    }
}
void dialog_sieciowy::odebranoDane()
{
    QDataStream in(gniazdo);
    in.setVersion(QDataStream::Qt_6_6);
    przetworzWiadomosc(in);
}
void dialog_sieciowy::przetworzWiadomosc(QDataStream &in)
{
    quint8 typ;
    in >> typ;

    switch (typ) {
    case MessageType::ControlSignal: {
        double u;
        in >> u;
        emit sygnalU(u);
        break;
    }
    case MessageType::RegulatedValue: {
        double y;
        in >> y;
        emit sygnalY(y);
        timerOczekiwania->stop();
        ustawStatusPolaczenia("Synchronizacja OK", Qt::green);
        break;
    }
    case MessageType::Konfiguracja: {
        KonfiguracjaSieciowa konf;
        in >> konf.interwalMs >> konf.k >> konf.Ti >> konf.Td >> konf.trybCalki
            >> konf.typSygnalu >> konf.zadana >> konf.stala >> konf.T >> konf.p;
        emit odebranoKonfiguracje(konf);
        break;
    }
    case MessageType::ReadyStatus: {
        emit otrzymanoGotowosc();
        ustawStatusPolaczenia("Obiekt gotowy", Qt::green);
        break;
    }
    case MessageType::StartCommand: {
        emit startSymulacjiZdalnej();
        ustawStatusPolaczenia("Symulacja rozpoczęta", Qt::green);
        break;
    }
    case MessageType::StopCommand: {
        emit stopSymulacjiZdalnej();
        ustawStatusPolaczenia("Symulacja zatrzymana", Qt::red);
        break;
    }
    default:
        qDebug() << "Nieznany typ wiadomości:" << typ;
        break;
    }
}
void dialog_sieciowy::przejdzWLokalnyTryb()
{
    if (gniazdo) {
        gniazdo->disconnectFromHost();
        gniazdo->deleteLater();
        gniazdo = nullptr;
    }
    if (serwer) {
        serwer->close();
        serwer->deleteLater();
        serwer = nullptr;
    }
    ustawTryb(TrybPracySieciowej::Brak);
    ui->btnReady->setVisible(false);
}
void dialog_sieciowy::on_btnReady_clicked()
{
    wyslijGotowosc();
    ui->btnReady->setEnabled(false);
    ustawStatusPolaczenia("Oczekiwanie na start", Qt::yellow);
}
void dialog_sieciowy::wyslijU(double u)
{
    if (!gniazdo || gniazdo->state() != QAbstractSocket::ConnectedState) return;

    QByteArray dane;
    QDataStream out(&dane, QIODevice::WriteOnly);
    out << static_cast<quint8>(MessageType::ControlSignal) << u;
    gniazdo->write(dane);
}
void dialog_sieciowy::wyslijY(double y)
{
    if (!gniazdo || gniazdo->state() != QAbstractSocket::ConnectedState) return;

    QByteArray dane;
    QDataStream out(&dane, QIODevice::WriteOnly);
    out << static_cast<quint8>(MessageType::RegulatedValue) << y;
    gniazdo->write(dane);
}
void dialog_sieciowy::wyslijKonfiguracje(KonfiguracjaSieciowa konf)
{
    if (!gniazdo || gniazdo->state() != QAbstractSocket::ConnectedState) return;

    QByteArray dane;
    QDataStream out(&dane, QIODevice::WriteOnly);
    out << static_cast<quint8>(MessageType::Konfiguracja)
        << konf.interwalMs << konf.k << konf.Ti << konf.Td << konf.trybCalki
        << konf.typSygnalu << konf.zadana << konf.stala << konf.T << konf.p;
    gniazdo->write(dane);
}
void dialog_sieciowy::wyslijGotowosc()
{
    if (!gniazdo || gniazdo->state() != QAbstractSocket::ConnectedState) return;

    QByteArray dane;
    QDataStream out(&dane, QIODevice::WriteOnly);
    out << static_cast<quint8>(MessageType::ReadyStatus);
    gniazdo->write(dane);
}
void dialog_sieciowy::wyslijStartStop(bool start)
{
    if (!gniazdo || gniazdo->state() != QAbstractSocket::ConnectedState) return;

    QByteArray dane;
    QDataStream out(&dane, QIODevice::WriteOnly);
    out << static_cast<quint8>(start ? MessageType::StartCommand : MessageType::StopCommand);
    gniazdo->write(dane);
}
bool dialog_sieciowy::czyPolaczono() const
{
    return (gniazdo && gniazdo->state() == QAbstractSocket::ConnectedState) ||
           (serwer && serwer->isListening());
}
bool dialog_sieciowy::czyAktywnePolaczenie() const
{
    return (tryb != TrybPracySieciowej::Brak) &&
           ((gniazdo && gniazdo->state() == QAbstractSocket::ConnectedState) ||
            (serwer && serwer->isListening()));
}
void dialog_sieciowy::rozlacz()
{
    przejdzWLokalnyTryb();
    emit rozlaczono();
}
