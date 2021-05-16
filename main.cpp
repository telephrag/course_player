
#include <iostream>
#include <thread>

#include <vlc/vlc.h>
#include <vlcpp/vlc.hpp>

#include "threads_src/support.hpp"
#include "threads_src/play.hpp"
#include "threads_src/video_controls.hpp"
#include "threads_src/common.hpp"

extern std::shared_ptr<VLC::Instance> instance; 
extern std::shared_ptr<VLC::MediaPlayer> player; 
// TODO: Make playlist variable global.
    
    
int main() {
    std::string location = "sample_playlist.txt";
    
#define custom_logs 1
#if custom_logs
    const char *const arg[] = {"--preferred-resolution=720", "--no-video", "--loop"};
    instance = std::make_shared<VLC::Instance>(VLC::Instance(3, arg));
#else
    const char *const arg[] = {"--preferred-resolution=720", "--no-video", "-vv", "--loop"};
    instance = std::make_shared<VLC::Instance>(VLC::Instance(4, arg));
#endif
    
    current_playlist = Playlist("");
    player = std::make_shared<VLC::MediaPlayer>(VLC::MediaPlayer(*instance));
    std::thread play_thread(play_music); 
    std::thread control_thread(listen_for_input);
    
    // TODO: Encapsulate player and playlist into a single class?
    
    play_thread.join();
    std::cin.get();
    
    return 0;
}
