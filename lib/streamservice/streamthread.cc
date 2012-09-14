// streamthread.cc
// 2/11/2012
// See: live555/testOnDemandRTSPServer.cpp
// See: live555/mpeg2TransportStreamFromUDPSourceTest.cpp

#include "streamthread.h"
#include "adtsaudiostreamsmss.h"
#include "h264videostreamsmss.h"
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <QtCore>
#include <string>

#define DEBUG "streamthread"
#include "lib/debug/debug.h"

// - Live555 -

enum { OutPacketBufferMaxSize = 2 * 1024 * 1024 }; // defval = 60000

void
StreamThread::initLive555()
{
  if (OutPacketBuffer::maxSize < OutPacketBufferMaxSize)
    OutPacketBuffer::maxSize = OutPacketBufferMaxSize;
}

// - Run -

void
StreamThread::stop(ulong timeout)
{
  DOUT("enter");
  stop_ = 1;
  wait(timeout);
  DOUT("exit");
}

void
StreamThread::shufflePort()
{
  enum { start = 6002, stop = 6880 };
  port_ = start + qrand() % (stop - start);
}

void
StreamThread::run()
{
  DOUT("enter");
  Q_ASSERT(in_);
  // Begin by setting up our usage environment:
  TaskScheduler *scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment *env = BasicUsageEnvironment::createNew(*scheduler);

  if (!port_)
    shufflePort();

  // Create the RTSP server:
  UserAuthenticationDatabase* authDB = 0;
  RTSPServer *rtspServer;
  int tries = 8;
  while (!(rtspServer = RTSPServer::createNew(*env, port_, authDB)) && tries-- > 0) {
    DOUT("warning: already bind, reseting port number");
    shufflePort();
  }
  if (!rtspServer) {
    url_.clear();
    emit errorMessage(tr("failed to create RTSP service") + ": " + env->getResultMsg());
    DOUT("exit: failed to create RTSP server: errno =" << env->getErrno() << "," << env->getResultMsg());
    return;
  }

  // Set up each of the possible streams that can be served by the
  // RTSP server.  Each such stream is implemented using a
  // "ServerMediaSession" object, plus one or more
  // "ServerMediaSubsession" objects for each audio/video substream.
  char const *desc = "Annot Player Streaming Service";
  char const *streamName = ""; // stream at root

  float duration = duration_ / 1000;

  bool hasSession = false;
  ServerMediaSession *sms
    = ServerMediaSession::createNew(*env, streamName, streamName, desc);
  foreach (const MediaStreamInfo &inf, ins_) {
    ServerMediaSubsession *smss;
    switch (inf.mediaType) {
    case MT_H264: smss = H264VideoStreamSMSS::createNew(*env, inf.inputStream, inf.mediaToc, duration); break;
    case MT_AAC: smss = ADTSAudioStreamSMSS::createNew(*env, inf.inputStream, inf.mediaToc, duration); break;
    default: Q_ASSERT(0); smss = 0;
    }
    if (smss) {
      hasSession = true;
      sms->addSubsession(smss);
    }
  }
  if (hasSession) {
    rtspServer->addServerMediaSession(sms);

    char *url = rtspServer->rtspURL(sms);
    url_ = QString::fromLocal8Bit(url);
    delete[] url;

    stop_ = 0;
  } else {
    url_.clear();
    stop_ = 1;
  }

  DOUT("stream ready");
  QTimer::singleShot(0,
                     this, SLOT(onStreamReady()));
  env->taskScheduler().doEventLoop(&stop_); // does not return
  DOUT("exit");
}

// EOF
/*

  // To make the second and subsequent client for each stream reuse the same
  // input stream as the first client (rather than playing the file from the
  // start for each client), change the following "False" to "True":
  //Boolean reuseFirstSource = False;

  // To stream *only* MPEG-1 or 2 video "I" frames
  // (e.g., to reduce network bandwidth),
  // change the following "False" to "True":
  Boolean iFramesOnly = False;

  char newMatroskaDemuxWatchVariable;
  MatroskaFileServerDemux *demux;

  void
  onMatroskaDemuxCreation(MatroskaFileServerDemux *newDemux, void *clientData)
  {
    Q_UNUSED(clientData)
    demux = newDemux;
    newMatroskaDemuxWatchVariable = 1;
  }
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
  } else {
    *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
  }

  // A MPEG-4 video elementary stream:
  {
    char const* streamName = "mpeg4ESVideoTest";
    char const* inputFileName = "test.m4e";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    sms->addSubsession(MPEG4VideoFileServerMediaSubsession
               ::createNew(*env, inputFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A MP3 audio stream (actually, any MPEG-1 or 2 audio file will work):
  // To stream using 'ADUs' rather than raw MP3 frames, uncomment the following:
//#define STREAM_USING_ADUS 1
  // To also reorder ADUs before streaming, uncomment the following:
//#define INTERLEAVE_ADUS 1
  // (For more information about ADUs and interleaving,
  //  see <http://www.live555.com/rtp-mp3/>)
  {
    char const* streamName = "mp3AudioTest";
    char const* inputFileName = "test.mp3";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    Boolean useADUs = False;
    Interleaving* interleaving = NULL;
#ifdef STREAM_USING_ADUS
    useADUs = True;
#ifdef INTERLEAVE_ADUS
    unsigned char interleaveCycle[] = {0,2,1,3}; // or choose your own...
    unsigned const interleaveCycleSize
      = (sizeof interleaveCycle)/(sizeof (unsigned char));
    interleaving = new Interleaving(interleaveCycleSize, interleaveCycle);
#endif
#endif
    sms->addSubsession(MP3AudioFileServerMediaSubsession
               ::createNew(*env, inputFileName, reuseFirstSource,
                   useADUs, interleaving));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A H.264 video elementary stream:
  {
    char const* streamName = "h264ESVideoTest";
    char const* inputFileName = "test.264";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    sms->addSubsession(H264VideoFileServerMediaSubsession
               ::createNew(*env, inputFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A MPEG-1 or 2 audio+video program stream:
  {
    char const* streamName = "mpeg1or2AudioVideoTest";
    char const* inputFileName = "test.mpg";
    // NOTE: This *must* be a Program Stream; not an Elementary Stream
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    MPEG1or2FileServerDemux* demux
      = MPEG1or2FileServerDemux::createNew(*env, inputFileName, reuseFirstSource);
    sms->addSubsession(demux->newVideoServerMediaSubsession(iFramesOnly));
    sms->addSubsession(demux->newAudioServerMediaSubsession());
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A MPEG-1 or 2 video elementary stream:
  {
    char const* streamName = "mpeg1or2ESVideoTest";
    char const* inputFileName = "testv.mpg";
    // NOTE: This *must* be a Video Elementary Stream; not a Program Stream
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    sms->addSubsession(MPEG1or2VideoFileServerMediaSubsession
           ::createNew(*env, inputFileName, reuseFirstSource, iFramesOnly));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A MP3 audio stream (actually, any MPEG-1 or 2 audio file will work):
  // To stream using 'ADUs' rather than raw MP3 frames, uncomment the following:
//#define STREAM_USING_ADUS 1
  // To also reorder ADUs before streaming, uncomment the following:
//#define INTERLEAVE_ADUS 1
  // (For more information about ADUs and interleaving,
  //  see <http://www.live555.com/rtp-mp3/>)
  {
    char const* streamName = "mp3AudioTest";
    char const* inputFileName = "test.mp3";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    Boolean useADUs = False;
    Interleaving* interleaving = NULL;
#ifdef STREAM_USING_ADUS
    useADUs = True;
#ifdef INTERLEAVE_ADUS
    unsigned char interleaveCycle[] = {0,2,1,3}; // or choose your own...
    unsigned const interleaveCycleSize
      = (sizeof interleaveCycle)/(sizeof (unsigned char));
    interleaving = new Interleaving(interleaveCycleSize, interleaveCycle);
#endif
#endif
    sms->addSubsession(MP3AudioFileServerMediaSubsession
               ::createNew(*env, inputFileName, reuseFirstSource,
                   useADUs, interleaving));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A WAV audio stream:
  {
    char const* streamName = "wavAudioTest";
    char const* inputFileName = "test.wav";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    // To convert 16-bit PCM data to 8-bit u-law, prior to streaming,
    // change the following to True:
    Boolean convertToULaw = False;
    sms->addSubsession(WAVAudioFileServerMediaSubsession
           ::createNew(*env, inputFileName, reuseFirstSource, convertToULaw));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // An AMR audio stream:
  {
    char const* streamName = "amrAudioTest";
    char const* inputFileName = "test.amr";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    sms->addSubsession(AMRAudioFileServerMediaSubsession
               ::createNew(*env, inputFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A 'VOB' file (e.g., from an unencrypted DVD):
  {
    char const* streamName = "vobTest";
    char const* inputFileName = "test.vob";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    // Note: VOB files are MPEG-2 Program Stream files, but using AC-3 audio
    MPEG1or2FileServerDemux* demux
      = MPEG1or2FileServerDemux::createNew(*env, inputFileName, reuseFirstSource);
    sms->addSubsession(demux->newVideoServerMediaSubsession(iFramesOnly));
    sms->addSubsession(demux->newAC3AudioServerMediaSubsession());
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A MPEG-2 Transport Stream:
  {
    char const* streamName = "mpeg2TransportStreamTest";
    char const* inputFileName = "test.ts";
    char const* indexFileName = "test.tsx";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    sms->addSubsession(MPEG2TransportFileServerMediaSubsession
               ::createNew(*env, inputFileName, indexFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // An AAC audio stream (ADTS-format file):
  {
    char const* streamName = "aacAudioTest";
    char const* inputFileName = "test.aac";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    sms->addSubsession(ADTSAudioFileServerMediaSubsession
               ::createNew(*env, inputFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A DV video stream:
  {
    // First, make sure that the RTPSinks' buffers will be large enough to handle the huge size of DV frames (as big as 288000).
    OutPacketBuffer::maxSize = 300000;

    char const* streamName = "dvVideoTest";
    char const* inputFileName = "test.dv";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    sms->addSubsession(DVVideoFileServerMediaSubsession
               ::createNew(*env, inputFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A AC3 video elementary stream:
  {
    char const* streamName = "ac3AudioTest";
    char const* inputFileName = "test.ac3";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);

    sms->addSubsession(AC3AudioFileServerMediaSubsession
               ::createNew(*env, inputFileName, reuseFirstSource));

    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A Matroska ('.mkv') file, with video+audio+subtitle streams:
  {
    char const* streamName = "matroskaFileTest";
    char const* inputFileName = "test.mkv";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);

    newMatroskaDemuxWatchVariable = 0;
    MatroskaFileServerDemux::createNew(*env, inputFileName, onMatroskaDemuxCreation, NULL);
    env->taskScheduler().doEventLoop(&newMatroskaDemuxWatchVariable);

    Boolean sessionHasTracks = False;
    ServerMediaSubsession* smss;
    while ((smss = demux->newServerMediaSubsession()) != NULL) {
      sms->addSubsession(smss);
      sessionHasTracks = True;
    }
    if (sessionHasTracks) {
      rtspServer->addServerMediaSession(sms);
    }
    // otherwise, because the stream has no tracks, we don't add a ServerMediaSession to the server.

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A WebM ('.webm') file, with video(VP8)+audio(Vorbis) streams:
  // (Note: ".webm' files are special types of Matroska files, so we use the same code as the Matroska ('.mkv') file code above.)
  {
    char const* streamName = "webmFileTest";
    char const* inputFileName = "test.webm";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);

    newMatroskaDemuxWatchVariable = 0;
    MatroskaFileServerDemux::createNew(*env, inputFileName, onMatroskaDemuxCreation, NULL);
    env->taskScheduler().doEventLoop(&newMatroskaDemuxWatchVariable);

    Boolean sessionHasTracks = False;
    ServerMediaSubsession* smss;
    while ((smss = demux->newServerMediaSubsession()) != NULL) {
      sms->addSubsession(smss);
      sessionHasTracks = True;
    }
    if (sessionHasTracks) {
      rtspServer->addServerMediaSession(sms);
    }
    // otherwise, because the stream has no tracks, we don't add a ServerMediaSession to the server.

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A MPEG-2 Transport Stream, coming from a live UDP (raw-UDP or RTP/UDP) source:
  {
    char const* streamName = "mpeg2TransportStreamFromUDPSourceTest";
    char const* inputAddressStr = "239.255.42.42";
        // This causes the server to take its input from the stream sent by the "testMPEG2TransportStreamer" demo application.
        // (Note: If the input UDP source is unicast rather than multicast, then change this to NULL.)
    portNumBits const inputPortNum = 1234;
        // This causes the server to take its input from the stream sent by the "testMPEG2TransportStreamer" demo application.
    Boolean const inputStreamIsRawUDP = False;
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
                      descriptionString);
    sms->addSubsession(MPEG2TransportUDPServerMediaSubsession
               ::createNew(*env, inputAddressStr, inputPortNum, inputStreamIsRawUDP));
    rtspServer->addServerMediaSession(sms);

    char* url = rtspServer->rtspURL(sms);
    *env << "\n\"" << streamName << "\" stream, from a UDP Transport Stream input source \n\t(";
    if (inputAddressStr != NULL) {
      *env << "IP multicast address " << inputAddressStr << ",";
    } else {
      *env << "unicast;";
    }
    *env << " port " << inputPortNum << ")\n";
    *env << "Play this stream using the URL \"" << url << "\"\n";
    delete[] url;
  }
*/
