
#pragma once

#include "common_sqlite.hpp"
#include "Session.hpp"

enum CommandsLogged
{
    logout, 
    add_song,
    snatch_song,
    del_song, 
    print_songs, 
    print_all_songs,
    make_playlist,
    del_playlist,
    print_playlist,
    print_playlists,
    add_to_playlist,
    del_from_playlist,
    get_playlist,
    del_user,
    logout_and_quit
};

std::map<std::string, CommandsLogged> commandMapLogged = {
        { "logout",          CommandsLogged::logout            },
        { "addsong",         CommandsLogged::add_song          },
        { "snatchsong",      CommandsLogged::snatch_song       },
        { "delsong",         CommandsLogged::del_song          },
        { "printsongs",      CommandsLogged::print_songs       },
        { "printallsongs",   CommandsLogged::print_all_songs   },
        { "makeplaylist",    CommandsLogged::make_playlist     },
        { "delplaylist",     CommandsLogged::del_playlist      },
        { "printplaylist",   CommandsLogged::print_playlist    },
        { "printplaylists",  CommandsLogged::print_playlists   },
        { "addtoplaylist",   CommandsLogged::add_to_playlist   },
        { "delfromplaylist", CommandsLogged::del_from_playlist },
        { "getplaylist",     CommandsLogged::get_playlist      },
        { "deluser",         CommandsLogged::del_user          },
        { "quit",            CommandsLogged::logout_and_quit   }
    };


int logged(Session &session)
{
    if (!session.validate())
    {
        std::cout << "Invalid login credentials.\n";
        return 0;
    }
    
    for(;;)
    {
        std::cout << "> ";
        std::string command = "";
        std::cin >> command;

        try {
            CommandsLogged c = commandMapLogged.at(command);
        }
        catch (std::out_of_range)
        {
            std::cout << "No such command.\n";
            continue;
        }

        switch(commandMapLogged[command])
        {
            case add_song:
                session.add_song();
                break;
            
            case snatch_song:
                session.snatch_song();
                break;
                
            case del_song:
                session.del_song();
                break;
                
            case print_songs:
                session.print_songs();
                break;
                
            case print_all_songs:
                session.print_all_songs();
                break;
                
            case make_playlist:
                session.make_playlist();
                break;
                
            case del_playlist:
                session.del_playlist();
                break;
                
            case print_playlist:
                session.print_playlist();
                break;
                
            case print_playlists:
                session.print_playlists();
                break;
                
            case add_to_playlist:
                session.add_to_playlist();
                break;
                
            case del_from_playlist:
                session.del_from_playlist();
                break;
                
            case get_playlist:
                session.set_playlist();
                break;
                
            case del_user:
                session.del_user();
                session.~Session();
                return 0;
                break;
            
            case logout:
                session.~Session();
                return 0;
                break;
                
            case logout_and_quit:
                session.~Session();
                return 1;
                break;

            default:
                break;
        }
    }
}
