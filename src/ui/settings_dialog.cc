// settings_dialog.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <sstream>
#include <QFormLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include "infrastructure/i18n.h"
#include "settings_dialog.h"

using namespace std;
using namespace infrastructure;



namespace ui {

SettingsDialog::SettingsDialog(): QDialog() {
    setWindowTitle(_("Settings"));
    resize(450, 120);

    myFormLayout = new QFormLayout{this};

    ostringstream versionString;
    versionString << "<small>" << _("Ampache Browser ver. ") << VERSION << "</small>";
    myVersionLabel = new QLabel{QString::fromStdString(versionString.str())};
    myVersionLabel->setAlignment(Qt::AlignRight);

    myUseDemoServerCheckBox = new QCheckBox{_("Use demo server")};
    myUseDemoServerCheckBox->setToolTip(
        _("When enabled Ampache Browser will connect to a predefined demo server.  Use it to try the plugin if you do not have an Ampache server around."));

    myServerLineEdit = new QLineEdit{};
    myServerLineEdit->setPlaceholderText("http://..., https://...");
    myServerLineEdit->setToolTip(_("Ampache server address."));
    myUserLineEdit = new QLineEdit{};
    myPasswordLineEdit = new QLineEdit{};
    myPasswordLineEdit->setEchoMode(QLineEdit::Password);

    myDialogButtonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};

    myFormLayout->addRow(myVersionLabel);
    myFormLayout->addRow(myUseDemoServerCheckBox);
    myFormLayout->addRow(_("Server URL:"), myServerLineEdit);
    myFormLayout->addRow(_("User Name: "), myUserLineEdit);
    myFormLayout->addRow(_("Password: "), myPasswordLineEdit);
    myFormLayout->addRow(myDialogButtonBox);

    connect(myUseDemoServerCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onUseDemoServerCheckBoxStateChanged(int)));
    connect(myDialogButtonBox, SIGNAL(accepted()), this, SLOT(onDialogButtonBoxAccepted()));
    connect(myDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
}



SettingsDialog::~SettingsDialog() {
    delete(myFormLayout);
}



bool SettingsDialog::getUseDemoServer() const {
    return myUseDemoServer;
}



string SettingsDialog::getServerUrl() const {
    return myServerUrl;
}



string SettingsDialog::getUserName() const {
    return myUserName;
}



string SettingsDialog::getPassword() const {
    return myPassword;
}



void SettingsDialog::populate(bool useDemoServer, string serverUrl, string userName, string password) {
    useDemoServer ? myUseDemoServerCheckBox->setCheckState(Qt::Checked) :
        myUseDemoServerCheckBox->setCheckState(Qt::Unchecked);
    myServerLineEdit->setText(QString::fromStdString(serverUrl));
    myUserLineEdit->setText(QString::fromStdString(userName));
    if (password != "") {
        myPasswordLineEdit->setText(QString::fromStdString(password));
    }
}



void SettingsDialog::onUseDemoServerCheckBoxStateChanged(int state) {
    if (state == Qt::Checked) {
        myServerLineEdit->setEnabled(false);
        myUserLineEdit->setEnabled(false);
        myPasswordLineEdit->setEnabled(false);
    } else {
        myServerLineEdit->setEnabled(true);
        myUserLineEdit->setEnabled(true);
        myPasswordLineEdit->setEnabled(true);
    }
}



void SettingsDialog::onDialogButtonBoxAccepted() {
    myUseDemoServer = myUseDemoServerCheckBox->checkState() == Qt::Checked;
    myServerUrl = myServerLineEdit->text().toStdString();
    myUserName = myUserLineEdit->text().toStdString();
    myPassword = myPasswordLineEdit->text().toStdString();
    accept();
}

}
