/********************************************************************************
** Form generated from reading UI file 'dialog_sieciowy.ui'
**
** Created by: Qt User Interface Compiler version 6.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_SIECIOWY_H
#define UI_DIALOG_SIECIOWY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_dialog_sieciowy
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QRadioButton *radioSerwer;
    QRadioButton *radioKlient;
    QLineEdit *lineIP;
    QLineEdit *linePort;
    QPushButton *btnReady;
    QPushButton *btnPolacz;
    QHBoxLayout *horizontalLayout;
    QLabel *labelStatus;
    QLabel *KontrolkaStatusu;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *dialog_sieciowy)
    {
        if (dialog_sieciowy->objectName().isEmpty())
            dialog_sieciowy->setObjectName("dialog_sieciowy");
        dialog_sieciowy->resize(567, 483);
        verticalLayout = new QVBoxLayout(dialog_sieciowy);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(dialog_sieciowy);
        groupBox->setObjectName("groupBox");
        groupBox->setMaximumSize(QSize(16777215, 100));
        radioSerwer = new QRadioButton(groupBox);
        radioSerwer->setObjectName("radioSerwer");
        radioSerwer->setGeometry(QRect(40, 30, 91, 22));
        radioKlient = new QRadioButton(groupBox);
        radioKlient->setObjectName("radioKlient");
        radioKlient->setGeometry(QRect(150, 30, 91, 22));

        verticalLayout->addWidget(groupBox);

        lineIP = new QLineEdit(dialog_sieciowy);
        lineIP->setObjectName("lineIP");

        verticalLayout->addWidget(lineIP);

        linePort = new QLineEdit(dialog_sieciowy);
        linePort->setObjectName("linePort");

        verticalLayout->addWidget(linePort);

        btnReady = new QPushButton(dialog_sieciowy);
        btnReady->setObjectName("btnReady");

        verticalLayout->addWidget(btnReady);

        btnPolacz = new QPushButton(dialog_sieciowy);
        btnPolacz->setObjectName("btnPolacz");

        verticalLayout->addWidget(btnPolacz);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        labelStatus = new QLabel(dialog_sieciowy);
        labelStatus->setObjectName("labelStatus");

        horizontalLayout->addWidget(labelStatus);

        KontrolkaStatusu = new QLabel(dialog_sieciowy);
        KontrolkaStatusu->setObjectName("KontrolkaStatusu");
        KontrolkaStatusu->setMinimumSize(QSize(16, 16));
        KontrolkaStatusu->setMaximumSize(QSize(50, 50));
        KontrolkaStatusu->setStyleSheet(QString::fromUtf8("background-color: gray;\n"
"border-radius: 8px;"));

        horizontalLayout->addWidget(KontrolkaStatusu);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(dialog_sieciowy);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(dialog_sieciowy);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog_sieciowy, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, dialog_sieciowy, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(dialog_sieciowy);
    } // setupUi

    void retranslateUi(QDialog *dialog_sieciowy)
    {
        dialog_sieciowy->setWindowTitle(QCoreApplication::translate("dialog_sieciowy", "Dialog", nullptr));
        groupBox->setTitle(QString());
        radioSerwer->setText(QCoreApplication::translate("dialog_sieciowy", "Serwer", nullptr));
        radioKlient->setText(QCoreApplication::translate("dialog_sieciowy", "Klient", nullptr));
        lineIP->setText(QCoreApplication::translate("dialog_sieciowy", "127.0.0.1", nullptr));
        linePort->setText(QCoreApplication::translate("dialog_sieciowy", "12345", nullptr));
        btnReady->setText(QCoreApplication::translate("dialog_sieciowy", "Gotowy", nullptr));
        btnPolacz->setText(QCoreApplication::translate("dialog_sieciowy", "Po\305\202\304\205cz", nullptr));
        labelStatus->setText(QCoreApplication::translate("dialog_sieciowy", "Niepod\305\202\304\205czono", nullptr));
        KontrolkaStatusu->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class dialog_sieciowy: public Ui_dialog_sieciowy {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_SIECIOWY_H
