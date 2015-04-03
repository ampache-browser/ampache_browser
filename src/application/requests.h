// requests.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef REQUESTS_H
#define REQUESTS_H



#include <vector>
#include "infrastructure/event.h"
#include "request_group.h"



namespace application {

class Requests {

public:
    infrastructure::Event<RequestGroup> readyToExecute{};

    void add(int offset);

    RequestGroup setFinished();

private:
    std::vector<RequestGroup> myRequestGroups;
    RequestGroup myCurrentRequestGroup = RequestGroup{};

    int findOwningGroupIdx(int offset);
    int findExtensibleGroupIdx(int offset);
};

}



#endif // REQUESTS_H
