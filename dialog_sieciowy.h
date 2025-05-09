#ifndef DIALOG_SIECIOWY_H
#define DIALOG_SIECIOWY_H

#include <QDialog>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QDataStream>

QT_BEGIN_NAMESPACE
namespace Ui { class dialog_sieciowy; }
QT_END_NAMESPACE

enum class TrybPracySieciowej {
    Brak,
    Serwer,
    Klient
};

enum MessageType : quint8 {
    ConnectionAck = 0x10,
    ControlSignal = 0x20,
    RegulatedValue = 0x30,
    Konfiguracja = 0x03,
    ReadyStatus = 0x10,
    StartCommand = 0x11,
    StopCommand = 0x12
};

struct KonfiguracjaSieciowa {
    int interwalMs;
    double k, Ti, Td;
    bool trybCalki;
    int typSygnalu;
    double zadana;
    double stala;
    int T;
    double p;
};

class dialog_sieciowy : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_sieciowy(QWidget *parent = nullptr);
    ~dialog_sieciowy();
    TrybPracySieciowej getTryb() const;
    bool czyPolaczono() const;
    bool czyAktywnePolaczenie() const;

signals:
    void sygnalU(double u);
    void sygnalY(double y);
    void odebranoKonfiguracje(KonfiguracjaSieciowa konf);
    void rozlaczenieSieciowe();
    void otrzymanoGotowosc();
    void startSymulacjiZdalnej();
    void stopSymulacjiZdalnej();
    void polaczono(bool jestSerwerem);
    void rozlaczono();
    void zmienGUI(bool jestemRegulatorem);

public slots:
    void wyslijU(double u);
    void wyslijY(double y);
    void wyslijKonfiguracje(KonfiguracjaSieciowa konf);
    void wyslijGotowosc();
    void wyslijStartStop(bool start);
    void rozlacz();

private slots:
    void on_btnPolacz_clicked();
    void on_btnReady_clicked();
    void nowePolaczenie();
    void odebranoDane();

private:
    Ui::dialog_sieciowy *ui;
    QTcpServer *serwer = nullptr;
    QTcpSocket *gniazdo = nullptr;
    TrybPracySieciowej tryb = TrybPracySieciowej::Brak;
    QTimer *timerOczekiwania = nullptr;
    void ustawTryb(TrybPracySieciowej t);
    void ustawStatusPolaczenia(const QString &tekst, const QColor &kolor);
    void przejdzWLokalnyTryb();
    void przetworzWiadomosc(QDataStream &in);
};

#endif // DIALOG_SIECIOWY_H
