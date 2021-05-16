
#pragma once

#include <memory>
#include <future>

#include <vlcpp/vlc.hpp>

#include "support.hpp"
#include "video_controls.hpp"

extern std::mutex mtx;
extern unsigned int current_played;
extern std::condition_variable cond_var;
extern std::shared_ptr<VLC::Instance> instance;

std::shared_ptr<VLC::Media> parse_into_media(
    const std::string mrl,
    std::shared_ptr<VLC::Instance> instance 
    )
{
//     std::unique_lock<std::mutex> ul (mtx);
    
    VLC::Media song = VLC::Media(*instance, mrl, VLC::Media::FromLocation);
    
    song.parseWithOptions(VLC::Media::ParseFlags::Network, -1);
    
    while (song.parsedStatus() != VLC::Media::ParsedStatus::Done)
    {
        if (song.parsedStatus() == VLC::Media::ParsedStatus::Failed)
            std::cout << "Parsing failed. Trying again.\n";
        std::cout << "Parsing...\n";
        std::this_thread::sleep_for( std::chrono::seconds(2) );
    }
    
    std::shared_ptr<VLC::Media> result = song.subitems()->itemAtIndex(0);
    
    return result;

}


void play_music()
{ 
    std::unique_lock<std::mutex> ulm (mtx); 
    
    while (true)
    {
        std::this_thread::sleep_for (std::chrono::seconds(1) );
        
        cond_var.wait(
            ulm,
            []{ return (current_playlist.get_length() > 0); }
        );
        
        while(true)
        {
            std::cout << "Curerntly playing: " << current_played << "\n";

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
                continue;
            }

            next_track();
        }
    }
}


