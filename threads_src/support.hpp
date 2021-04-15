
#pragma once

#include <memory>
#include "playlist.hpp"

Playlist support(std::string location)
{
    auto playlist = Playlist(location);
    
    playlist.parse_file_with_urls();
    playlist.print();
    std::cout << "\n";
    
    return playlist;
}
