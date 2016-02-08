// request_group.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef REQUESTGROUP_H
#define REQUESTGROUP_H



#include <utility>



namespace application {

/**
 * @brief Sequential set of integer numbers.
 */
class RequestGroup {

public:
    /**
     * @brief Constructor.
     *
     * @param lower Lower bound - the smallest number which shall be included.  It should be >= 0 and <= @p upper.
     * @param upper Upper bound - the biggest number which shall be included.  It should be >= 0 and >= @p lower.
     */
    explicit RequestGroup(int lower, int upper);

    /**
     * @brief Constructor for empty group.
     */
    explicit RequestGroup();

    /**
     * @brief Returns true it the group is empty.
     *
     * @return bool
     */
    bool isEmpty() const;

    /**
     * @brief Gets the lower bound - the smallest member.
     *
     * @return int
     */
    int getLower() const;

    /**
     * @brief Gets the upper bound - the biggest member.
     *
     * @return int
     */
    int getUpper() const;

    /**
     * @brief Gets number of members in the group.
     *
     * @return 'getUpper() - getLower() + 1' or 0 if empty.
     */
    int getSize() const;

    /**
     * @brief Tests whether the given number is a member of the group.
     *
     * @param offset The tested member.
     * @return true if the given @p offset is a member.
     */
    bool isMember(int offset) const;

    /**
     * @brief Checks whether group can be extended so that it contains the given number.
     *
     * Group can be extended only by 1.  If the @p offset is already member or it is smaller than getLower() then the
     * group can not be extended.
     *
     * @param offset The number to which the group should be extended.
     * @return true if the group can be extended to the given @p offset.
     *
     * @sa extend()
     */
    bool canBeExtendedToOffset(int offset) const;

    /**
     * @brief Checks whether this group intersects with the @p other.
     *
     * @param other The other group.
     * @return bool
     */
    bool hasIntersection(RequestGroup other) const;


    /**
     * @brief Splits the group at the given @p offset.
     *
     * The @p offset becomes lower part of the second group.  If @p offset is not a member of this group then two empty
     * groups are returned.
     *
     * @param offset The place at which the group shall be split.
     * @return Pair of new groups which are result of the split.
     */
    std::pair<RequestGroup, RequestGroup> split(int offset) const;

    /**
     * @brief Substracts @p other group from this one.
     *
     * In case the @p other group is smaller and fully intersects with this one the substract operation can be
     * imagined as if the other group punched a hole into this one resulting to two groups (one on each side of the
     * hole).
     *
     * If the other group partially intersects with this one then one of the resulting groups will be empty.
     *
     * If the other group does not intersets with this one then the result is two empty groups.
     *
     * @param other The group which shall be substracted from this one.
     * @return Pair of new groups which are result of the substraction.
     */
    std::pair<RequestGroup, RequestGroup> substract(RequestGroup other) const;

    /**
     * @brief Makes group size bigger by 1 at the upper bound.
     *
     * @sa canBeExtendedToOffset(), getSize(), getUpper()
     */
    void extend();

    /**
     * @brief Makes group size smalled by 1 at the lower bound.
     *
     * @sa getSize(), getLower()
     */
    void shrink();

private:
    // lower bound (smallest member)
    int myLower;

    // upper bound (biggest member)
    int myUpper;
};

}



#endif // REQUESTGROUP_H
