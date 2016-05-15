// settings_internal.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "application/data_loader.h"
#include "settings_internal.h"

using namespace std;



namespace application {

// SMELL: With this approach the key (variable) can be anything - it is not checked against defined constants in .h.
SettingsInternal::SettingsInternal() {
}



void SettingsInternal::connectChanged(function<void()> callback) {
    myChangedCb = callback;
}



string SettingsInternal::getString(const string& key) const {
    return myStrings.at(key);
}



void SettingsInternal::setString(const string& key, const string& value) {
    bool isNewKey;
    auto oldValue = (isNewKey = myStrings.count(key) == 0) ? myStrings[key] : "";

    myStrings[key] = value;

    if (isNewKey || (value != oldValue)) {
        fireChanged();
    }
}



bool SettingsInternal::getBool(const string& key) const {
    return myBools.at(key);
}



void SettingsInternal::setBool(const string& key, bool value) {
    bool isNewKey;
    auto oldValue = (isNewKey = myBools.count(key) == 0) ? myBools[key] : false;

    myBools[key] = value;

    if (isNewKey || (value != oldValue)) {
        fireChanged();
    }
}



void SettingsInternal::beginGroupSet() {
    myIsGroupSet = true;
}



void SettingsInternal::endGroupSet() {
    myIsGroupSet = false;
    if (myWasChangedDuringGroupSet) {
        myWasChangedDuringGroupSet = false;
        fireChanged();
    }
}



void SettingsInternal::fireChanged() {
    if (myIsGroupSet) {
        myWasChangedDuringGroupSet = true;
    }
    else {
        myChangedCb();
    }
}

}
