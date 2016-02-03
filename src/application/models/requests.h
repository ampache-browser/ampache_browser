// requests.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef REQUESTS_H
#define REQUESTS_H



#include <limits>
#include <vector>
#include <memory>
#include "infrastructure/event/event.h"
#include "request_group.h"
#include "request_groups.h"



namespace application {

class Requests {

public:
    explicit Requests(int granularity);

    explicit Requests();

    Requests(const Requests& other) = delete;

    Requests& operator=(const Requests& other) = delete;

    infrastructure::Event<RequestGroup> readyToExecute{};

    void add(int offset);

    void cancel();

    RequestGroup setFinished();

    bool isInProgress() const;

private:
    const std::unique_ptr<RequestGroups> myRequestGroups;
    RequestGroup myCurrentRequestGroup = RequestGroup{};
    int myLastEnqueuedOffset = std::numeric_limits<int>::max();

};

}



#endif // REQUESTS_H
