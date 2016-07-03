// artist.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTIST_H
#define ARTIST_H



#include <string>



namespace domain {

/**
 * @brief Represents the artist domain object.
 */
class Artist {

public:
    /**
     * @brief Constructor.
     *
     * @param id Identifier.
     * @param name Artist's name.
     */
    Artist(const std::string& id, const std::string& name);

    Artist(const Artist& other) = delete;

    Artist& operator=(const Artist& other) = delete;

    /**
     * @brief Gets the identifier.
     */
    const std::string getId() const;

    /**
     * @brief Gets artist's name.
     */
    const std::string getName() const;

private:
    // arguments from the constructor
    const std::string myId;
    const std::string myName;
};

bool operator==(const Artist& lhs, const Artist& rhs);

bool operator!=(const Artist& lhs, const Artist& rhs);

bool operator<(const Artist& lhs, const Artist& rhs);

bool operator>(const Artist& lhs, const Artist& rhs);

bool operator<=(const Artist& lhs, const Artist& rhs);

bool operator>=(const Artist& lhs, const Artist& rhs);

}



namespace std {

template<>
class hash<domain::Artist> {

public:
    size_t operator()(const domain::Artist& artist) const;
};

}



#endif // ARTIST_H
