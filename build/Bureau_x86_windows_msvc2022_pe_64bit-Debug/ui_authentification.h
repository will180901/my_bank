/********************************************************************************
** Form generated from reading UI file 'authentification.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTHENTIFICATION_H
#define UI_AUTHENTIFICATION_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Authentification
{
public:
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QWidget *widget;
    QGridLayout *gridLayout;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *verticalLayout_7;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_6;
    QWidget *carte_connexion;
    QVBoxLayout *verticalLayout_5;
    QWidget *widget_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QLabel *label_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *lineEdit_email_connexion;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_4;
    QLineEdit *lineEdit_mot_de_passe_connexion;
    QLabel *zone_message_erreur_page_se_connecter;
    QPushButton *btn_mot_de_passe_oublier;
    QPushButton *btn_se_connecter_zone_connexion;
    QWidget *widget_4;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QPushButton *btn_creer_compte_zone_connexion;
    QWidget *page_2;
    QVBoxLayout *verticalLayout_14;
    QWidget *widget_5;
    QVBoxLayout *verticalLayout_9;
    QWidget *carte_creer_compte;
    QVBoxLayout *verticalLayout_8;
    QWidget *widget_6;
    QVBoxLayout *verticalLayout_10;
    QLabel *label_4;
    QLabel *label_5;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_11;
    QLineEdit *lineEdit_nom_complet_zone_creer_compte;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_12;
    QLineEdit *lineEdit_email_zone_creer_compte;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_15;
    QLineEdit *lineEdit_mot_de_passe_zone_creer_compte;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_16;
    QLineEdit *lineEdit_confirme_mot_de_passe_zone_creer_compte;
    QLabel *zone_message_erreur_page_creer_compte;
    QPushButton *btn_creer_compte_zone_creer_compte;
    QWidget *widget_7;
    QVBoxLayout *verticalLayout_13;
    QLabel *label_6;
    QPushButton *btn_se_connecter_zone_creer_compte;
    QWidget *page_3;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *Authentification)
    {
        if (Authentification->objectName().isEmpty())
            Authentification->setObjectName("Authentification");
        Authentification->resize(676, 576);
        Authentification->setStyleSheet(QString::fromUtf8("\n"
"* {\n"
"    \n"
"	 /*border: 1px solid rgb(0, 0, 0); */\n"
"	 color: rgb(74, 74, 74);\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"\n"
"\n"
"#zone_message_erreur_page_creer_compte,#zone_message_erreur_page_se_connecter{\n"
"\n"
"		\n"
"	font: italic 8pt \"Segoe UI\";\n"
"\n"
"	\n"
"	color: rgb(186, 2, 45);\n"
"\n"
"}\n"
"\n"
"\n"
"#btn_se_connecter_zone_creer_compte,#btn_creer_compte_zone_connexion{\n"
"\n"
"\n"
"    background-color: rgb(245, 247, 250);\n"
"    border: 1px solid rgb(214, 221, 235);\n"
"    color: rgb(74, 74, 74);\n"
"    border-radius: 4px;\n"
"    padding: 6px 10px;\n"
"\n"
"}\n"
"\n"
"\n"
"\n"
"#btn_se_connecter_zone_creer_compte:hover , #btn_creer_compte_zone_connexion:hover{\n"
"\n"
"   background-color: rgb(41, 98, 255);\n"
"	\n"
"	color: rgb(255, 255, 255);\n"
"}\n"
"\n"
"\n"
"#btn_mot_de_passe_oublier{\n"
"\n"
"		border:none ;\n"
"		background:transparent ;\n"
"		color:rgb(41, 98, 255);\n"
"\n"
"	\n"
"	font: 700 8pt \"Segoe UI\";\n"
"\n"
"\n"
"}\n"
"\n"
"#btn_mot_"
                        "de_passe_oublier:hover{\n"
"\n"
"		color:rgba(41, 98, 255,0.1);\n"
"\n"
"}\n"
"\n"
"\n"
"\n"
"/* ===============================================\n"
"   CONTENEURS ET PAGES PRINCIPALES\n"
"   =============================================== */\n"
"\n"
"#scrollAreaWidgetContents,#scrollAreaWidgetContents_2, \n"
"#scrollArea,#scrollArea_2, #Authentification , #page , #page_2{\n"
"    background-color: rgb(245, 247, 250);\n"
"    border: none;\n"
"}\n"
"\n"
"\n"
"\n"
"/* ===============================================\n"
"   CARTES ET CONTENEURS\n"
"   =============================================== */\n"
"\n"
"#carte_connexion , #carte_creer_compte {\n"
"    background-color: rgb(255, 255, 255);\n"
"    border-radius: 4px;\n"
"    border: 1px solid rgb(214, 221, 235);\n"
"}\n"
"\n"
"#carte_resume_liste {\n"
"    background-color: rgb(255, 255, 255);\n"
"    border-radius: 4px;\n"
"    border: 1px solid rgb(214, 221, 235);\n"
"}\n"
"\n"
"/* ===============================================\n"
"   INPUTS DE BASE\n"
" "
                        "  =============================================== */\n"
"\n"
"\n"
"QLineEdit, QTextEdit {\n"
"    background-color: rgb(245, 247, 250);\n"
"    border: 1px solid #D6DDEB;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    color: #4A4A4A;\n"
"}\n"
"\n"
"QLineEdit:hover, QTextEdit:hover {\n"
"    border: 1px solid #2962FF;\n"
"    background-color: rgb(250, 251, 253);\n"
"}\n"
"\n"
"QLineEdit:focus, QTextEdit:focus, QComboBox:focus, QDoubleSpinBox:focus {\n"
"    border: 1px solid #2962FF;\n"
"    outline: none;\n"
"}\n"
"\n"
"\n"
"/* ===============================================\n"
"   BOUTONS STANDARDS\n"
"   =============================================== */\n"
"\n"
"QPushButton {\n"
"    background-color: rgb(41, 98, 255);\n"
"    padding: 6px 12px;\n"
"    border-radius: 4px;\n"
"    border: none;\n"
"    color: rgb(255, 255, 255);\n"
"    font-weight: 500;\n"
"    min-height: 18px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(26, 84, 221);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
""
                        "    background-color: rgb(21, 74, 201);\n"
"}\n"
"\n"
"QToolButton {\n"
"    background: transparent;\n"
"    border: none;\n"
"    outline: none;\n"
"}\n"
"\n"
"\n"
"/* ===============================================\n"
"   S\303\211PARATEURS\n"
"   =============================================== */\n"
"\n"
"Line {\n"
"    background-color: rgb(37, 38, 40);\n"
"    border-radius: 1px;\n"
"    border: 1px solid rgb(37, 38, 40);\n"
"    width: 1px;\n"
"}\n"
"\n"
"/* ===============================================\n"
"   QCOMBOBOX - AM\303\211LIOR\303\211\n"
"   =============================================== */\n"
"\n"
"QComboBox {\n"
"    background-color: rgb(245, 247, 250);\n"
"    border: 1px solid #D6DDEB;\n"
"    border-radius: 4px;\n"
"    padding: 6px 24px 6px 8px;\n"
"    color: #4A4A4A;\n"
"    font-size: 13px;\n"
"    min-height: 18px;\n"
"    selection-background-color: #2962FF;\n"
"}\n"
"\n"
"QComboBox:hover {\n"
"    border-color: #2962FF;\n"
"    background-color: rgb(250, 251, 253);\n"
"}\n"
"\n"
""
                        "QComboBox:focus {\n"
"    border: 1px solid #2962FF;\n"
"    outline: none;\n"
"}\n"
"\n"
"QComboBox:pressed {\n"
"    background-color: rgb(240, 242, 247);\n"
"}\n"
"\n"
"/* Bouton d\303\251roulant - SANS BORDURE */\n"
"QComboBox::drop-down {\n"
"    subcontrol-origin: padding;\n"
"    subcontrol-position: top right;\n"
"    width: 20px;\n"
"    border: none; /* Bordure supprim\303\251e */\n"
"    background-color: transparent;\n"
"    border-top-right-radius: 4px;\n"
"    border-bottom-right-radius: 4px;\n"
"}\n"
"\n"
"QComboBox::drop-down:hover {\n"
"    background-color: rgba(214, 221, 235, 30);\n"
"}\n"
"\n"
"QComboBox::drop-down:pressed {\n"
"    background-color: rgba(214, 221, 235, 50);\n"
"}\n"
"\n"
"/* Fl\303\250che du ComboBox */\n"
"QComboBox::down-arrow {\n"
"    image: url(:/icon_gris/chevron-down.svg);\n"
"    width: 12px;\n"
"    height: 12px;\n"
"}\n"
"\n"
"QComboBox::down-arrow:pressed {\n"
"    image: url(:/icon_noir/chevron-down.svg);\n"
"}\n"
"\n"
"/* Liste d\303\251roulante */\n"
"QComboB"
                        "ox QAbstractItemView {\n"
"    border: 1px solid #2962FF;\n"
"    border-radius: 4px;\n"
"    background-color: white;\n"
"    selection-background-color: #2962FF;\n"
"    selection-color: white;\n"
"    padding: 2px;\n"
"    outline: none;\n"
"    show-decoration-selected: 1;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView::item {\n"
"    height: 28px;\n"
"    padding-left: 8px;\n"
"    padding-right: 8px;\n"
"    border-bottom: 1px solid rgba(214, 221, 235, 50);\n"
"    color: #4A4A4A;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView::item:last {\n"
"    border-bottom: none;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView::item:hover {\n"
"    background-color: rgba(41, 98, 255, 10);\n"
"    color: #2962FF;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView::item:selected {\n"
"    background-color: #2962FF;\n"
"    color: white;\n"
"}\n"
"\n"
"/* ===============================================\n"
"   QDOUBLESPINBOX - AM\303\211LIOR\303\211\n"
"   =============================================== */\n"
"\n"
"QDoubleSpinBox, QSpinBox "
                        "{\n"
"    background-color: rgb(245, 247, 250);\n"
"    border: 1px solid #D6DDEB;\n"
"    border-radius: 4px;\n"
"    padding: 6px 8px;\n"
"    color: #4A4A4A;\n"
"    font-size: 13px;\n"
"    min-height: 18px;\n"
"    selection-background-color: #2962FF;\n"
"}\n"
"\n"
"QDoubleSpinBox:hover, QSpinBox:hover {\n"
"    border-color: #2962FF;\n"
"    background-color: rgb(250, 251, 253);\n"
"}\n"
"\n"
"QDoubleSpinBox:focus, QSpinBox:focus {\n"
"    border: 1px solid #2962FF;\n"
"    outline: none;\n"
"}\n"
"\n"
"/* Bouton UP (Incr\303\251ment) - SANS BORDURE */\n"
"QDoubleSpinBox::up-button, QSpinBox::up-button {\n"
"    subcontrol-origin: border;\n"
"    subcontrol-position: top right;\n"
"    width: 18px;\n"
"    height: 10px;\n"
"    border: none; /* Bordure supprim\303\251e */\n"
"    background-color: transparent;\n"
"    margin-top: 1px;\n"
"    margin-right: 1px;\n"
"}\n"
"\n"
"QDoubleSpinBox::up-button:hover, QSpinBox::up-button:hover {\n"
"    background-color: rgba(214, 221, 235, 30);\n"
"}\n"
"\n"
"QDo"
                        "ubleSpinBox::up-button:pressed, QSpinBox::up-button:pressed {\n"
"    background-color: rgba(214, 221, 235, 50);\n"
"}\n"
"\n"
"/* Bouton DOWN (D\303\251cr\303\251ment) - SANS BORDURE */\n"
"QDoubleSpinBox::down-button, QSpinBox::down-button {\n"
"    subcontrol-origin: border;\n"
"    subcontrol-position: bottom right;\n"
"    width: 18px;\n"
"    height: 10px;\n"
"    border: none; /* Bordure supprim\303\251e */\n"
"    background-color: transparent;\n"
"    margin-bottom: 1px;\n"
"    margin-right: 1px;\n"
"}\n"
"\n"
"QDoubleSpinBox::down-button:hover, QSpinBox::down-button:hover {\n"
"    background-color: rgba(214, 221, 235, 30);\n"
"}\n"
"\n"
"QDoubleSpinBox::down-button:pressed, QSpinBox::down-button:pressed {\n"
"    background-color: rgba(214, 221, 235, 50);\n"
"}\n"
"\n"
"/* Fl\303\250ches des SpinBox */\n"
"QDoubleSpinBox::up-arrow, QSpinBox::up-arrow {\n"
"    image: url(:/icon_gris/chevron-up.svg);\n"
"    width: 8px;\n"
"    height: 8px;\n"
"}\n"
"\n"
"QDoubleSpinBox::up-arrow:hover, QSpinBox::up"
                        "-arrow:hover {\n"
"    image: url(:/icon_noir/chevron-up.svg);\n"
"}\n"
"\n"
"QDoubleSpinBox::up-arrow:pressed, QSpinBox::up-arrow:pressed {\n"
"    image: url(:/icon_noir/chevron-up.svg);\n"
"}\n"
"\n"
"QDoubleSpinBox::down-arrow, QSpinBox::down-arrow {\n"
"    image: url(:/icon_gris/chevron-down.svg);\n"
"    width: 8px;\n"
"    height: 8px;\n"
"}\n"
"\n"
"QDoubleSpinBox::down-arrow:hover, QSpinBox::down-arrow:hover {\n"
"    image: url(:/icon_noir/chevron-down.svg);\n"
"}\n"
"\n"
"QDoubleSpinBox::down-arrow:pressed, QSpinBox::down-arrow:pressed {\n"
"    image: url(:/icon_noir/chevron-down.svg);\n"
"}\n"
"\n"
"/* D\303\251sactiver les fl\303\250ches */\n"
"QDoubleSpinBox::up-arrow:disabled, QSpinBox::up-arrow:disabled {\n"
"    image: url(:/icon_gris/chevron-up-disabled.svg);\n"
"}\n"
"\n"
"QDoubleSpinBox::down-arrow:disabled, QSpinBox::down-arrow:disabled {\n"
"    image: url(:/icon_gris/chevron-down-disabled.svg);\n"
"}\n"
"\n"
"/* ===============================================\n"
"   QDATEEDIT\n"
"   =="
                        "============================================= */\n"
"\n"
"QDateEdit {\n"
"    border: 1px solid #D6DDEB;\n"
"    border-radius: 4px;\n"
"    padding: 6px 24px 6px 8px;\n"
"    background-color: rgb(245, 247, 250);\n"
"    color: #4A4A4A;\n"
"    font-size: 13px;\n"
"    min-height: 18px;\n"
"}\n"
"\n"
"QDateEdit:hover {\n"
"    border-color: #2962FF;\n"
"    background-color: rgb(250, 251, 253);\n"
"}\n"
"\n"
"QDateEdit:focus {\n"
"    border: 1px solid #2962FF;\n"
"    outline: none;\n"
"}\n"
"\n"
"QDateEdit::drop-down {\n"
"    subcontrol-origin: padding;\n"
"    subcontrol-position: top right;\n"
"    width: 20px;\n"
"    border: none;\n"
"    border-left: 1px solid #D6DDEB;\n"
"    border-top-right-radius: 4px;\n"
"    border-bottom-right-radius: 4px;\n"
"    background-color: rgba(214, 221, 235, 30);\n"
"}\n"
"\n"
"QDateEdit::drop-down:hover {\n"
"    background-color: rgba(214, 221, 235, 60);\n"
"}\n"
"\n"
"QDateEdit::down-arrow {\n"
"    image: url(:/icon_gris/calendar.svg);\n"
"    width: 12px;\n"
"   "
                        " height: 12px;\n"
"}\n"
"\n"
"/* ===============================================\n"
"   QGROUPBOX\n"
"   =============================================== */\n"
"\n"
"QGroupBox {\n"
"    \n"
"	border:none;\n"
"    border-radius: 4px;\n"
"    margin-top: 6px;\n"
"    padding-top: 8px;\n"
"    padding-left: 6px;\n"
"    padding-right: 6px;\n"
"    padding-bottom: 6px;\n"
"    background: rgba(255, 255, 255, 70);\n"
"    font-weight: 500;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 8px;\n"
"    padding: 0 6px;\n"
"    color: #505050;\n"
"    font-weight: 600;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"\n"
"\n"
"/* ===============================================\n"
"   BARRES DE D\303\211FILEMENT PERSONNALIS\303\211ES\n"
"   =============================================== */\n"
"\n"
"/* Barre de d\303\251filement verticale */\n"
"QScrollBar:vertical {\n"
"    border: none;\n"
"    background-color: rgba(245, 247, 250, 0);\n"
"    width: 10px;\n"
"    margin: 0;\n"
"}\n"
"\n"
"/* B"
                        "arre de d\303\251filement horizontale */\n"
"QScrollBar:horizontal {\n"
"    border: none;\n"
"    background-color: rgba(245, 247, 250, 0);\n"
"    height: 10px;\n"
"    margin: 0;\n"
"}\n"
"\n"
"/* Poign\303\251e de la barre de d\303\251filement - \303\211tat normal */\n"
"QScrollBar::handle:vertical,\n"
"QScrollBar::handle:horizontal {\n"
"    background-color: rgba(214, 221, 235, 50);\n"
"    border-radius: 4px;\n"
"    min-height: 20px;\n"
"    min-width: 20px;\n"
"    opacity: 0;\n"
"    transition: opacity 300ms ease, background-color 300ms ease;\n"
"}\n"
"\n"
"/* Poign\303\251e de la barre de d\303\251filement - Survol */\n"
"QScrollBar::handle:vertical:hover,\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background-color: rgba(41, 98, 255, 150);\n"
"    opacity: 1;\n"
"}\n"
"\n"
"/* Poign\303\251e de la barre de d\303\251filement - Press\303\251e */\n"
"QScrollBar::handle:vertical:pressed,\n"
"QScrollBar::handle:horizontal:pressed {\n"
"    background-color: rgba(21, 74, 201, 200);\n"
"}\n"
"\n"
""
                        "/* Zone d'incr\303\251ment/d\303\251cr\303\251ment - Masqu\303\251e */\n"
"QScrollBar::add-line:vertical,\n"
"QScrollBar::sub-line:vertical,\n"
"QScrollBar::add-line:horizontal,\n"
"QScrollBar::sub-line:horizontal {\n"
"    border: none;\n"
"    background: none;\n"
"    height: 0;\n"
"    width: 0;\n"
"}\n"
"\n"
"/* Fond de la barre de d\303\251filement - Survol */\n"
"QScrollBar:vertical:hover,\n"
"QScrollBar:horizontal:hover {\n"
"    background-color: rgba(245, 247, 250, 30);\n"
"}\n"
"\n"
"/* Coin entre les deux barres de d\303\251filement */\n"
"QScrollBar::add-page:vertical,\n"
"QScrollBar::sub-page:vertical,\n"
"QScrollBar::add-page:horizontal,\n"
"QScrollBar::sub-page:horizontal {\n"
"    background: none;\n"
"}\n"
"\n"
"\n"
""));
        horizontalLayout = new QHBoxLayout(Authentification);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(80, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        widget = new QWidget(Authentification);
        widget->setObjectName("widget");
        widget->setMinimumSize(QSize(491, 441));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(0);
        gridLayout->setVerticalSpacing(2);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        stackedWidget = new QStackedWidget(widget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setMinimumSize(QSize(325, 350));
        page = new QWidget();
        page->setObjectName("page");
        verticalLayout_7 = new QVBoxLayout(page);
        verticalLayout_7->setSpacing(2);
        verticalLayout_7->setObjectName("verticalLayout_7");
        verticalLayout_7->setContentsMargins(2, 2, 2, 2);
        widget_2 = new QWidget(page);
        widget_2->setObjectName("widget_2");
        widget_2->setMinimumSize(QSize(400, 400));
        verticalLayout_6 = new QVBoxLayout(widget_2);
        verticalLayout_6->setSpacing(2);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(2, 2, 2, 2);
        carte_connexion = new QWidget(widget_2);
        carte_connexion->setObjectName("carte_connexion");
        carte_connexion->setMinimumSize(QSize(0, 0));
        verticalLayout_5 = new QVBoxLayout(carte_connexion);
        verticalLayout_5->setSpacing(2);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(10, 10, 10, 10);
        widget_3 = new QWidget(carte_connexion);
        widget_3->setObjectName("widget_3");
        verticalLayout_2 = new QVBoxLayout(widget_3);
        verticalLayout_2->setObjectName("verticalLayout_2");
        label_2 = new QLabel(widget_3);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_2->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);

        verticalLayout_2->addWidget(label_2);

        label_3 = new QLabel(widget_3);
        label_3->setObjectName("label_3");
        label_3->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_3->setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByKeyboard|Qt::TextInteractionFlag::LinksAccessibleByMouse|Qt::TextInteractionFlag::TextBrowserInteraction|Qt::TextInteractionFlag::TextSelectableByKeyboard|Qt::TextInteractionFlag::TextSelectableByMouse);

        verticalLayout_2->addWidget(label_3);


        verticalLayout_5->addWidget(widget_3);

        groupBox = new QGroupBox(carte_connexion);
        groupBox->setObjectName("groupBox");
        groupBox->setMinimumSize(QSize(0, 0));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setObjectName("verticalLayout_3");
        lineEdit_email_connexion = new QLineEdit(groupBox);
        lineEdit_email_connexion->setObjectName("lineEdit_email_connexion");

        verticalLayout_3->addWidget(lineEdit_email_connexion);


        verticalLayout_5->addWidget(groupBox);

        groupBox_2 = new QGroupBox(carte_connexion);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setMinimumSize(QSize(0, 0));
        verticalLayout_4 = new QVBoxLayout(groupBox_2);
        verticalLayout_4->setObjectName("verticalLayout_4");
        lineEdit_mot_de_passe_connexion = new QLineEdit(groupBox_2);
        lineEdit_mot_de_passe_connexion->setObjectName("lineEdit_mot_de_passe_connexion");
        lineEdit_mot_de_passe_connexion->setEchoMode(QLineEdit::EchoMode::Password);

        verticalLayout_4->addWidget(lineEdit_mot_de_passe_connexion);


        verticalLayout_5->addWidget(groupBox_2);

        zone_message_erreur_page_se_connecter = new QLabel(carte_connexion);
        zone_message_erreur_page_se_connecter->setObjectName("zone_message_erreur_page_se_connecter");
        zone_message_erreur_page_se_connecter->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_5->addWidget(zone_message_erreur_page_se_connecter);

        btn_mot_de_passe_oublier = new QPushButton(carte_connexion);
        btn_mot_de_passe_oublier->setObjectName("btn_mot_de_passe_oublier");
        btn_mot_de_passe_oublier->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_5->addWidget(btn_mot_de_passe_oublier, 0, Qt::AlignmentFlag::AlignLeft);

        btn_se_connecter_zone_connexion = new QPushButton(carte_connexion);
        btn_se_connecter_zone_connexion->setObjectName("btn_se_connecter_zone_connexion");
        btn_se_connecter_zone_connexion->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icon_blanc/log-in.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btn_se_connecter_zone_connexion->setIcon(icon);

        verticalLayout_5->addWidget(btn_se_connecter_zone_connexion);


        verticalLayout_6->addWidget(carte_connexion);

        widget_4 = new QWidget(widget_2);
        widget_4->setObjectName("widget_4");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_4->sizePolicy().hasHeightForWidth());
        widget_4->setSizePolicy(sizePolicy);
        widget_4->setMinimumSize(QSize(0, 0));
        verticalLayout = new QVBoxLayout(widget_4);
        verticalLayout->setSpacing(25);
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(widget_4);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label->setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByKeyboard|Qt::TextInteractionFlag::LinksAccessibleByMouse|Qt::TextInteractionFlag::TextBrowserInteraction|Qt::TextInteractionFlag::TextSelectableByKeyboard|Qt::TextInteractionFlag::TextSelectableByMouse);

        verticalLayout->addWidget(label, 0, Qt::AlignmentFlag::AlignVCenter);

        btn_creer_compte_zone_connexion = new QPushButton(widget_4);
        btn_creer_compte_zone_connexion->setObjectName("btn_creer_compte_zone_connexion");
        btn_creer_compte_zone_connexion->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout->addWidget(btn_creer_compte_zone_connexion);


        verticalLayout_6->addWidget(widget_4, 0, Qt::AlignmentFlag::AlignTop);


        verticalLayout_7->addWidget(widget_2, 0, Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        verticalLayout_14 = new QVBoxLayout(page_2);
        verticalLayout_14->setSpacing(2);
        verticalLayout_14->setObjectName("verticalLayout_14");
        verticalLayout_14->setContentsMargins(2, 2, 2, 2);
        widget_5 = new QWidget(page_2);
        widget_5->setObjectName("widget_5");
        widget_5->setMinimumSize(QSize(400, 400));
        verticalLayout_9 = new QVBoxLayout(widget_5);
        verticalLayout_9->setSpacing(2);
        verticalLayout_9->setObjectName("verticalLayout_9");
        verticalLayout_9->setContentsMargins(2, 2, 2, 2);
        carte_creer_compte = new QWidget(widget_5);
        carte_creer_compte->setObjectName("carte_creer_compte");
        carte_creer_compte->setMinimumSize(QSize(311, 291));
        verticalLayout_8 = new QVBoxLayout(carte_creer_compte);
        verticalLayout_8->setSpacing(2);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(10, 10, 10, 10);
        widget_6 = new QWidget(carte_creer_compte);
        widget_6->setObjectName("widget_6");
        verticalLayout_10 = new QVBoxLayout(widget_6);
        verticalLayout_10->setObjectName("verticalLayout_10");
        label_4 = new QLabel(widget_6);
        label_4->setObjectName("label_4");
        label_4->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_10->addWidget(label_4);

        label_5 = new QLabel(widget_6);
        label_5->setObjectName("label_5");
        label_5->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_10->addWidget(label_5);


        verticalLayout_8->addWidget(widget_6);

        groupBox_3 = new QGroupBox(carte_creer_compte);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setMinimumSize(QSize(0, 0));
        verticalLayout_11 = new QVBoxLayout(groupBox_3);
        verticalLayout_11->setObjectName("verticalLayout_11");
        lineEdit_nom_complet_zone_creer_compte = new QLineEdit(groupBox_3);
        lineEdit_nom_complet_zone_creer_compte->setObjectName("lineEdit_nom_complet_zone_creer_compte");

        verticalLayout_11->addWidget(lineEdit_nom_complet_zone_creer_compte);


        verticalLayout_8->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(carte_creer_compte);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setMinimumSize(QSize(0, 0));
        verticalLayout_12 = new QVBoxLayout(groupBox_4);
        verticalLayout_12->setObjectName("verticalLayout_12");
        lineEdit_email_zone_creer_compte = new QLineEdit(groupBox_4);
        lineEdit_email_zone_creer_compte->setObjectName("lineEdit_email_zone_creer_compte");

        verticalLayout_12->addWidget(lineEdit_email_zone_creer_compte);


        verticalLayout_8->addWidget(groupBox_4);

        groupBox_5 = new QGroupBox(carte_creer_compte);
        groupBox_5->setObjectName("groupBox_5");
        groupBox_5->setMinimumSize(QSize(0, 0));
        verticalLayout_15 = new QVBoxLayout(groupBox_5);
        verticalLayout_15->setObjectName("verticalLayout_15");
        lineEdit_mot_de_passe_zone_creer_compte = new QLineEdit(groupBox_5);
        lineEdit_mot_de_passe_zone_creer_compte->setObjectName("lineEdit_mot_de_passe_zone_creer_compte");
        lineEdit_mot_de_passe_zone_creer_compte->setEchoMode(QLineEdit::EchoMode::Password);

        verticalLayout_15->addWidget(lineEdit_mot_de_passe_zone_creer_compte);


        verticalLayout_8->addWidget(groupBox_5);

        groupBox_6 = new QGroupBox(carte_creer_compte);
        groupBox_6->setObjectName("groupBox_6");
        groupBox_6->setMinimumSize(QSize(0, 0));
        verticalLayout_16 = new QVBoxLayout(groupBox_6);
        verticalLayout_16->setObjectName("verticalLayout_16");
        lineEdit_confirme_mot_de_passe_zone_creer_compte = new QLineEdit(groupBox_6);
        lineEdit_confirme_mot_de_passe_zone_creer_compte->setObjectName("lineEdit_confirme_mot_de_passe_zone_creer_compte");
        lineEdit_confirme_mot_de_passe_zone_creer_compte->setEchoMode(QLineEdit::EchoMode::Password);

        verticalLayout_16->addWidget(lineEdit_confirme_mot_de_passe_zone_creer_compte);

        zone_message_erreur_page_creer_compte = new QLabel(groupBox_6);
        zone_message_erreur_page_creer_compte->setObjectName("zone_message_erreur_page_creer_compte");
        zone_message_erreur_page_creer_compte->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_16->addWidget(zone_message_erreur_page_creer_compte);


        verticalLayout_8->addWidget(groupBox_6);

        btn_creer_compte_zone_creer_compte = new QPushButton(carte_creer_compte);
        btn_creer_compte_zone_creer_compte->setObjectName("btn_creer_compte_zone_creer_compte");
        btn_creer_compte_zone_creer_compte->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icon_blanc/user-plus.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btn_creer_compte_zone_creer_compte->setIcon(icon1);

        verticalLayout_8->addWidget(btn_creer_compte_zone_creer_compte);


        verticalLayout_9->addWidget(carte_creer_compte);

        widget_7 = new QWidget(widget_5);
        widget_7->setObjectName("widget_7");
        widget_7->setMinimumSize(QSize(311, 71));
        verticalLayout_13 = new QVBoxLayout(widget_7);
        verticalLayout_13->setSpacing(25);
        verticalLayout_13->setObjectName("verticalLayout_13");
        label_6 = new QLabel(widget_7);
        label_6->setObjectName("label_6");
        label_6->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_13->addWidget(label_6, 0, Qt::AlignmentFlag::AlignBottom);

        btn_se_connecter_zone_creer_compte = new QPushButton(widget_7);
        btn_se_connecter_zone_creer_compte->setObjectName("btn_se_connecter_zone_creer_compte");
        btn_se_connecter_zone_creer_compte->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_13->addWidget(btn_se_connecter_zone_creer_compte);


        verticalLayout_9->addWidget(widget_7);


        verticalLayout_14->addWidget(widget_5, 0, Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);

        stackedWidget->addWidget(page_2);
        page_3 = new QWidget();
        page_3->setObjectName("page_3");
        stackedWidget->addWidget(page_3);

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);


        horizontalLayout->addWidget(widget);

        horizontalSpacer_2 = new QSpacerItem(79, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        retranslateUi(Authentification);

        QMetaObject::connectSlotsByName(Authentification);
    } // setupUi

    void retranslateUi(QDialog *Authentification)
    {
        Authentification->setWindowTitle(QCoreApplication::translate("Authentification", "Dialog", nullptr));
        label_2->setText(QCoreApplication::translate("Authentification", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:20pt; font-weight:700; color:#4387ca;\">My Bank .</span></p></body></html>", nullptr));
        label_3->setText(QCoreApplication::translate("Authentification", "Connectez-vous \303\240 votre espace personnel .", nullptr));
        groupBox->setTitle(QCoreApplication::translate("Authentification", "Adresse e-mail", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("Authentification", "Mot  de passe", nullptr));
        zone_message_erreur_page_se_connecter->setText(QString());
        btn_mot_de_passe_oublier->setText(QCoreApplication::translate("Authentification", "Mot  de passe oubli\303\251 ?", nullptr));
        btn_se_connecter_zone_connexion->setText(QCoreApplication::translate("Authentification", "Se connecter", nullptr));
        label->setText(QCoreApplication::translate("Authentification", "Vous n'avez pas de compte ?", nullptr));
        btn_creer_compte_zone_connexion->setText(QCoreApplication::translate("Authentification", "cr\303\251er un compte", nullptr));
        label_4->setText(QCoreApplication::translate("Authentification", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:20pt; font-weight:700; color:#4387ca;\">My Bank .</span></p></body></html>", nullptr));
        label_5->setText(QCoreApplication::translate("Authentification", "Rejoignez Banque Moderne d\303\250s aujourd'hui.", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("Authentification", "Nom complet", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("Authentification", "Adresse e-mail", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("Authentification", "Mot de passe ", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("Authentification", "Confirmer le mot de passe ", nullptr));
        zone_message_erreur_page_creer_compte->setText(QString());
        btn_creer_compte_zone_creer_compte->setText(QCoreApplication::translate("Authentification", "Cr\303\251er mon compte", nullptr));
        label_6->setText(QCoreApplication::translate("Authentification", "Vous avez d\303\251j\303\240 un compte ?", nullptr));
        btn_se_connecter_zone_creer_compte->setText(QCoreApplication::translate("Authentification", "Se connecter", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Authentification: public Ui_Authentification {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTHENTIFICATION_H
