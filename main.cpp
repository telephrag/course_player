
#include <iostream>
#include <thread>

#include <vlc/vlc.h>
#include <vlcpp/vlc.hpp>

#include "threads_src/playlist/support.hpp"
#include "threads_src/play/play.hpp"

std::mutex m;

std::shared_ptr<VLC::Instance> instance;
std::shared_ptr<VLC::MediaPlayer> player;

int main() {
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
    std::thread play_thread(play_music, player, instance); 
    // TODO: Figure out how to pass new playlist into this thread. Condition varialbe?
    
    // Cleaning up after ending the programm
    play_thread.join();
    std::cin.get();
    
    return 0;
}
