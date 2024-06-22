// request_groups.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#ifndef REQUESTGROUPS_H
#define REQUESTGROUPS_H



#include <vector>

#include "request_group.h"



namespace application {

/**
 * @brief Set of size bounded RequestGroup() objects.
 */
class RequestGroups {

public:
    /**
     * @brief Constructor.
     *
     * @param granularity The maximal size of member groups.  0 if unlimited.
     */
    explicit RequestGroups(int granularity);

    RequestGroups(const RequestGroups& other) = delete;

    RequestGroups& operator=(const RequestGroups& other) = delete;

    /**
     * @brief Returns true if the collection is empty.
     */
    bool isEmpty() const;


    /**
     * @brief Cuts @p requestGroup from the set.
     *
     * The operation modifies/removes affected groups as needed.  For example if @p requestGroup is big, spreading
     * over (intersecting with) several groups in the set, inner intersecting groups are removed and groups
     * intersecting on bounds are shrinked accordingly.
     *
     * @note The entire set is then modified so that no group is bigger than the set granularity.
     *
     * @param requestGroup Group that shall be cut.
     */
    void cut(RequestGroup requestGroup);

    /**
     * @brief Cuts @p requestGroup from the set and places it on top.
     *
     * @note The entire set is then modified so that no group is bigger than the set granularity.
     *
     * @param requestGroup The group that shall be cut and moved.
     *
     * @sa cut()
     */
    void moveOnTop(RequestGroup requestGroup);

    /**
     * @brief Make grup determined by @p offset bigger by one.
     *
     * Another group may be shrinked in order to prevent overlapping.
     *
     * @param offset Group extensible to this offset shall be extended.
     * @return true if a group was extended, false no group extensible to the @p offset was found.
     */
    bool extend(int offset);

    /**
     * @brief Removes and returns group from top of the set.
     *
     * @return application::RequestGroup
     *
     * @sa cutAndPlaceOnTop()
     */
    RequestGroup pop();

    /**
     * @brief Removes all groups
     */
    void clear();

private:
    // argument from the constructor
    const int myGranularity;

    // stored groups
    std::vector<RequestGroup> myRequestGroups;

    void cutRequestGroup(RequestGroup requestGroup);
    void chop();
    void appendOnTop(std::vector<RequestGroup>& groups, RequestGroup groupToPlace);
    int findOwningGroupIdx(int offset) const;
    int findExtensibleGroupIdx(int offset) const;
    std::vector<int> findIntersectingGroupIdxs(RequestGroup requestGroup) const;
};

}



#endif // REQUESTGROUPS_H
