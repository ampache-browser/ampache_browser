// album_data.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský




#ifndef ALBUMDATA_H
#define ALBUMDATA_H



#include <string>
#include <memory>
#include "domain/album.h"



namespace data {

class AlbumData {

public:
    explicit AlbumData(const std::string& id, const std::string& artUrl, const std::string& artistId,
        std::unique_ptr<domain::Album> album);

    AlbumData(const AlbumData& other) = delete;

    AlbumData& operator=(const AlbumData& other) = delete;

    std::string getId() const;

    std::string getArtUrl() const;

    std::string getArtistId() const;

    domain::Album& getAlbum() const;

private:
    const std::string myId;
    const std::string myArtUrl;
    const std::string myArtistId;
    const std::unique_ptr<domain::Album> myAlbum;
};

}



#endif // ALBUMDATA_H
