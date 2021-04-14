
#pragma once

#include <memory>
#include <future>

#include <vlcpp/vlc.hpp>

#include "common.hpp"
#include "support.hpp"
#include "video_controls.hpp"

extern std::mutex mtx;
extern int current_played;
extern std::condition_variable cond_var;

std::shared_ptr<VLC::Media> parse_into_media(
    const std::string mrl,
    std::shared_ptr<VLC::Instance> instance 
    )
{
    //std::unique_lock<std::mutex> ul (mtx);
    
    VLC::Media song = VLC::Media(*instance, mrl, VLC::Media::FromLocation);
    
    song.parseWithOptions(VLC::Media::ParseFlags::Network, -1); 
    
    while (song.parsedStatus() != VLC::Media::ParsedStatus::Done)
    {
        if (song.parsedStatus() == VLC::Media::ParsedStatus::Failed)
            std::cout << "Parsing failed. Trying again.\n";
    }
    
    std::shared_ptr<VLC::Media> result = song.subitems()->itemAtIndex(0);
    
    return result;

}


void play_music()
{ 
    // NOTE: Async plalist retrieval might not be needed if I decide to stop music when user decide to switch playlist
    auto playlist_future = std::async(std::launch::async, support, "sample_playlist.txt");
    Playlist playlist = playlist_future.get();
    playlist.parse_file_with_urls();
    playlist.print();
    
    std::vector<std::string> song_vector = playlist.get_result();
     
    std::unique_lock<std::mutex> ulm (mtx); // NOTE Do I need a lock here?
    while(true)
    {
        std::string song_mrl = song_vector.at(current_played);
        std::shared_ptr<VLC::Media> parsed_song = parse_into_media(song_mrl, instance);
        player->setMedia(*parsed_song);
        player->play();
        
        std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
        int duration = parsed_song->duration();
        cond_var.wait_for(
            ulm,
            std::chrono::milliseconds(duration),
            []{ return input_sent; }
        );
        if (input_sent) 
        {
            input_sent = false;
            handle_input();
            current_input = "";
            continue;
        }
        
        current_played++;
    }
}


