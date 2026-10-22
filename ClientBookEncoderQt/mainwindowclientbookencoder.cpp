#include "mainwindowclientbookencoder.h"
#include "ui_mainwindowclientbookencoder.h"
#include "unistd.h"
#include <QInputDialog>
#include <QMessageBox>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "OBEP.h"
#include "TCP.h"

using namespace std;

MainWindowClientBookEncoder::MainWindowClientBookEncoder(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClientBookEncoder)
{
    ui->setupUi(this);
    sSocket = -1;
    ::close(2); // Redirection standard du squelette

    // Configuration de la table des livres encodés
    ui->tableWidgetEncodedBooks->setColumnCount(9);
    ui->tableWidgetEncodedBooks->setRowCount(0);
    QStringList labelsTableEmployes;
    labelsTableEmployes << "Id" << "Titre" << "Auteur" << "Sujet" << "ISBN" << "Pages" << "Année" << "Prix" << "Stock";
    ui->tableWidgetEncodedBooks->setHorizontalHeaderLabels(labelsTableEmployes);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setVisible(true);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetEncodedBooks->verticalHeader()->setVisible(false);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setStyleSheet("background-color: lightyellow");
    int columnWidths[] = {35, 250, 200, 200, 150, 50, 50, 50, 40};
    for (int col = 0; col < 9; ++col)
        ui->tableWidgetEncodedBooks->setColumnWidth(col, columnWidths[col]);

    this->logoutOk();

    // Suppression des données de test fictives en dur au démarrage
    this->clearTableBooks();
    this->clearComboBoxAuthors();
    this->clearComboBoxSubjects();
}

MainWindowClientBookEncoder::~MainWindowClientBookEncoder() {
    if (sSocket != -1) {
        ::close(sSocket);
    }
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table des livres encodés (ne pas modifier) ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::addTupleTableBooks(int id,
                                                     string title,
                                                     string author,
                                                     string subject,
                                                     string isbn,
                                                     int pageCount,
                                                     int publishYear,
                                                     float price,
                                                     int stockQuantity)
{
    int nb = ui->tableWidgetEncodedBooks->rowCount();
    nb++;
    ui->tableWidgetEncodedBooks->setRowCount(nb);
    ui->tableWidgetEncodedBooks->setRowHeight(nb-1,10);

    // id
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(id));
    ui->tableWidgetEncodedBooks->setItem(nb-1,0,item);

    // title
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setText(QString::fromStdString(title));
    ui->tableWidgetEncodedBooks->setItem(nb-1,1,item);

    // author
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(author));
    ui->tableWidgetEncodedBooks->setItem(nb-1,2,item);

    // subject
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(subject));
    ui->tableWidgetEncodedBooks->setItem(nb-1,3,item);

    // isbn
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(isbn));
    ui->tableWidgetEncodedBooks->setItem(nb-1,4,item);

    // pageCount
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(pageCount));
    ui->tableWidgetEncodedBooks->setItem(nb-1,5,item);

    // publishYear
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(publishYear));
    ui->tableWidgetEncodedBooks->setItem(nb-1,6,item);

    // price
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(price));
    ui->tableWidgetEncodedBooks->setItem(nb-1,7,item);

    // stockQuantity
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(stockQuantity));
    ui->tableWidgetEncodedBooks->setItem(nb-1,8,item);
}

void MainWindowClientBookEncoder::clearTableBooks() {
    ui->tableWidgetEncodedBooks->setRowCount(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles des comboboxes (ne pas modifier) //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::addComboBoxAuthors(string author){
    ui->comboBoxAuthors->addItem(QString::fromStdString(author));
}

string MainWindowClientBookEncoder::getSelectionAuthor() const {
    return ui->comboBoxAuthors->currentText().toStdString();
}

void MainWindowClientBookEncoder::clearComboBoxAuthors() {
    ui->comboBoxAuthors->clear();
}

void MainWindowClientBookEncoder::addComboBoxSubjects(string subject){
    ui->comboBoxSubjects->addItem(QString::fromStdString(subject));
}

string MainWindowClientBookEncoder::getSelectionSubject() const {
    return ui->comboBoxSubjects->currentText().toStdString();
}

void MainWindowClientBookEncoder::clearComboBoxSubjects() {
    ui->comboBoxSubjects->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonction utiles de la fenêtre (ne pas modifier) ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string MainWindowClientBookEncoder::getTitle() const {
    return ui->lineEditTitle->text().toStdString();
}

string MainWindowClientBookEncoder::getIsbn() const {
    return ui->lineEditIsbn->text().toStdString();
}

int MainWindowClientBookEncoder::getPageCount() const {
    return ui->spinBoxPageCount->value();
}

float MainWindowClientBookEncoder::getPrice() const {
    return ui->doubleSpinBoxPrice->value();
}

int MainWindowClientBookEncoder::getPublishYear() const {
    return ui->spinBoxPublishYear->value();
}

int MainWindowClientBookEncoder::getStockQuantity() const {
    return ui->spinBoxStockQuantity->value();
}

void MainWindowClientBookEncoder::loginOk() {
    ui->pushButtonClear->setEnabled(true);
    ui->pushButtonAddBook->setEnabled(true);
    ui->pushButtonAddAuthor->setEnabled(true);
    ui->pushButtonAddSubject->setEnabled(true);
    ui->actionLogin->setEnabled(false);
    ui->actionLogout->setEnabled(true);
}

void MainWindowClientBookEncoder::logoutOk() {
    ui->pushButtonClear->setEnabled(false);
    ui->pushButtonAddBook->setEnabled(false);
    ui->pushButtonAddAuthor->setEnabled(false);
    ui->pushButtonAddSubject->setEnabled(false);
    ui->actionLogin->setEnabled(true);
    ui->actionLogout->setEnabled(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue (ne pas modifier) ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::dialogMessage(const string& title,const string& message) {
   QMessageBox::information(this,QString::fromStdString(title),QString::fromStdString(message));
}

void MainWindowClientBookEncoder::dialogError(const string& title,const string& message) {
   QMessageBox::critical(this,QString::fromStdString(title),QString::fromStdString(message));
}

string MainWindowClientBookEncoder::dialogInputText(const string& title,const string& question) {
    return QInputDialog::getText(this,QString::fromStdString(title),QString::fromStdString(question)).toStdString();
}

int MainWindowClientBookEncoder::dialogInputInt(const string& title,const string& question) {
    return QInputDialog::getInt(this,QString::fromStdString(title),QString::fromStdString(question));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions gestion des boutons et items de menu (IMPLÉMENTÉES) //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindowClientBookEncoder::on_actionLogin_triggered() {
    string user = this->dialogInputText("Connexion", "Nom d'utilisateur :");
    string password = this->dialogInputText("Connexion", "Mot de passe :");

    if (user.empty() || password.empty()) {
        this->dialogError("Erreur", "Identifiants invalides.");
        return;
    }

    if (sSocket == -1) {
        sSocket = ClientSocket((char*)"127.0.0.1", 50000);
        if (sSocket == -1) {
            this->dialogError("Erreur", "Impossible de se connecter au serveur.");
            return;
        }
    }

    char requete[2048];
    char reponse[2048];
    char* saveptr = NULL;

    sprintf(requete, "LOGIN#%s#%s\n", user.c_str(), password.c_str());

    Send(sSocket, requete, strlen(requete));
    if (ReceiveLine(sSocket, reponse, sizeof(reponse)) > 0) {
        char* token = strtok_r(reponse, "#", &saveptr); 
        char* status = strtok_r(NULL, "#", &saveptr);   

        if (status != NULL && strcmp(status, "OK") == 0) {
            this->loginOk();
            this->dialogMessage("Succès", "Connexion réussie !");

            // --- 1. CHARGEMENT DE LA COMBOBOX DES AUTEURS ---
            this->clearComboBoxAuthors();
            sprintf(requete, "GET_AUTHORS\n");
            Send(sSocket, requete, strlen(requete));

            memset(reponse, 0, sizeof(reponse));
            if (ReceiveLine(sSocket, reponse, sizeof(reponse)) > 0) {
                char* saveptrAuthors = NULL;
                token = strtok_r(reponse, "#", &saveptrAuthors); // GET_AUTHORS
                status = strtok_r(NULL, "#", &saveptrAuthors);  // OK

                if (status != NULL && strcmp(status, "OK") == 0) {
                    char* nbStr = strtok_r(NULL, "#", &saveptrAuthors);
                    int nbAuteurs = nbStr ? atoi(nbStr) : 0;

                    for (int i = 0; i < nbAuteurs; i++) {
                        char* nom = strtok_r(NULL, "#", &saveptrAuthors);
                        char* prenom = strtok_r(NULL, "#", &saveptrAuthors);
                        if (nom && prenom) {
                            string nomComplet = string(nom) + " " + string(prenom);
                            this->addComboBoxAuthors(nomComplet);
                        }
                    }
                }
            }

            // --- 2. CHARGEMENT DE LA COMBOBOX DES SUJETS ---
            this->clearComboBoxSubjects();
            sprintf(requete, "GET_SUBJECTS\n");
            Send(sSocket, requete, strlen(requete));

            memset(reponse, 0, sizeof(reponse));
            if (ReceiveLine(sSocket, reponse, sizeof(reponse)) > 0) {
                char* saveptrSubjects = NULL;
                token = strtok_r(reponse, "#", &saveptrSubjects); // GET_SUBJECTS
                status = strtok_r(NULL, "#", &saveptrSubjects);  // OK

                if (status != NULL && strcmp(status, "OK") == 0) {
                    char* nbStr = strtok_r(NULL, "#", &saveptrSubjects);
                    int nbSujets = nbStr ? atoi(nbStr) : 0;

                    for (int i = 0; i < nbSujets; i++) {
                        char* nomSujet = strtok_r(NULL, "#", &saveptrSubjects);
                        if (nomSujet) {
                            this->addComboBoxSubjects(nomSujet);
                        }
                    }
                }
            }

        } else {
            this->dialogError("Erreur", "Identifiants incorrects.");
        }
    }
}

void MainWindowClientBookEncoder::on_actionLogout_triggered() {
    if (sSocket != -1) {
        char requete[] = "LOGOUT\n";
        char reponse[512];

        Send(sSocket, requete, strlen(requete));
        ReceiveLine(sSocket, reponse, sizeof(reponse));

        ::close(sSocket);
        sSocket = -1;
    }
    this->logoutOk();
    this->dialogMessage("Information", "Vous êtes déconnecté.");
}

void MainWindowClientBookEncoder::on_pushButtonAddAuthor_clicked() {
    string nom = this->dialogInputText("Nouvel auteur", "Nom de l'auteur :");
    string prenom = this->dialogInputText("Nouvel auteur", "Prénom de l'auteur :");

    if (nom.empty() || prenom.empty()) {
        this->dialogError("Erreur", "Le nom et le prénom sont obligatoires.");
        return;
    }

    char requete[512];
    char reponse[512];
    
    sprintf(requete, "ADD_AUTHOR#%s#%s\n", nom.c_str(), prenom.c_str());

    Send(sSocket, requete, strlen(requete));
    if (ReceiveLine(sSocket, reponse, sizeof(reponse)) > 0) {
        char* token = strtok(reponse, "#");
        char* status = strtok(NULL, "#");

        if (status != NULL && strcmp(status, "OK") == 0) {
            this->addComboBoxAuthors(nom + " " + prenom);
            this->dialogMessage("Succès", "Auteur ajouté avec succès !");
        } else {
            this->dialogError("Erreur", "Échec de l'ajout de l'auteur.");
        }
    }
}

void MainWindowClientBookEncoder::on_pushButtonAddSubject_clicked() {
    string name = this->dialogInputText("Nouveau sujet", "Nom du sujet ?");

    if (name.empty()) {
        this->dialogError("Erreur", "Le nom du sujet est obligatoire.");
        return;
    }

    char requete[512];
    char reponse[512];

    // Format textuel : ADD_SUBJECT#nomSujet\n
    sprintf(requete, "ADD_SUBJECT#%s\n", name.c_str());

    Send(sSocket, requete, strlen(requete));

    if (ReceiveLine(sSocket, reponse, sizeof(reponse)) > 0) {
        char* token = strtok(reponse, "#");
        char* status = strtok(NULL, "#");

        if (status != NULL && strcmp(status, "OK") == 0) {
            this->addComboBoxSubjects(name);
            this->dialogMessage("Succès", "Sujet ajouté avec succès !");
        } else {
            this->dialogError("Erreur", "Échec de l'ajout du sujet (ou sujet déjà existant).");
        }
    }
}

void MainWindowClientBookEncoder::on_pushButtonAddBook_clicked() {
    string titre = this->getTitle();
    string author = this->getSelectionAuthor();
    string subject = this->getSelectionSubject();
    string isbn = this->getIsbn();
    int pageCount = this->getPageCount();
    int publishYear = this->getPublishYear();
    float price = this->getPrice();
    int stockQuantity = this->getStockQuantity();

    if (titre.empty() || isbn.empty()) {
        this->dialogError("Erreur", "Le titre et l'ISBN sont obligatoires.");
        return;
    }

    char requete[1024];
    char reponse[512];

    // Format textuel : ADD_BOOK#titre#author#subject#isbn#pageCount#publishYear#price#stockQuantity\n
    sprintf(requete, "ADD_BOOK#%s#%s#%s#%s#%d#%d#%.2f#%d\n", 
            titre.c_str(), author.c_str(), subject.c_str(), 
            isbn.c_str(), pageCount, publishYear, price, stockQuantity);

    Send(sSocket, requete, strlen(requete));
    if (ReceiveLine(sSocket, reponse, sizeof(reponse)) > 0) {
        char* token = strtok(reponse, "#");
        char* status = strtok(NULL, "#");

        if (status != NULL && strcmp(status, "OK") == 0) {
            char* idStr = strtok(NULL, "#");
            int id = idStr ? atoi(idStr) : 0;

            this->addTupleTableBooks(id, titre, author, subject, isbn, pageCount, publishYear, price, stockQuantity);
            this->dialogMessage("Succès", "Livre ajouté avec succès !");
        } else {
            this->dialogError("Erreur", "Échec de l'ajout du livre.");
        }
    }
}

void MainWindowClientBookEncoder::on_pushButtonClear_clicked() {
    ui->lineEditTitle->clear();
    ui->lineEditIsbn->clear();
    ui->spinBoxPageCount->setValue(0);
    ui->doubleSpinBoxPrice->setValue(0);
    ui->spinBoxPublishYear->setValue(0);
    ui->spinBoxStockQuantity->setValue(0);
}

void MainWindowClientBookEncoder::on_actionQuitter_triggered() {
    if (sSocket != -1) {
        char requete[] = "LOGOUT\n";
        Send(sSocket, requete, strlen(requete));
        ::close(sSocket);
    }
    QApplication::exit(0);
}