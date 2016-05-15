// name_filter_for_albums.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef NAMEFILTERFORALBUMS_H
#define NAMEFILTERFORALBUMS_H



#include <vector>
#include <memory>
#include "filter.h"



namespace data {

class AlbumData;



/**
 * @brief Provides albums data filtered by name.
 *
 * Filter is making exact, case insensitive match of the album's name (domain::Album::getName()) with the given pattern.
 */
class NameFilterForAlbums: public Filter<AlbumData> {

public:
    /**
     * @brief Constructor.
     *
     * @param namePattern The pattern that shall be used to match the album's name.
     */
    explicit NameFilterForAlbums(const std::string& namePattern);

    void processUpdatedSourceData(int offset = -1, int length = -1) override;

private:
    const std::string myNamePattern;
};

}



#endif // NAMEFILTERFORALBUMS_H
