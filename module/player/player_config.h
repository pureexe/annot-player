#ifndef PLAYER_CONFIG_H
#define PLAYER_CONFIG_H

// player_config.h
// 7/30/2011

#include <QtGlobal>

//#define DEBUG

#define PLAYER_FORMAT_VIDEO(...) \
    __VA_ARGS__ "avi" \
    __VA_ARGS__ "mkv" \
    __VA_ARGS__ "mp4" \
    __VA_ARGS__ "mpeg" \
    __VA_ARGS__ "mpg" \
    __VA_ARGS__ "rm" \
    __VA_ARGS__ "rmvb" \
    __VA_ARGS__ "flv"

#define PLAYER_FORMAT_AUDIO(...) \
    __VA_ARGS__ "m4a" \
    __VA_ARGS__ "m4r" \
    __VA_ARGS__ "mp3" \
    __VA_ARGS__ "mp4"

#define PLAYER_FORMAT_SUBTITLE(...) \
    __VA_ARGS__ "ass" \
    __VA_ARGS__ "ssa"


#endif // PLAYER_CONFIG_H
