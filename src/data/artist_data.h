// artist_data.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ARTISTDATA_H
#define ARTISTDATA_H



#include <string>
#include <memory>
#include "domain/artist.h"



namespace data {

class ArtistData {

public:
    explicit ArtistData(const std::string& id, std::unique_ptr<domain::Artist> artist);

    std::string getId() const;

    domain::Artist& getArtist() const;

private:
    const std::string myId;
    const std::unique_ptr<domain::Artist> myArtist;
};

}



#endif // ARTISTDATA_H
