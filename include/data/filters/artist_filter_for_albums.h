// artist_filter_for_albums.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTISTFILTERFORALBUMS_H
#define ARTISTFILTERFORALBUMS_H



#include <vector>
#include <memory>
#include "filter.h"



namespace domain {
class Artist;
}



namespace data {

class AlbumData;
class Indices;



/**
 * @brief Provides albums data filtered by artists.
 */
class ArtistFilterForAlbums: public Filter<AlbumData> {

public:
    /**
     * @brief Constructor.
     *
     * @param artists Artists by which the albums shall be filtered.
     * @param indices Access to the data that shall be filtered.
     */
    explicit ArtistFilterForAlbums(const std::vector<std::reference_wrapper<const domain::Artist>>& artists,
        Indices& indices);

    ~ArtistFilterForAlbums() override;

    void setSourceData(const std::vector<std::unique_ptr<AlbumData>>& sourceData) override;

    void processUpdatedSourceData(int offset = -1, int length = -1) override;

private:
    // arguments from the constructor
    const std::vector<std::reference_wrapper<const domain::Artist>> myArtists;
    Indices& myIndices;

    void onArtistAlbumsUpdated(const std::vector<std::reference_wrapper<const domain::Artist>>& updatedArtists);

    void processUpdatedIndices();
};

}



#endif // ARTISTFILTERFORALBUMS_H
