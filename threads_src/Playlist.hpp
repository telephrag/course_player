
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class Playlist // TODO: Maybe I should use json 
{
private:
    
    std::vector<std::string> url_vector;
    std::string location = ""; // NOTE: File with URL must be placed into executable directory (build in this case)

public:
    
    Playlist(std::string location = "")
    {
        this->location = location;
    }
    
    void ser_url_list(std::vector<std::string> url_vector)
    {
        this->url_vector = url_vector;
    }

    
    void parse_file_with_urls()
    {
        if (location.length() == 0)
            std::cout << "No file location was specified. Aborting.\n";
        
        std::ifstream file;
        file.open(location, std::ios::in);
        
        if (!file)
        {
            std::cout << "Error opening file. Abborting. \n";
            return;
        }
        
        std::string temp = "";
        while (!file.eof())
        {
            std::getline(file, temp);
            url_vector.push_back(temp);
        }
        
        url_vector.pop_back(); // poping back last element which is empty string
    }
    
    
    std::string get(unsigned int index)
    {
        return url_vector.at(index);
    }
    
    
    int get_length()
    {
        return url_vector.size();
    }
    
    
    void print()
    {
        for (auto song: this->url_vector)
        {
            std::cout << song << "\n";
        }
    }
    
    
    std::vector<std::string> get_result()
    {
        return this->url_vector;
    }
    
    
    void clear()
    {
        this->url_vector.clear();
    }
};
