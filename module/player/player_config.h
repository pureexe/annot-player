#ifndef PLAYER_CONFIG_H
#define PLAYER_CONFIG_H

// player_config.h
// 7/30/2011

//#define DEBUG

#define PLAYER_FORMAT_IMAGE(...) \
    __VA_ARGS__ "bin" \
    __VA_ARGS__ "iso" \
    __VA_ARGS__ "img" \
    __VA_ARGS__ "mdf"

#define PLAYER_FORMAT_VIDEO(...) \
    __VA_ARGS__ "3gp" \
    __VA_ARGS__ "asf" \
    __VA_ARGS__ "avi" \
    __VA_ARGS__ "axv" \
    __VA_ARGS__ "dixv" \
    __VA_ARGS__ "dv" \
    __VA_ARGS__ "flv" \
    __VA_ARGS__ "gxf" \
    __VA_ARGS__ "m1v" \
    __VA_ARGS__ "m2p" \
    __VA_ARGS__ "m2ts" \
    __VA_ARGS__ "m2v" \
    __VA_ARGS__ "m4v" \
    __VA_ARGS__ "mks" \
    __VA_ARGS__ "mkv" \
    __VA_ARGS__ "mov" \
    __VA_ARGS__ "moov" \
    __VA_ARGS__ "mp4" \
    __VA_ARGS__ "mpeg" \
    __VA_ARGS__ "mpeg1" \
    __VA_ARGS__ "mpeg2" \
    __VA_ARGS__ "mpeg4" \
    __VA_ARGS__ "mpg" \
    __VA_ARGS__ "mpv" \
    __VA_ARGS__ "mt2s" \
    __VA_ARGS__ "mts" \
    __VA_ARGS__ "mxf" \
    __VA_ARGS__ "nsv" \
    __VA_ARGS__ "nuv" \
    __VA_ARGS__ "ogm" \
    __VA_ARGS__ "ogv" \
    __VA_ARGS__ "ps" \
    __VA_ARGS__ "qt" \
    __VA_ARGS__ "ram" \
    __VA_ARGS__ "rm" \
    __VA_ARGS__ "rmvb" \
    __VA_ARGS__ "tod" \
    __VA_ARGS__ "vro" \
    __VA_ARGS__ "vob" \
    __VA_ARGS__ "webm" \
    __VA_ARGS__ "wm" \
    __VA_ARGS__ "wmv" \
    PLAYER_FORMAT_IMAGE(__VA_ARGS__)

#define PLAYER_FORMAT_AUDIO(...) \
    __VA_ARGS__ "a52" \
    __VA_ARGS__ "aac" \
    __VA_ARGS__ "ac3" \
    __VA_ARGS__ "aiff" \
    __VA_ARGS__ "amr" \
    __VA_ARGS__ "aob" \
    __VA_ARGS__ "asx" \
    __VA_ARGS__ "ape" \
    __VA_ARGS__ "axa" \
    __VA_ARGS__ "dts" \
    __VA_ARGS__ "flac" \
    __VA_ARGS__ "it" \
    __VA_ARGS__ "m2a" \
    __VA_ARGS__ "m4a" \
    __VA_ARGS__ "m4r" \
    __VA_ARGS__ "mid" \
    __VA_ARGS__ "mlp" \
    __VA_ARGS__ "mod" \
    __VA_ARGS__ "mp1" \
    __VA_ARGS__ "mp2" \
    __VA_ARGS__ "mp3" \
    __VA_ARGS__ "mpa" \
    __VA_ARGS__ "mpc" \
    __VA_ARGS__ "mka" \
    __VA_ARGS__ "oma" \
    __VA_ARGS__ "oga" \
    __VA_ARGS__ "ogg" \
    __VA_ARGS__ "ogx" \
    __VA_ARGS__ "rec" \
    __VA_ARGS__ "rmi" \
    __VA_ARGS__ "s3m" \
    __VA_ARGS__ "spx" \
    __VA_ARGS__ "tta" \
    __VA_ARGS__ "vqf" \
    __VA_ARGS__ "voc" \
    __VA_ARGS__ "wav" \
    __VA_ARGS__ "wma" \
    __VA_ARGS__ "wv" \
    __VA_ARGS__ "xm" \
    PLAYER_FORMAT_IMAGE(__VA_ARGS__)

#define PLAYER_FORMAT_PICTURE(...) \
    __VA_ARGS__ "bmp" \
    __VA_ARGS__ "cdg" \
    __VA_ARGS__ "idx" \
    __VA_ARGS__ "gif" \
    __VA_ARGS__ "jpeg" \
    __VA_ARGS__ "jpg" \
    __VA_ARGS__ "png" \
    __VA_ARGS__ "tiff"

#define PLAYER_FORMAT_SUBTITLE(...) \
    __VA_ARGS__ "aqt" \
    __VA_ARGS__ "ass" \
    __VA_ARGS__ "jss" \
    __VA_ARGS__ "mpsub" \
    __VA_ARGS__ "pjs" \
    __VA_ARGS__ "psb" \
    __VA_ARGS__ "rt" \
    __VA_ARGS__ "smi" \
    __VA_ARGS__ "srt" \
    __VA_ARGS__ "ssa" \
    __VA_ARGS__ "sub" \
    __VA_ARGS__ "usf" \
    __VA_ARGS__ "utf"

#endif // PLAYER_CONFIG_H
