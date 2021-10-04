
#pragma once

#define PY_SSIZE_T_CLEAN
#include <python3.9/Python.h>

#include <memory>
#include <future>

#include <vlcpp/vlc.hpp>

#include "support.hpp"
#include "video_controls.hpp"


extern std::mutex mtx;
extern unsigned int current_played;
extern Playlist current_playlist;
extern std::condition_variable cond_var;
extern bool input_sent;


std::string get_title(
    std::string song_mrl
    ) 
{
    const char *const arg[] = {"--preferred-resolution=720", "--no-video"};
    std::shared_ptr<VLC::Instance> dummy = std::make_shared<VLC::Instance>(VLC::Instance(2, arg));
    
    VLC::Media media = VLC::Media(*dummy, song_mrl, VLC::Media::FromLocation);
    media.parseWithOptions(VLC::Media::ParseFlags::Network, -1);
    
    while (media.parsedStatus() != VLC::Media::ParsedStatus::Done) { }
    
    std::string result = media.subitems()->itemAtIndex(0)->meta(libvlc_meta_Title);
    
    return result;
}


void list_tracks()
{
    const int l = current_playlist.get_length();
    
    int left = current_played - 5;
    if (left < 0) 
        left = 0;
    for (int i = left; i < current_played; i++)
    {
        std::cout << " " << i << ". " << get_title(current_playlist.get(i)) << std::endl;
    }
    
    std::cout << ">" << current_played << ". " << get_title(current_playlist.get(current_played)) << std::endl;
    
    int right = current_played + 5;
    if (right >= l)
        right = l;
    for (int i = current_played + 1; i < right; i++)
    {
        std::cout << " " << i << ". " << get_title(current_playlist.get(i)) << std::endl;
    }
}


void wait_for_duration(
    std::promise<int>&& duration_promise,
    VLC::Media&& media
)
{
    int duration = media.duration(); 
    
    while (duration <= 0)  
    {
        std::cout << "Waiting for duration...\n";
        duration = media.duration();
        std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
    }
    duration_promise.set_value(duration);
    std::cout << "Duration received, starting playback.\n"; 
    std::this_thread::sleep_for( std::chrono::milliseconds(300) );
}


std::string get_play_mrl (
    const std::string mrl
    )
{
    Py_Initialize();
    
    PyObject* py_module_name = PyUnicode_FromString((char*)"get_str");     
    PyObject* py_module      = PyImport_Import( py_module_name );  

    if (!py_module)
    {
        std::cout << "Error importing module.\n";
        return "Error";
    }

    PyObject* function = PyObject_GetAttrString(py_module, (char*)"get_play_url");  
    
    const char* mrl_c = mrl.c_str();                                                
    PyObject* args = PyTuple_Pack(1, PyUnicode_FromString(mrl_c));       
    
    PyObject* result = PyObject_CallObject(function, args);                         
    
    Py_ssize_t* size = nullptr; 
    wchar_t* play_mrl_wchar = PyUnicode_AsWideCharString(result, size); // TODO free memory
    
    std::wstring ws( play_mrl_wchar );
    std::string play_mrl_str( ws.begin(), ws.end() );
    delete [] play_mrl_wchar;
    
    Py_XDECREF(py_module_name);
    Py_XDECREF(py_module);
    Py_XDECREF(function);
    Py_XDECREF(args);
    Py_XDECREF(result);

    //Py_Finalize(); uncommenting this will result in program crash at the second iteration of the loop
    
    return play_mrl_str;
}


void play_music()
{ 
    current_playlist = Playlist("");
    std::shared_ptr<VLC::Instance> instance;
    
    #define min_logs 1
    #if min_logs
        const char *const arg[] = {"--preferred-resolution=720", "--no-video"};
        instance = std::make_shared<VLC::Instance>(VLC::Instance(2, arg));
    #else
        const char *const arg[] = {"--preferred-resolution=720", "--no-video", "-vv"};
        instance = std::make_shared<VLC::Instance>(VLC::Instance(3, arg));
    #endif
        
    std::unique_lock<std::mutex> ulm (mtx); 
    
    while (true)
    {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        cond_var.wait(
            ulm,
            []{ return (current_playlist.get_length() > 0); }
        );
        
        while(true)
        {
            input_sent = false;
            
            std::string song_mrl = current_playlist.get(current_played);
            
            VLC::Media song = VLC::Media(*instance, get_play_mrl(song_mrl), VLC::Media::FromLocation);
            
            auto player = std::make_shared<VLC::MediaPlayer>(song);
            player->play();
            
            std::promise<int> duration_promise;
            std::future<int> duration_future = duration_promise.get_future();
            std::thread duration_waiter(wait_for_duration, std::move(duration_promise), song);
            
            while (duration_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            
            std::system("clear");
            list_tracks();
            
            int duration = duration_future.get(); // hadle error when future is received
            duration_waiter.join();
            
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


