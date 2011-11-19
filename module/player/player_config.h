#ifndef PLAYER_CONFIG_H
#define PLAYER_CONFIG_H

// player_config.h
// 7/30/2011

//#define DEBUG

#define PLAYER_FORMAT_VIDEO(...) \
    __VA_ARGS__ "avi" \
    __VA_ARGS__ "mkv" \
    __VA_ARGS__ "mp4" \
    __VA_ARGS__ "mpeg" \
    __VA_ARGS__ "mpg" \
    __VA_ARGS__ "ram" \
    __VA_ARGS__ "rm" \
    __VA_ARGS__ "rmvb" \
    __VA_ARGS__ "flv" \
    __VA_ARGS__ "vob" \
    __VA_ARGS__ "wmv"

#define PLAYER_FORMAT_AUDIO(...) \
    __VA_ARGS__ "ape" \
    __VA_ARGS__ "flac" \
    __VA_ARGS__ "m4a" \
    __VA_ARGS__ "m4r" \
    __VA_ARGS__ "mp3" \
    __VA_ARGS__ "tta" \
    __VA_ARGS__ "wav" \
    __VA_ARGS__ "wma"

#define PLAYER_FORMAT_PICTURE(...) \
    __VA_ARGS__ "bmp" \
    __VA_ARGS__ "gif" \
    __VA_ARGS__ "jpeg" \
    __VA_ARGS__ "jpg" \
    __VA_ARGS__ "png" \
    __VA_ARGS__ "tiff"

#define PLAYER_FORMAT_SUBTITLE(...) \
    __VA_ARGS__ "ass" \
    __VA_ARGS__ "srt" \
    __VA_ARGS__ "ssa" \
    __VA_ARGS__ "sub"

#endif // PLAYER_CONFIG_H
