
#pragma once

#include <memory>
#include <future>

#include <vlcpp/vlc.hpp>

#include "common.hpp"
#include "support.hpp"
#include "video_controls.hpp"

extern std::mutex mtx;
extern unsigned int current_played;
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
        std::this_thread::sleep_for( std::chrono::seconds(2) );
    }
    
    std::shared_ptr<VLC::Media> result = song.subitems()->itemAtIndex(0);
    
    return result;

}


void play_music()
{ 
    // NOTE: Async plalist retrieval might not be needed if I decide to stop music when user decide to switch playlist
//     current_playlist = support("sample_playlist");
//     current_playlist.parse_file_with_urls();
//     current_playlist.print();
//     
//     std::vector<std::string> song_vector;
     
    std::unique_lock<std::mutex> ulm (mtx); // NOTE Do I need a lock here? UPD: Ok, I'll just leave it here.
    
    while (true)
    {
        std::this_thread::sleep_for (std::chrono::seconds(1) );
        
        cond_var.wait(
            ulm,
            []{ return (current_playlist.get_length() > 0); }
        );
        
        while(true)
        {
            std::cerr << "Curerntly played: " << current_played << "\n";

            std::string song_mrl = current_playlist.get(current_played);
            std::shared_ptr<VLC::Media> parsed_song = parse_into_media(song_mrl, instance);
            player->setMedia(*parsed_song);
            player->play();

            std::this_thread::sleep_for( std::chrono::seconds(1) );

            int duration = parsed_song->duration();
            cond_var.wait_for(
                ulm,
                std::chrono::milliseconds(duration),
                [] { return input_sent; }
            );

            if (input_sent)
            {
                input_sent = false;
                current_input = "";
                continue;
            }
            else
            {
                next_track();
            }
        }
    }
}


