// request_tracker.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef REQUESTTRACKER_H
#define REQUESTTRACKER_H



#include <unordered_set>

using namespace std;



namespace application {

class RequestTracker {
public:
    bool isRequested(int row) const;

    void addRequests(int row, int count);

    void removeRequests(int row, int count);

private:
    unordered_set<int> myRequests{};
};

}


#endif // REQUESTTRACKER_H
