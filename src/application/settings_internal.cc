// settings_internal.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <functional>
#include <string>

#include "application/settings_internal.h"

using namespace std;



namespace application {

// SMELL: With this approach the key (variable) can be anything - it is not checked against defined constants in .h.
SettingsInternal::SettingsInternal() {
}



void SettingsInternal::connectChanged(function<void()> callback) {
    myChangedCb = callback;
}



string SettingsInternal::getString(const string& key) const {
    if (myStrings.count(key) != 0) {
        return myStrings.at(key);
    } else {
        return "";
    }
}



void SettingsInternal::setString(const string& key, const string& value) {
    bool isNewKey;
    auto oldValue = (isNewKey = myStrings.count(key) == 0) ? myStrings[key] : "";

    myStrings[key] = value;

    if (isNewKey || (value != oldValue)) {
        fireChanged();
    }
}



int SettingsInternal::getInt(const string& key) const {
    if (myInts.count(key) != 0) {
        return myInts.at(key);
    } else {
        return 0;
    }
}



void SettingsInternal::setInt(const string& key, int value) {
    bool isNewKey;
    auto oldValue = (isNewKey = myInts.count(key) == 0) ? myInts[key] : 0;

    myInts[key] = value;

    if (isNewKey || (value != oldValue)) {
        fireChanged();
    }
}



bool SettingsInternal::getBool(const string& key) const {
    if (myBools.count(key) != 0) {
        return myBools.at(key);
    } else {
        return false;
    }
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
