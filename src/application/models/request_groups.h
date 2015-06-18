// request_groups.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef REQUESTGROUPS_H
#define REQUESTGROUPS_H



class RequestGroup;

#include <vector>



namespace application {

class RequestGroups {

public:
    explicit RequestGroups(int granularity);

    bool isEmpty() const;

    void cutAndPlaceOnTop(RequestGroup requestGroup);

    bool extend(int offset);

    RequestGroup pop();

private:
    const int myGranularity;

    std::vector<RequestGroup> myRequestGroups;

    void chop();

    void appendOnTop(std::vector<RequestGroup>& groups, const RequestGroup groupToPlace);

    int findOwningGroupIdx(int offset) const;

    int findExtensibleGroupIdx(int offset) const;

    std::vector<int> findIntersectingGroupIdxs(RequestGroup requestGroup) const;
};

}



#endif // REQUESTGROUPS_H