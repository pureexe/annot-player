// main.cc
// 11/30/2011
// http://www.libssh2.org/examples/x11.html

#define ME "x"
#define APPLET_IP       "184.168.90.152"
#define APPLET_USERNAME "jichi"
#define APPLET_PASSWORD ""
//#define APPLET_IP       "129.115.26.69"

#include "x.h"
extern "C" {
# include <libssh2.h>
# include <sys/ioctl.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/un.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <termios.h>
# include <unistd.h>
} // extern "C"
//#include <QtCore> // for debug use only -- TO BE REMOVED
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>

#define _PATH_UNIX_X "/tmp/.X11-unix/X%d"

// if ssh version < 1.2.8
#if LIBSSH2_VERSION_MAJOR == 1 && \
    LIBSSH2_VERSION_MINOR == 2 && \
    LIBSSH2_VERSION_PATH < 8
typedef int libssh2_socket_t;
inline int
libssh2_session_handshake(LIBSSH2_SESSION *session, libssh2_socket_t socket)
{ return ::libssh2_session_startup(session, socket); }
#endif // LIBSSH2_VERSION

//#define DEBUG "x"
//#include "module/debug/debug.h"
//

namespace { // anonymous

  // Chained list that contains channels and associated X11 socket for each X11 connections
  struct chan_X11_list {
    LIBSSH2_CHANNEL  *chan;
    int         sock;
    chan_X11_list *next;
  };

  termios     _saved_tio;

  chan_X11_list *gp_x11_chan = NULL;

  // Utility function to remove a Node of the chained list
  void remove_node(chan_X11_list *elem)
  {
    chan_X11_list *current_node = NULL;

    current_node = ::gp_x11_chan;

    if (::gp_x11_chan == elem) {
      /* Removing the only one element in the list */
      free(::gp_x11_chan);
      ::gp_x11_chan = NULL;
    }

    if (current_node && current_node->next &&
      current_node->next ==elem) {
      current_node->next = current_node->next->next;
      current_node = current_node->next;
      free(current_node);
    }
  }

  void session_shutdown(LIBSSH2_SESSION *session)
  {
    libssh2_session_disconnect(session,
                  "Session Shutdown, Thank you for playing");
    libssh2_session_free(session);
  }

  int _raw_mode(void)
  {
    int rc;
    termios tio;

    rc = tcgetattr(fileno(stdin), &tio);
    if (rc != -1) {
      _saved_tio = tio;
      cfmakeraw(&tio);
      rc = tcsetattr(fileno(stdin), TCSADRAIN, &tio);
    }
    return rc;
  }

  int _normal_mode(void)
  {
    int rc;
    rc = tcsetattr(fileno(stdin), TCSADRAIN, &_saved_tio);
    return rc;
  }

  // CallBack to initialize the forwarding.
  // Save the channel to loop on it, save the X11 forwarded socket to send
  // and receive info from our X server.

  void x11_callback(LIBSSH2_SESSION *session, LIBSSH2_CHANNEL *channel,
               char *shost, int sport, void **abstract)
  {
    const char * display    = NULL;
    char * ptr      = NULL;
    char * temp_buff  = NULL;
    int   display_port = 0;
    int   sock     = 0;
    int   rc       = 0;
    chan_X11_list *new_;
    chan_X11_list *chan_iter;
    static sockaddr_un addr;

    /*
     * Connect to the display
     * Inspired by x11_connect_display in openssh
     */
    display = ::getenv("DISPLAY");
    if (strcmp("127.0.0.1:0,0", display) == 0)
      display = ":0";
    if (display != NULL) {
      if (strncmp(display, "unix:", 5) == 0 ||
        display[0] == ':') {
        /* Connect to the local unix domain */
        ptr = (char*)strrchr(display, ':');
        temp_buff = (char *) calloc(strlen(ptr+1), sizeof(char));
        if (!temp_buff) {
          ::perror("calloc");
          return;
        }
        memcpy(temp_buff, ptr+1, strlen(ptr+1));
        display_port = atoi(temp_buff);
        free(temp_buff);

        sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock < 0)
          return;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        snprintf(addr.sun_path, sizeof(addr.sun_path),
             _PATH_UNIX_X, display_port);
        rc = connect(sock, (sockaddr *) &addr, sizeof(addr));

        if (rc != -1){
          /* Connection Successfull */
          if (::gp_x11_chan == NULL) {
            /* Calloc ensure that ::gp_x11_chan is full of 0 */
            ::gp_x11_chan = (chan_X11_list *)
              calloc(1, sizeof(chan_X11_list));
            ::gp_x11_chan->sock = sock;
            ::gp_x11_chan->chan = channel;
            ::gp_x11_chan->next = NULL;
          }
          else {
            chan_iter = ::gp_x11_chan;
            while (chan_iter->next != NULL)
              chan_iter = chan_iter->next;
            /* Create the new Node */
            new_ = (chan_X11_list *)
              malloc(sizeof(chan_X11_list));
            new_->sock = sock;
            new_->chan = channel;
            new_->next = NULL;
            chan_iter->next = new_;
          }
        }
        else
          close(sock);
      }
    }
    return;
  }


  // Send and receive Data for the X11 channel.
  // If the connection is closed, returns -1, 0 either.

  // Not reentrant, not thread-safe!
  int x11_send_receive(LIBSSH2_CHANNEL *channel, int sock)
  {
    enum { bufsize    = 8192 };
    static char buf[bufsize];

    static timeval timeval_out; {
      timeval_out.tv_sec = 0;
      timeval_out.tv_usec = 0;
    }

    static fd_set set; {
      FD_ZERO(&set);
      FD_SET(sock, &set);
    }

    enum { nfds = 1 };
    static LIBSSH2_POLLFD  fds[nfds]; {
      fds[0].type = LIBSSH2_POLLFD_CHANNEL;
      fds[0].fd.channel = channel;
      fds[0].events = LIBSSH2_POLLFD_POLLIN;
      fds[0].revents = LIBSSH2_POLLFD_POLLIN;
    }

    int rc = ::libssh2_poll(fds, nfds, 0);
    if (rc > 0) {
      rc = ::libssh2_channel_read(channel, buf,sizeof(buf));
      rc = ::write(sock, buf, rc);
    }

    rc = ::select(sock+1,&set,NULL,NULL,&timeval_out);
    if (rc > 0) {
      ::memset(buf, 0, bufsize);

      // Data in sock
      rc = ::read(sock, buf, sizeof(buf));
      if (rc > 0)
        rc = ::libssh2_channel_write(channel, buf, rc);
      else
        return -1;
    }

    if (::libssh2_channel_eof(channel) == 1)
      return -1;

    return 0;
  }

} // anonymous namespace

// Main, more than inspired by ssh2.c by Bagder
int
main(int argc, char *argv[])
{
  // - Settings -
  const char *username = APPLET_USERNAME;
  const char *password = APPLET_PASSWORD;
  char env_DISPLAY[] = "DISPLAY=:0"; // default DISPLAY if not set
  char env_LC_ALL[] = "LC_ALL=" APPLET_LOCALE;
  char env_LANG[] = "LANG=" APPLET_LOCALE;

  enum { bufsize = 8193 };
  bool o_debug = false;
  bool o_output = true;

  if (!::getenv("DISPLAY"))
    ::putenv(env_DISPLAY);
  ::putenv(env_LC_ALL);
  ::putenv(env_LANG);

  fprintf(stderr, ME ": O_DEBUG = %d\n", (int)o_debug);
  fprintf(stderr, ME ": O_OUTPUT = %d\n", (int)o_output);
  fprintf(stderr, ME ": DISPLAY = %s\n", env_DISPLAY);
  fprintf(stderr, ME ": LOCALE = %s\n", env_LC_ALL);

  // - Init -

  int rc; // temp return code

  rc = ::libssh2_init(0);
  if (rc) {
    perror("libssh2 initialization failed\n");
    return 1;
  }

  sockaddr_in sin = { }; {
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = ::inet_addr(APPLET_IP);
  }

  int sock = ::socket(AF_INET, SOCK_STREAM, 0);

  if (::connect(sock, (sockaddr *)&sin, sizeof(sin))) {
    ::perror("failed to established connection\n");
    return -1;
  }

  // Open SSH session
  LIBSSH2_SESSION *session = ::libssh2_session_init();

  if (::libssh2_session_handshake(session, sock)) {
    ::perror("failed Start the SSH session\n");
    return -1;
  }

  if (o_debug)
    ::libssh2_trace(session, LIBSSH2_TRACE_CONN);

  // Set x11 callback
  ::libssh2_session_callback_set(session, LIBSSH2_CALLBACK_X11, (void *)x11_callback);

  // Authenticate via password
  if (::libssh2_userauth_password(session, username, password)) {
    ::perror("failed to authenticate\n");
    ::session_shutdown(session);
    ::close(sock);
    return -1;
  }

  // Open a channel
  LIBSSH2_CHANNEL *channel = ::libssh2_channel_open_session(session);
  if (!channel) {
    ::perror("failed to open a new channel\n");
    ::session_shutdown(session);
    ::close(sock);
    return -1;
  }

  // Request a PTY
  if (::libssh2_channel_request_pty(channel, "xterm")) {
    ::perror("failed to request a pty\n");
    ::session_shutdown(session);
    ::close(sock);
    return -1;
  }

  // Request X11
  if (::libssh2_channel_x11_req(channel, 0)) {
    ::perror("failed to request X11 forwarding\n");
    ::session_shutdown(session);
    ::close(sock);
    return -1;
  }

  /* Request a shell */
  if (::libssh2_channel_shell(channel)) {
    ::perror("failed to open a shell\n");
    ::session_shutdown(session);
    ::close(sock);
    return -1;
  }

  if (_raw_mode()) {
    ::perror("failed to enter in raw mode\n");
    ::session_shutdown(session);
    ::close(sock);
    return -1;
  }

  // - Service loop -

  // Struct winsize for term size
  winsize w_size = { };
  winsize w_size_bck = { };

  chan_X11_list *current_node = nullptr; // Chan List struct

  char *buf = nullptr;
  //char buf[bufsize];
  do {
    fd_set set; { // For select on stdin
      FD_ZERO(&set);
      FD_SET(::fileno(stdin), &set);
    }

    /* Search if a resize pty has to be send */
    ::ioctl(::fileno(stdin), TIOCGWINSZ, &w_size);
    if (w_size.ws_row != w_size_bck.ws_row ||
        w_size.ws_col != w_size_bck.ws_col) {
      w_size_bck = w_size;

      ::libssh2_channel_request_pty_size(channel, w_size.ws_col, w_size.ws_row);
    }

    buf = (char*)calloc (bufsize, sizeof(char));
    if (!buf)
      break;

    enum { nfds = 1 };
    LIBSSH2_POLLFD fds[nfds]; {
      fds[0].type = LIBSSH2_POLLFD_CHANNEL;
      fds[0].fd.channel = channel;
      fds[0].events = LIBSSH2_POLLFD_POLLIN;
      fds[0].revents = LIBSSH2_POLLFD_POLLIN;
    }
    rc = ::libssh2_poll(fds, nfds, 0);
    if (rc > 0) {
      rc = ::libssh2_channel_read(channel, buf, sizeof(buf));
      if (o_output) {
        ::fprintf(stdout, "%s", buf);
        ::fflush(stdout);
      }
    }

    // Looping on X clients
    current_node = ::gp_x11_chan;
    while (current_node) {
      chan_X11_list *next_node;
      rc = x11_send_receive(current_node->chan, current_node->sock);
      next_node = current_node->next;
      if (rc == -1){
        ::shutdown(current_node->sock,SHUT_RDWR);
        ::close(current_node->sock);
        remove_node(current_node);
      }
      current_node = next_node;
   }


   timeval timeval_out = { };
   if (::select(fileno(stdin)+1,&set,NULL,NULL,&timeval_out) > 0 &&
       ::read(fileno(stdin), buf,1) > 0) // Data in stdin
     ::libssh2_channel_write(channel, buf, sizeof(buf));

    //free(fds);
    ::free(buf);
  } while (::libssh2_channel_eof(channel) != 1);

  // - Clean up -

  if (channel)
    ::libssh2_channel_free(channel);

  _normal_mode();

  ::libssh2_exit();

  return 0;
}
