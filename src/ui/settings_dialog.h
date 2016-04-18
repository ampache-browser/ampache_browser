// settings_dialog.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H



#include <memory>
#include <QDialog>

class QFormLayout;
class QCheckBox;
class QLabel;
class QLineEdit;
class QDialogButtonBox;



namespace ui {

/**
 * @brief Dialog for settings and user preferences.
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog();

    ~SettingsDialog();

    /**
     * @brief Gets the preference on the demo server usage.
     *
     * @return true if the user chose to use the demo server, false otherwise
     */
    bool getUseDemoServer() const;

    /**
     * @brief Gets the server URL.
     */
    std::string getServerUrl() const;

    /**
     * @brief Gets user name for login to the server.
     */
    std::string getUserName() const;

    /**
     * @brief Gets the password.
     */
    std::string getPassword() const;

    /**
     * @brief Populate the dialog with the given values.
     */
    void populate(bool useDemoServer, std::string serverUrl, std::string userName, std::string password = "");

private slots:
    void onUseDemoServerCheckBoxStateChanged(int state);
    void onDialogButtonBoxAccepted();

private:
    QFormLayout* myFormLayout;
    QCheckBox* myUseDemoServerCheckBox;
    QLineEdit* myServerLineEdit;
    QLineEdit* myUserLineEdit;
    QLineEdit* myPasswordLineEdit;
    QDialogButtonBox* myDialogButtonBox;

    bool myUseDemoServer;
    std::string myServerUrl;
    std::string myUserName;
    std::string myPassword;
};

}



#endif // SETTINGSDIALOG_H
