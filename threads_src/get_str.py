
#!/usr/bin/python3

import pafy
import youtube_dl

def get_play_url(url):

    # url = "https://youtu.be/QzqZ1RY1seQ"
    song = pafy.new(url)
    duration = song.length
    audiostreams = song.audiostreams
    best = song.getbest()
    play_url = best.url
    # print(url + " " + play_url)

    return play_url
