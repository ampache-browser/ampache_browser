// selected_items.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef SELECTEDITEMS_H
#define SELECTEDITEMS_H



#include <string>
#include <vector>



namespace ui {

/**
 * @brief Container for selected artists, albums and tracks.
 */
class SelectedItems {

public:
    /**
     * @brief Constructor.
     *
     * @param artists Selected artist IDs.
     * @param albums Selected album IDs.
     * @param tracks Selected track IDs.
     */
    explicit SelectedItems(const std::vector<std::string>& artists, const std::vector<std::string>& albums,
        const std::vector<std::string>& tracks);

    /**
     * @brief Constructor.
     *
     * Creates instance with no selected items.
     */
    explicit SelectedItems();

    SelectedItems(const SelectedItems& other) = delete;

    SelectedItems& operator=(SelectedItems& other) = delete;

    SelectedItems(SelectedItems&& other);

    SelectedItems& operator=(SelectedItems&& other);

    /**
     * @brief Gets selected artist IDs.
     */
    std::vector<std::string> getArtists() const;

    /**
     * @brief Gets selected album IDs.
     */
    std::vector<std::string> getAlbums() const;

    /**
     * @brief Gets selected track IDs.
     */
    std::vector<std::string> getTracks() const;

private:
    // arguments from the constructor
    std::vector<std::string> myArtists{};
    std::vector<std::string> myAlbums{};
    std::vector<std::string> myTracks{};
};

}



#endif // SELECTEDITEMS_H
