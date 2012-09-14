// vlccore/video.cc
// See: vlc/control/video.cc
// 11/26/2011

#ifdef _MSC_VER
# pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
# pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include "lib/vlccore/video.h"
#include <cassert>

//#define DEBUG "vlccore::video"
#include "lib/debug/debug.h"

#ifndef MODULE_STRING
# define MODULE_STRING "main"  // needed by VLC
#endif // MODULE_STRING
#ifdef _MSC_VER
# include <inttypes.h>
#endif // _MSC_VER
#include <vlc/vlc.h>
#include <vlc/plugins/vlc_input.h>
#include <vlc/plugins/vlc_vout.h>
#include <vlc/plugins/vlc_vout_display.h>
#include <vlc/lib/media_player_internal.h>
#include <vlc/src/video_output/vout_internal.h>

// - vlc -

// Must be consistent with: vlc/lib/control/media_player.c
// Revision: VLC 2.0.0

namespace { namespace detail {

  // Defined in vlc/src/control/media_player.c
  inline void lock_input(libvlc_media_player_t *mp)
  { ::vlc_mutex_lock(&mp->input.lock); }

  // Defined in vlc/src/control/media_player.c
  inline void unlock_input(libvlc_media_player_t *mp)
  { ::vlc_mutex_unlock(&mp->input.lock); }

} } // anonymous detail

input_thread_t*
libvlc_get_input_thread(libvlc_media_player_t *p_mi)
{
  input_thread_t *p_input_thread;
  Q_ASSERT(p_mi);

  detail::lock_input(p_mi);
  p_input_thread = p_mi->input.p_thread;
  if( p_input_thread )
    vlc_object_hold( p_input_thread );
  else
    libvlc_printerr( "No active input" );
  detail::unlock_input(p_mi);

  return p_input_thread;
}

// - vlccore -

// Return a list of vout threads, save count to \param n.
// See: vlc/lib/video.c
// Revision: VLC 2.0.0
vout_thread_t**
vlccore::GetVouts(libvlc_media_player_t *p_mi, size_t *n)
{
  Q_ASSERT(p_mi);
  input_thread_t *p_input = ::libvlc_get_input_thread(p_mi);
  if(!p_input) {
    *n = 0;
    return 0;
  }

  vout_thread_t **pp_vouts; // return
  if (::input_Control(p_input, INPUT_GET_VOUTS, &pp_vouts, n)) {
    *n = 0;
    pp_vouts = nullptr;
  }
  ::vlc_object_release(p_input);
  return pp_vouts;
}

// Extract (width, height) from vout.
// Revision: VLC 2.0.0
QSize
vlccore::vout_dimension(vout_thread_t *vout)
{
  //return  QSize(vout->render.i_width, vout->render.i_height);
  if (!vout)
    return QSize();
  vout_thread_sys_t *sys = vout->p;
  if (!sys)
    return QSize();
  vout_display_t *vd = sys->display.vd;
  if (!vd)
    return QSize();
  const vout_display_cfg_t *cfg = vd->cfg;
  if (!cfg)
    return QSize();
  return  QSize(cfg->display.width, cfg->display.height);
}

QPoint
vlccore::vout_map_to_widget(vout_thread_t *vout, const QPoint &voutPos, const QSize &widgetSize)
{
  QSize voutSize = vout_dimension(vout);
  if (voutSize.isEmpty() || voutSize == widgetSize)
    return voutPos;

 int x = (voutPos.x() * widgetSize.width()) / voutSize.width(),
     y = (voutPos.y() * widgetSize.height()) / voutSize.height();
 return QPoint(x, y);
}

QPoint
vlccore::vout_map_from_global(vout_thread_t *vout, const QPoint &globalPos, const QRect &voutRect)
{
  if (!vout ||
      !voutRect.width() || !voutRect.height())
    return QPoint();

  // jichi 11/26/2011 FIXME: All coordinates in p_sys are zero.
  // p_sys is not properly initialized.
  //
  //uint i_height = vout->p_sys->i_height,
  //     i_width  = vout->p_sys->i_width;
  //uint i_x, i_y;
  //::vout_PlacePicture(vout, i_width, i_height,
  //                    &i_x, &i_y, &i_width, &i_height);
  //int x = (pos.x() - i_x) * vout->render.i_width / i_width,
  //    y = (pos.y() - i_y) * vout->render.i_height / i_height;

  // render size
  QSize sz = vout_dimension(vout);
  if (sz.isEmpty())
    return QPoint();

  // offset
  int x = globalPos.x() - voutRect.x(),
      y = globalPos.y() - voutRect.y();

  // scale
  x = (x * sz.width()) / voutRect.width();
  y = (y * sz.height()) / voutRect.height();

  return QPoint(x, y);
}

// See: vlc/modules/gui/minimal_macosx/voutagl.m
void
vlccore::vout_mouse_down(vout_thread_t *vout, const QPoint &pos, vlcbutton button)
{
  DOUT("enter");

  //if (!pos.isNull())
  vout_mouse_moved(vout, pos);

  vlc_value_t val = { };
  ::var_Get(vout, "mouse-button-down", &val);
  val.i_int |= (int)button;
  ::var_Set(vout, "mouse-button-down", val);
  DOUT("exit");
}

void
vlccore::vout_mouse_up(vout_thread_t *vout, const QPoint &pos, vlcbutton button)
{
  DOUT("enter");

  //if (!pos.isNull())
  vout_mouse_moved(vout, pos);

  int32_t x, y;
  ::var_GetCoords(vout, "mouse-moved", &x, &y);
  ::var_SetCoords(vout, "mouse-clicked", x, y);

  vlc_value_t val = { };
  ::var_Get(vout, "mouse-button-down", &val);
  val.i_int &= ~(int)button;
  ::var_Set(vout, "mouse-button-down", val);

  DOUT("exit");
}

void
vlccore::vout_mouse_moved(vout_thread_t *vout, const QPoint &pos)
{
  DOUT("enter: pos =" << pos);
  if (vout)
    ::var_SetCoords(vout, "mouse-moved", pos.x(), pos.y());

  DOUT("exit");
}

QPoint
vlccore::vout_mouse_pos(vout_thread_t *vout)
{
  Q_ASSERT(vout);
  qint32 x = 0, y = 0;
  ::var_GetCoords(vout, "mouse-moved", &x, &y);
  return QPoint(x, y);
}


vlccore::vlcbuttons
vlccore::vout_mouse_buttons(vout_thread_t *vout)
{
  vlc_value_t btval = { };
  ::var_Get(vout, "mouse-button-down", &btval);
  return btval.i_int;
}

// See: vlc_fourcc.h
const char *
vlccore::codec_name(uint i_codec)
{
  switch (i_codec) {
    // Video
  case VLC_CODEC_MPGV:          return "mpgv";
  case VLC_CODEC_MP4V:          return "mp4v";
  case VLC_CODEC_DIV1:          return "DIV1";
  case VLC_CODEC_DIV2:          return "DIV2";
  case VLC_CODEC_DIV3:          return "DIV3";
  case VLC_CODEC_SVQ1:          return "SVQ1";
  case VLC_CODEC_SVQ3:          return "SVQ3";
  case VLC_CODEC_H264:          return "h264";
  case VLC_CODEC_H263:          return "h263";
  case VLC_CODEC_H263I:         return "I263";
  case VLC_CODEC_H263P:         return "ILVR";
  case VLC_CODEC_FLV1:          return "FLV1";
  case VLC_CODEC_H261:          return "h261";
  case VLC_CODEC_MJPG:          return "MJPG";
  case VLC_CODEC_MJPGB:         return "mjpb";
  case VLC_CODEC_LJPG:          return "LJPG";
  case VLC_CODEC_WMV1:          return "WMV1";
  case VLC_CODEC_WMV2:          return "WMV2";
  case VLC_CODEC_WMV3:          return "WMV3";
  case VLC_CODEC_WMVA:          return "WMVA";
  case VLC_CODEC_WMVP:          return "WMVP";
  case VLC_CODEC_WMVP2:         return "WVP2";
  case VLC_CODEC_VC1:           return "VC-1";
  case VLC_CODEC_THEORA:        return "theo";
  case VLC_CODEC_TARKIN:        return "tark";
  case VLC_CODEC_SNOW:          return "SNOW";
  case VLC_CODEC_DIRAC:         return "drac";
  case VLC_CODEC_CAVS:          return "CAVS";
  case VLC_CODEC_NUV:           return "NJPG";
  case VLC_CODEC_RV10:          return "RV10";
  case VLC_CODEC_RV13:          return "RV13";
  case VLC_CODEC_RV20:          return "RV20";
  case VLC_CODEC_RV30:          return "RV30";
  case VLC_CODEC_RV40:          return "RV40";
  case VLC_CODEC_VP3:           return "VP3 ";
  case VLC_CODEC_VP5:           return "VP5 ";
  case VLC_CODEC_VP6:           return "VP62";
  case VLC_CODEC_VP6F:          return "VP6F";
  case VLC_CODEC_VP6A:          return "VP6A";
  case VLC_CODEC_MSVIDEO1:      return "MSVC";
  case VLC_CODEC_FLIC:          return "FLIC";
  case VLC_CODEC_SP5X:          return "SP5X";
  case VLC_CODEC_DV:            return "dv  ";
  case VLC_CODEC_MSRLE:         return "mrle";
  case VLC_CODEC_INDEO3:        return "IV31";
  case VLC_CODEC_HUFFYUV:       return "HFYU";
  case VLC_CODEC_FFVHUFF:       return "FFVH";
  case VLC_CODEC_ASV1:          return "ASV1";
  case VLC_CODEC_ASV2:          return "ASV2";
  case VLC_CODEC_FFV1:          return "FFV1";
  case VLC_CODEC_VCR1:          return "VCR1";
  case VLC_CODEC_CLJR:          return "CLJR";
  case VLC_CODEC_RPZA:          return "rpza";
  case VLC_CODEC_SMC:           return "smc ";
  case VLC_CODEC_CINEPAK:       return "CVID";
  case VLC_CODEC_TSCC:          return "TSCC";
  case VLC_CODEC_CSCD:          return "CSCD";
  case VLC_CODEC_ZMBV:          return "ZMBV";
  case VLC_CODEC_VMNC:          return "VMnc";
  case VLC_CODEC_FRAPS:         return "FPS1";
  case VLC_CODEC_TRUEMOTION1:   return "DUCK";
  case VLC_CODEC_TRUEMOTION2:   return "TM20";
  case VLC_CODEC_QTRLE:         return "rle ";
  case VLC_CODEC_QDRAW:         return "qdrw";
  case VLC_CODEC_QPEG:          return "QPEG";
  case VLC_CODEC_ULTI:          return "ULTI";
  case VLC_CODEC_VIXL:          return "VIXL";
  case VLC_CODEC_LOCO:          return "LOCO";
  case VLC_CODEC_WNV1:          return "WNV1";
  case VLC_CODEC_AASC:          return "AASC";
  case VLC_CODEC_INDEO2:        return "IV20";
  case VLC_CODEC_FLASHSV:       return "FSV1";
  case VLC_CODEC_KMVC:          return "KMVC";
  case VLC_CODEC_SMACKVIDEO:    return "SMK2";
  case VLC_CODEC_DNXHD:         return "AVdn";
  case VLC_CODEC_8BPS:          return "8BPS";
  case VLC_CODEC_MIMIC:         return "ML2O";
  case VLC_CODEC_INTERPLAY:     return "imve";
  case VLC_CODEC_IDCIN:         return "IDCI";
  case VLC_CODEC_4XM:           return "4XMV";
  case VLC_CODEC_ROQ:           return "RoQv";
  case VLC_CODEC_MDEC:          return "MDEC";
  case VLC_CODEC_VMDVIDEO:      return "VMDV";
  case VLC_CODEC_CDG:           return "CDG ";
  case VLC_CODEC_FRWU:          return "FRWU";
  case VLC_CODEC_AMV:           return "AMV ";
  case VLC_CODEC_INDEO5:        return "IV50";
  case VLC_CODEC_VP8:           return "VP80";
  case VLC_CODEC_JPEG2000:      return "JP2K";
  case VLC_CODEC_LAGARITH:      return "LAGS";
  case VLC_CODEC_FLASHSV2:      return "FSV2";
  case VLC_CODEC_PRORES:        return "apcn";
  case VLC_CODEC_MXPEG:         return "MXPG";

  case VLC_CODEC_YV9:           return "YVU9";
  case VLC_CODEC_YV12:          return "YV12";
  case VLC_CODEC_I410:          return "I410";
  case VLC_CODEC_I411:          return "I411";
  case VLC_CODEC_I420:          return "I420";
  case VLC_CODEC_I420_9L:       return "I09L";
  case VLC_CODEC_I420_9B:       return "I09B";
  case VLC_CODEC_I420_10L:      return "I0AL";
  case VLC_CODEC_I420_10B:      return "I0AB";
  case VLC_CODEC_I422:          return "I422";
  case VLC_CODEC_I422_9L:       return "I29L";
  case VLC_CODEC_I422_9B:       return "I29B";
  case VLC_CODEC_I422_10L:      return "I2AL";
  case VLC_CODEC_I422_10B:      return "I2AB";
  case VLC_CODEC_I440:          return "I440";
  case VLC_CODEC_I444:          return "I444";
  case VLC_CODEC_I444_9L:       return "I49L";
  case VLC_CODEC_I444_9B:       return "I49B";
  case VLC_CODEC_I444_10L:      return "I4AL";
  case VLC_CODEC_I444_10B:      return "I4AB";
  case VLC_CODEC_J420:          return "J420";
  case VLC_CODEC_J422:          return "J422";
  case VLC_CODEC_J440:          return "J440";
  case VLC_CODEC_J444:          return "J444";
  case VLC_CODEC_YUVP:          return "YUVP";
  case VLC_CODEC_YUVA:          return "YUVA";
  case VLC_CODEC_RGBP:          return "RGBP";
  case VLC_CODEC_RGB8:          return "RGB8";
  case VLC_CODEC_RGB12:         return "RV12";
  case VLC_CODEC_RGBA16:        return "AV16";
  case VLC_CODEC_RGB15:         return "RV15";
  case VLC_CODEC_RGBT:          return "RGBT";
  case VLC_CODEC_RGB16:         return "RV16";
  case VLC_CODEC_RGB24:         return "RV24";
  case VLC_CODEC_RGB32:         return "RV32";
  case VLC_CODEC_RGBA:          return "RGBA";
  case VLC_CODEC_GREY:          return "GREY";
  case VLC_CODEC_UYVY:          return "UYVY";
  case VLC_CODEC_VYUY:          return "VYUY";
  case VLC_CODEC_YUYV:          return "YUY2";
  case VLC_CODEC_YVYU:          return "YVYU";
  case VLC_CODEC_Y211:          return "Y211";
  case VLC_CODEC_CYUV:          return "cyuv";
  case VLC_CODEC_V210:          return "v210";
  case VLC_CODEC_NV12:          return "NV12";
  case VLC_CODEC_NV21:          return "NV21";
  case VLC_CODEC_NV16:          return "NV16";
  case VLC_CODEC_NV61:          return "NV61";

    // Image
  case VLC_CODEC_PNG:           return "png ";
  case VLC_CODEC_PPM:           return "ppm ";
  case VLC_CODEC_PGM:           return "pgm ";
  case VLC_CODEC_PGMYUV:        return "pgmy";
  case VLC_CODEC_PAM:           return "pam ";
  case VLC_CODEC_JPEG:          return "jpeg";
  case VLC_CODEC_JPEGLS:        return "MJLS";
  case VLC_CODEC_BMP:           return "bmp ";
  case VLC_CODEC_TIFF:          return "tiff";
  case VLC_CODEC_GIF:           return "gif ";
  case VLC_CODEC_TARGA:         return "tga ";
  case VLC_CODEC_SGI:           return "sgi ";
  case VLC_CODEC_PNM:           return "pnm ";
  case VLC_CODEC_PCX:           return "pcx ";

    // Audio
  case VLC_CODEC_MPGA:          return "mpga";
  case VLC_CODEC_MP4A:          return "mp4a";
  case VLC_CODEC_ALS:           return "als ";
  case VLC_CODEC_A52:           return "a52 ";
  case VLC_CODEC_EAC3:          return "eac3";
  case VLC_CODEC_DTS:           return "dts ";
  case VLC_CODEC_WMA1:          return "WMA1";
  case VLC_CODEC_WMA2:          return "WMA2";
  case VLC_CODEC_WMAP:          return "WMAP";
  case VLC_CODEC_WMAL:          return "WMAL";
  case VLC_CODEC_WMAS:          return "WMAS";
  case VLC_CODEC_FLAC:          return "flac";
  case VLC_CODEC_MLP:           return "mlp ";
  case VLC_CODEC_TRUEHD:        return "trhd";
  case VLC_CODEC_DVAUDIO:       return "dvau";
  case VLC_CODEC_SPEEX:         return "spx ";
  case VLC_CODEC_VORBIS:        return "vorb";
  case VLC_CODEC_MACE3:         return "MAC3";
  case VLC_CODEC_MACE6:         return "MAC6";
  case VLC_CODEC_MUSEPACK7:     return "MPC ";
  case VLC_CODEC_MUSEPACK8:     return "MPCK";
  case VLC_CODEC_RA_144:        return "14_4";
  case VLC_CODEC_RA_288:        return "28_8";
  case VLC_CODEC_ADPCM_4XM:     return "4xma";
  case VLC_CODEC_ADPCM_EA:      return "ADEA";
  case VLC_CODEC_INTERPLAY_DPCM:return "idpc";
  case VLC_CODEC_ROQ_DPCM:      return "RoQa";
  case VLC_CODEC_DSICINAUDIO:   return "DCIA";
  case VLC_CODEC_ADPCM_XA:      return "xa  ";
  case VLC_CODEC_ADPCM_ADX:     return "adx ";
  case VLC_CODEC_ADPCM_IMA_WS:  return "AIWS";
  case VLC_CODEC_ADPCM_G722:    return "g722";
  case VLC_CODEC_ADPCM_G726:    return "g726";
  case VLC_CODEC_ADPCM_SWF:     return "SWFa";
  case VLC_CODEC_ADPCM_MS:      return "ms0x000x02";
  case VLC_CODEC_ADPCM_IMA_WAV: return "ms0x000x11";
  case VLC_CODEC_VMDAUDIO:      return "vmda";
  case VLC_CODEC_AMR_NB:        return "samr";
  case VLC_CODEC_AMR_WB:        return "sawb";
  case VLC_CODEC_ALAC:          return "alac";
  case VLC_CODEC_QDM2:          return "QDM2";
  case VLC_CODEC_COOK:          return "cook";
  case VLC_CODEC_SIPR:          return "sipr";
  case VLC_CODEC_TTA:           return "TTA1";
  case VLC_CODEC_SHORTEN:       return "shn ";
  case VLC_CODEC_WAVPACK:       return "WVPK";
  case VLC_CODEC_GSM:           return "gsm ";
  case VLC_CODEC_GSM_MS:        return "agsm";
  case VLC_CODEC_ATRAC1:        return "atr1";
  case VLC_CODEC_ATRAC3:        return "atrc";
  case VLC_CODEC_SONIC:         return "SONC";
  case VLC_CODEC_IMC:           return "0x10x40x00x0";
  case VLC_CODEC_TRUESPEECH:    return "0x220x00x00x0";
  case VLC_CODEC_NELLYMOSER:    return "NELL";
  case VLC_CODEC_APE:           return "APE ";
  case VLC_CODEC_QCELP:         return "Qclp";
  case VLC_CODEC_302M:          return "302m";
  case VLC_CODEC_DVD_LPCM:      return "lpcm";
  case VLC_CODEC_DVDA_LPCM:     return "apcm";
  case VLC_CODEC_BD_LPCM:       return "bpcm";
  case VLC_CODEC_SDDS:          return "sdds";
  case VLC_CODEC_MIDI:          return "MIDI";
  case VLC_CODEC_S8:            return "s8  ";
  case VLC_CODEC_U8:            return "u8  ";
  case VLC_CODEC_S16L:          return "s16l";
  case VLC_CODEC_S16B:          return "s16b";
  case VLC_CODEC_U16L:          return "u16l";
  case VLC_CODEC_U16B:          return "u16b";
  case VLC_CODEC_S20B:          return "s20b";
  case VLC_CODEC_S24L:          return "s24l";
  case VLC_CODEC_S24B:          return "s24b";
  case VLC_CODEC_U24L:          return "u24l";
  case VLC_CODEC_U24B:          return "u24b";
  case VLC_CODEC_S32L:          return "s32l";
  case VLC_CODEC_S32B:          return "s32b";
  case VLC_CODEC_U32L:          return "u32l";
  case VLC_CODEC_U32B:          return "u32b";
  case VLC_CODEC_F32L:          return "f32l";
  case VLC_CODEC_F32B:          return "f32b";
  case VLC_CODEC_F64L:          return "f64l";
  case VLC_CODEC_F64B:          return "f64b";

  case VLC_CODEC_ALAW:          return "alaw";
  case VLC_CODEC_MULAW:         return "mlaw";
  case VLC_CODEC_DAT12:         return "LP12";
  case VLC_CODEC_S24DAUD:       return "daud";
  case VLC_CODEC_FI32:          return "fi32";
  case VLC_CODEC_TWINVQ:        return "TWIN";
  case VLC_CODEC_ADPCM_IMA_AMV: return "imav";

    // Subtitle
  case VLC_CODEC_SPU:           return "spu ";
  case VLC_CODEC_DVBS:          return "dvbs";
  case VLC_CODEC_SUBT:          return "subt";
  case VLC_CODEC_XSUB:          return "XSUB";
  case VLC_CODEC_SSA:           return "ssa ";
  case VLC_CODEC_TEXT:          return "TEXT";
  case VLC_CODEC_TELETEXT:      return "telx";
  case VLC_CODEC_KATE:          return "kate";
  case VLC_CODEC_CMML:          return "cmml";
  case VLC_CODEC_ITU_T140:      return "t140";
  case VLC_CODEC_USF:           return "usf";
  case VLC_CODEC_OGT:           return "ogt";
  case VLC_CODEC_CVD:           return "cvd";

    // Blu-ray Presentation Graphics
  case VLC_CODEC_BD_PG:         return "bdpg";

    // EBU STL "TECH. 3264-E"
  case VLC_CODEC_EBU_STL:       return "STL";

  default: return 0;
  }
}


// EOF
