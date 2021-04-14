
#include <iostream>
#include <thread>

#include <vlc/vlc.h>
#include <vlcpp/vlc.hpp>

#include "threads_src/support.hpp"
#include "threads_src/play.hpp"
#include "threads_src/video_controls.hpp"
#include "threads_src/common.hpp"

extern std::mutex mtx;
extern std::condition_variable cond_var;

extern std::map<std::string, Input> input_map;

extern std::shared_ptr<VLC::Instance> instance;
extern std::shared_ptr<VLC::MediaPlayer> player; 
// TODO: Make playlist variable global.


void init_input_map()
{
    input_map = {
        {"next", Input::next}, 
        {"prev", Input::prev},
        {"stop", Input::stop},
    {"playlist", Input::playlist}
    };
}


int main() {
    init_input_map();
    std::string location = "sample_playlist.txt";
    
    // Player initialization
#define custom_logs 1
#if custom_logs
    const char *const arg[] = {"--preferred-resolution=720", "--no-video", "--loop"};
    instance = std::make_shared<VLC::Instance>(VLC::Instance(3, arg));
#else
    const char *const arg[] = {"--preferred-resolution=720", "--no-video", "-vv", "--loop"};
    instance = std::make_shared<VLC::Instance>(VLC::Instance(4, arg));
#endif
    
    player = std::make_shared<VLC::MediaPlayer>(VLC::MediaPlayer(*instance));
    std::thread play_thread(play_music); 
    std::thread video_control(listen_for_input);
    // TODO: Figure out how to pass new playlist into this thread. Condition varialbe?
    // TODO: Restructure project directory.
    // TODO: Create a "makeshift" user controls.
    // TODO: Encapsulate player and playlist into a single class?
    
    // Cleaning up after ending the programm
    play_thread.join();
    std::cin.get();
    
    return 0;
}
