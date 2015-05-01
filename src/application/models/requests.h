// requests.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef REQUESTS_H
#define REQUESTS_H



#include <limits>
#include <vector>
#include "infrastructure/event.h"
#include "request_group.h"
#include "request_groups.h"



namespace application {

class Requests {

public:
    explicit Requests(int granularity);

    explicit Requests();

    ~Requests();

    infrastructure::Event<RequestGroup> readyToExecute{};

    void add(int offset);

    RequestGroup setFinished();

private:
    RequestGroups* myRequestGroups;
    RequestGroup myCurrentRequestGroup = RequestGroup{};
    int myLastEnqueuedOffset = std::numeric_limits<int>::max();

};

}



#endif // REQUESTS_H
