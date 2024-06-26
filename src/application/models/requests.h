// requests.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#ifndef REQUESTS_H
#define REQUESTS_H



#include <limits>
#include <memory>
#include "infrastructure/event/event.h"
#include "request_group.h"
#include "request_groups.h"



namespace application {

/**
 * @brief Manages requests for an arbitrary operation.
 *
 * Requests are represented by integer numbers needs to be added via add() method.  These are arranged into groups
 * of a size specified as @p granularity parameter to constructor.  When enough requests are addded so that they form
 * a full group or no operation is currently being executed the event ::readyToExecute is fired.  Consumer should
 * execute whatever (asynchronous) operation for data mapped by the numbers in the ready group.  Once the operation
 * is finished the consumer has to call setFinished() method.
 *
 * More recently an operation was added higher priority it gets.  Same operation can be added multiple times however
 * it will be executed only once (it affects only its priority).
 */
class Requests {

public:
    /**
     * @brief Constructor.
     *
     * @param granularity The maximal size of request group.  0 if unlimited.
     */
    explicit Requests(int granularity);

    /**
     * @brief Parameterless constructor.
     *
     * Creates instance with unlimited granularity.
     */
    explicit Requests();

    Requests(const Requests& other) = delete;

    Requests& operator=(const Requests& other) = delete;

    // SMELL: Not necesary to expose RequestGroup.  Use just pair<> as parameter and Requests will be the only
    // "interface" class to request handling.  Same for readyToExecute event.
    /**
     * @brief A group of requests is ready to be executed.
     *
     * Consumer should start an (asynchronous) operation and call setFinished() once it finishes.
     *
     * @sa setFinished()
     */
    infrastructure::Event<RequestGroup> readyToExecute{};

    /**
     * @brief Adds a request.
     *
     * @param offset A number that represens the request.
     */
    void add(int offset);

    /**
     * @brief Remove all requests.
     *
     * @sa add()
     */
    void removeAll();

    /**
     * @brief Inform the instance that the operation started upon ::readyToExecute event has finished.
     *
     * Ready to execute requests which are not reported as finished will be set as finished (will not be
     * executed again).  Finished requests which were not ready to execute will also be set as finished.
     *
     * @param offset The number of first finished request (may be different from what was originally requested).
     * @param count The count of finished requests (may be different from what was originally requested).
     *
     * @sa ::readyToExecute
     */
    void setFinished(int offset, int count);

    /**
     * @brief Returns true if some operation is being executed.
     *
     * true is returned in case ::readyToExecute was fired but setFinished() was not yet called.
     *
     * @return bool
     */
    bool isInProgress() const;

private:
    // stores request groups
    const std::unique_ptr<RequestGroups> myRequestGroups;

    // group that is currently being executed
    RequestGroup myCurrentRequestGroup = RequestGroup{};

    // operation number which was included into myRequestGroups most recently
    int myLastEnqueuedOffset = std::numeric_limits<int>::max();
};

}



#endif // REQUESTS_H
