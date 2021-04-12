
#pragma once

#include "common.hpp"

extern std::map<std::string, Input> input_map;
extern std::shared_ptr<VLC::MediaPlayer> player;

void next_track();
void prev_track();
void stop_playback();
void set_playlist();

void listen_for_input()
{
    
    for (;;)
    {
        std::string input = "";
        std::cin >> input;
        
        switch (input_map[input])
        {
            case next:
                next_track();
            case prev:
                prev_track();
            case stop:
                stop_playback();
            case playlist:
                set_playlist();
        }
    }
}

void next_track()
{
}

void prev_track()
{
}

void stop_playback()
{
    player->stop();
}

void set_playlist()
{
}

