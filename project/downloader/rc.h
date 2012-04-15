#ifndef RC_H
#define RC_H

// rc.h
// 7/15/2011

#include "ac/acrc.h"

#define RC_NULL         ACRC_NULL

#define RC_PREFIX_IMAGE ":/images/"
#define RC_IMAGE_NULL   RC_NULL

#define RC_PREFIX_TR    ":/tr/"
#define RC_TR_NULL      RC_NULL

// - Translations -

#define RC_TR_EN        RC_PREFIX_TR "tr_en"
#define RC_TR_JA        RC_PREFIX_TR "tr_ja"
#define RC_TR_ZH        RC_PREFIX_TR "tr_zh"
#define RC_TR_TW        RC_TR_ZH

// - Apps -

#define RC_IMAGE_APP    ACRC_IMAGE_DOWNLOADER

#endif // RC_H
