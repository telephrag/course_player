
#pragma once

#include <memory>
#include <future>

#include <vlcpp/vlc.hpp>

#include "common.hpp"


std::shared_ptr<VLC::Media> parse_into_media(
    const std::string mrl,
    std::shared_ptr<VLC::Instance> instance 
    )
{
    std::unique_lock<std::mutex> ul (mtx);
    
    VLC::Media song = VLC::Media(*instance, mrl, VLC::Media::FromLocation);
    
    song.parseWithOptions(VLC::Media::ParseFlags::Network, -1); // TODO: Code handler for when parsing failed
    
    while (song.parsedStatus() != VLC::Media::ParsedStatus::Done)
    {
        if (song.parsedStatus() == VLC::Media::ParsedStatus::Failed)
            std::cout << "Parsing failed. Trying again.\n";
    }
    
    std::shared_ptr<VLC::Media> result = song.subitems()->itemAtIndex(0);
    
    return result;

}


void play_music(
    std::shared_ptr<VLC::MediaPlayer> player,
    std::shared_ptr<VLC::Instance> instance
)
{ 
    // TODO: Make on thread and reuse it for both getting new plalist and parsing next songs?
    // NOTE: Async plalist retrieval might not be needed if I decide to stop music when user decide to switch playlist
    auto playlist_future = std::async(std::launch::async, support, "sample_playlist.txt");
    Playlist playlist = playlist_future.get();
    playlist.parse_file_with_urls();
    playlist.print();
    
    std::vector<std::string> song_vector = playlist.get_result();
    
    for (auto song_mrl : song_vector)
    {
        std::shared_ptr<VLC::Media> parsed_song = parse_into_media(song_mrl, instance);
        player->setMedia(*parsed_song);
        player->play();
        
        
        std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
        int duration = parsed_song->duration();
        std::this_thread::sleep_for( std::chrono::milliseconds(duration) );
        std::cout << "Parsed song with the duration: " << duration << "ms \n";
    }
    
    /*
     Consider: 
     After parsing the media make the player play in a separate thread via async.
     Then define user controls in a parent thread. For example kill thread if you want to stop playback.
     */
}


