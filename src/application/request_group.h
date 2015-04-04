// request_group.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef REQUESTGROUP_H
#define REQUESTGROUP_H



#include <utility>



namespace application {

class RequestGroup {

public:
    explicit RequestGroup(int lower, int upper);

    explicit RequestGroup();

    bool isEmpty() const;

    int getLower() const;

    int getUpper() const;

    int getSize() const;

    bool isMember(int offset) const;

    bool canBeExtendedToOffset(int offset) const;

    bool hasIntersection(RequestGroup other) const;

    std::pair<RequestGroup, RequestGroup> split(int offset) const;

    std::pair<RequestGroup, RequestGroup> substract(RequestGroup other) const;

    void extend();

    void shrink();

private:
    int myLower;

    int myUpper;
};

}



#endif // REQUESTGROUP_H
