/** @file

  A brief file description

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <poll.h>
#include <netinet/tcp.h>
#include <sys/resource.h>
#include <math.h>
#include <limits.h>
#include <sys/mman.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

typedef int64_t ink_hrtime;

#define bool int
#define false 0
#define true 1
#define SIZE(x) (sizeof(x)/sizeof((x)[0]))

/*
 FTP - Traffic Server Template
   220 i5 FTP server (Version wu-2.4(3) Mon Jul 8 14:39:48 PDT 1996) ready.
   USER anonymous
   331 Guest login ok, send your complete e-mail address as password.
   PASS traffic_server@inktomi.com
   230 Guest login ok, access restrictions apply.
   CWD .
   250 CWD command successful.
   TYPE I
   200 Type set to I.
   PASV
   227 Entering Passive Mode (128,174,5,14,16,238)
   RETR foo
   LIST
   150 Opening ASCII mode data connection for /bin/ls.
*/

#define MAX_URL_LEN 1024

#if defined(freebsd)
extern "C" int gethostname(char *name, int namelen);
#endif

//
// Compilation Options
//

#define SERVER_BUFSIZE             4096
#define CLIENT_BUFSIZE             2048
#define MAX_BUFSIZE                (65536 + 4096)


//#define RETRY_CLIENT_WRITE_ERRORS
#define VERSION_NUM "$Revision: 1.2 $"
#if defined(BUILD_PERSON) && defined(BUILD_MACHINE)
#define VERSION ("JTest Version %s - " __DATE__ " " __TIME__ \
                 " (" BUILD_PERSON "@" BUILD_MACHINE ")\n" )
#else
#define VERSION ("JTest Version %s - " __DATE__ " " __TIME__ "\n")
#endif
// #define PRINT_LOCAL_PORT

//
// Contants
//
#define MAXFDS                65536
#define HEADER_DONE           -1
#define POLL_GROUP_SIZE       800
#define MAX_RESPONSE_LENGTH   1000000
#define HEADER_SIZE           10000
#define POLL_TIMEOUT          10
#define STATE_FTP_DATA_READY  0xFAD
#define MAX_DEFERED_URLS      10000
#define DEFERED_URLS_BLOCK    2000

enum FTP_MODE {
  FTP_NULL,
  FTP_PORT,
  FTP_PASV
};

typedef int (*accept_fn_t)(int);
typedef int (*poll_cb)(int);
struct ArgumentDescription;
typedef void ArgumentFunction(
  ArgumentDescription * argument_descriptions, int n_argument_descriptions,
  char * arg);

static int read_request(int sock);
static int write_request(int sock);
static int make_client (unsigned int addr, int port);
static void make_bfc_client (unsigned int addr, int port);
static int make_url_client(char * url,char * base_url = 0, bool seen = false,
                           bool unthrottled = false);
static int write_ftp_response(int sock);
static void interval_report();
static void undefer_url(bool unthrottled = false);
static void done();
static int is_done();
static int open_server(unsigned short int port, accept_fn_t accept_fn);
static int accept_ftp_data (int sock);

char ** defered_urls = NULL; 
int n_defered_urls = 0;
int server_fd = 0;
int server_port = 0;
int proxy_port = 8080;
unsigned int proxy_addr = 0;
unsigned int local_addr = 0;
char proxy_host[81] = "localhost";
char local_host[81];
int verbose = 0;
int verbose_errors = 1;
int debug = 0;
int nclients = 100;
int current_clients = 0;
int client_speed = 0;
int check_content = 0;
int nocheck_length = 0;
int obey_redirects = 1;
int only_clients = 0;
int only_server = 0;
int drop_after_CL = 0;
int server_speed = 0;
int server_delay = 0;
int interval = 1;
int sbuffersize = SERVER_BUFSIZE;
int cbuffersize = CLIENT_BUFSIZE;
int test_time = 0;
int last_fd = -1;
char * response_buffer = NULL;
int errors = 0;
int clients = 0, running_clients = 0, new_clients = 0, total_clients = 0;
int servers = 0, running_servers = 0, new_servers = 0, total_servers = 0;
float running_ops = 0;
int ops = 0, new_ops = 0;
float total_ops = 0;
int running_sops = 0, new_sops = 0, total_sops = 0;
int running_latency = 0, latency = 0;
int lat_ops = 0, b1_ops = 0, running_b1latency = 0, b1latency = 0;
uint64_t running_cbytes = 0, new_cbytes = 0, total_cbytes = 0;
uint64_t running_tbytes = 0, new_tbytes = 0, total_tbytes = 0;
int average_over = 5;
double hitrate = 0.4;
int hotset = 1000;
int keepalive = 4;
int keepalive_cons = 4;
int follow_arg = 0;
int follow = 0;
int follow_same_arg = 0;
int follow_same = 0;
char current_host[512];
int fullpage = 0;
int show_before = 0;
int show_headers = 0;
int server_keepalive = 4;
int version = 0;
int urls_mode = 0;
int pipeline = 1;
int hostrequest = 0;
int ftp = 0;
double ftp_mdtm_err_rate = 0.0;
int ftp_mdtm_rate = 0;
time_t ftp_mdtm_last_update = 0;
char ftp_mdtm_str[64];
int embed_url = 1;
double ims_rate = 0.5;
double client_abort_rate = 0.0;
double server_abort_rate = 0.0;
int compd_port = 0;
int compd_suite = 0;
int ka_cache_head[500];
int ka_cache_tail[500];
int n_ka_cache = 0;
char urls_file[256] = "";
FILE * urls_fp = NULL;
char urlsdump_file[256] = "";
FILE * urlsdump_fp = NULL;
int drand_seed = 0;
int docsize = -1;
int url_hash_entries = 1000000;
char url_hash_filename[256] = "";
int bandwidth_test = 0;
int bandwidth_test_to_go = 0;
uint64_t total_client_request_bytes = 0;
uint64_t total_proxy_request_bytes = 0;
uint64_t total_server_response_body_bytes = 0;
uint64_t total_server_response_header_bytes = 0;
uint64_t total_proxy_response_body_bytes = 0;
uint64_t total_proxy_response_header_bytes = 0;
ink_hrtime now = 0, start_time = 0;
ArgumentFunction jtest_usage;
int extra_headers = 0;
int alternates = 0;
int abort_retry_speed = 0;
int abort_retry_bytes = 0;
int abort_retry_secs = 5;
int client_rate = 0;
double reload_rate = 0;
int vary_user_agent = 0;
int server_content_type = 0;
int request_extension = 0;
int no_cache = 0;
double evo_rate = 0.0;
double zipf = 0.0;
int zipf_bucket_size = 1;

struct ArgumentDescription {
  char * name;
  char   key;
  char * description;
  char * type;
  void * location;
  char * env;
  ArgumentFunction * pfn;       
};

ArgumentDescription argument_descriptions[] = {
  {"proxy_port",'p',"Proxy Port","I",&proxy_port,"JTEST_PROXY_PORT",NULL},
  {"proxy_host",'P',"Proxy Host","S80",&proxy_host,"JTEST_PROXY_HOST",NULL},
  {"server_port",'s',"Server Port (0:auto select)","I",&server_port,
   "JTEST_SERVER_PORT",NULL},
  {"server_host",'S',"Server Host (null:localhost)","S80",
   &local_host,"JTEST_SERVER_HOST",NULL},
  {"server_speed",'r',"Server Bytes Per Second (0:unlimit)","I",
   &server_speed,"JTEST_SERVER_SPEED",NULL},
  {"server_delay", 'w', "Server Initial Delay (msec)", "I", &server_delay,
   "JTEST_SERVER_INITIAL_DELAY", NULL},
  {"clients",'c',"Clients","I",&nclients,"JTEST_CLIENTS",NULL},
  {"client_speed",'R',"Client Bytes Per Second (0:unlimit)","I",
   &client_speed,"JTEST_CLIENT_SPEED",NULL},
  {"sbuffersize",'b',"Server Buffer Size","I",&sbuffersize,
   "JTEST_SERVER_BUFSIZE", NULL},
  {"cbuffersize",'B',"Client Buffer Size","I",&cbuffersize,
   "JTEST_CLIENT_BUFSIZE", NULL},
  {"average_over",'a',"Seconds to Average Over","I",&average_over,
   "JTEST_AVERAGE_OVER", NULL},
  {"hitrate",'z',"Hit Rate","D",&hitrate,"JTEST_HITRATE",NULL},
  {"hotset",'Z',"Hotset Size","I",&hotset,"JTEST_HOTSET",NULL},
  {"interval",'i',"Reporting Interval (seconds)","I",&interval,
   "JTEST_INTERVAL",NULL},
  {"keepalive",'k',"Keep-Alive Length","I",&keepalive,
   "JTEST_KEEPALIVE",NULL},
  {"keepalive_cons",'K',"# Keep-Alive Connections (0:unlimit)","I",
   &keepalive_cons, "JTEST_KEEPALIVE_CONNECTIONS",NULL },
  {"docsize", 'L', "Document Size (-1:varied)","I", &docsize, 
   "JTEST_DOCSIZE",NULL},
  {"skeepalive",'j',"Server Keep-Alive (0:unlimit)","I",&server_keepalive,
   "JTEST_SERVER_KEEPALIVE",NULL},
  {"show_urls", 'x', "Show URLs before they are accessed","F", &show_before, 
   "JTEST_SHOW_URLS",NULL},
  {"show_headers", 'X', "Show Headers","F", &show_headers,      
   "JTEST_SHOW_HEADERS",NULL},
  {"ftp",'f',"FTP Requests","F",&ftp, "JTEST_FTP",NULL},
  {"ftp_mdtm_err_rate", ' ', "FTP MDTM 550 Error Rate", "D",
   &ftp_mdtm_err_rate, "JTEST_FTP_MDTM_ERR_RATE", NULL},
  {"ftp_mdtm_rate", ' ', "FTP MDTM Update Rate (sec, 0:never)", "I",
   &ftp_mdtm_rate, "JTEST_FTP_MDTM_RATE", NULL},
  {"fullpage",'l',"Full Page (Images)","F",&fullpage, 
   "JTEST_FULLPAGE",NULL},
  {"follow",'F',"Follow Links","F",&follow_arg, "JTEST_FOLLOW",NULL},
  {"same_host",'J',"Only follow URLs on same host","F",&follow_same_arg, 
   "JTEST_FOLLOW_SAME",NULL},
  {"test_time",'t',"run for N seconds (0:unlimited)","I", &test_time, 
   "TEST_TIME",NULL},
  {"urls",'u',"URLs from File","S256", urls_file, "JTEST_URLS",NULL},
  {"urlsdump", 'U', "URLs to File", "S256", urlsdump_file, 
   "JTEST_URLS_DUMP", NULL},
  {"hostrequest",'H',"Host Request(1=yes,2=transparent)","I", &hostrequest, "JTEST_HOST_REQUEST",
   NULL},
  {"check_content",'C',"Check returned content", "F", 
   &check_content, "JTEST_CHECK_CONTENT", NULL},
  {"nocheck_length",' ',"Don't check returned length", "F", 
   &nocheck_length, "JTEST_NOCHECK_LENGTH", NULL},
  {"obey_redirects",'m',"Obey Redirects", "f", &obey_redirects, 
   "JTEST_OBEY_REDIRECTS", NULL},
  {"embed URL",'M',"Embed URL in synth docs", "f", 
   &embed_url, "JTEST_EMBED_URL", NULL},
  {"url_hash_entries",'q',"URL Hash Table Size (-1:use file size)", 
   "I", &url_hash_entries, "JTEST_URL_HASH_ENTRIES", NULL},
  {"url_hash_filename",'Q',"URL Hash Table Filename", "S256", 
   url_hash_filename, "JTEST_URL_HASH_FILENAME", NULL},
  {"only_clients",'y',"Only Clients","F",&only_clients,
   "JTEST_ONLY_CLIENTS",NULL},
  {"only_server",'Y',"Only Server","F",&only_server,"JTEST_ONLY_SERVER",NULL},
  {"bandwidth_test",'A',"Bandwidth Test","I",&bandwidth_test,
   "JTEST_BANDWIDTH_TEST",NULL},
  {"drop_after_CL",'T',"Drop after Content-Length","F", 
   &drop_after_CL, "JTEST_DROP",NULL},
  {"version",'V',"Version","F",&version,"JTEST_VERSION",NULL},
  {"verbose",'v',"Verbose Flag","F",&verbose,"JTEST_VERBOSE",NULL},
  {"verbose_errors",'E',"Verbose Errors Flag","f",&verbose_errors,
   "JTEST_VERBOSE_ERRORS",NULL},
  {"drand",'D',"Random Number Seed","I",&drand_seed,"JTEST_DRAND",NULL},
  {"ims_rate",'I',"IMS Not-Changed Rate","D",&ims_rate, "JTEST_IMS_RATE",NULL},
  {"client_abort_rate",'g',"Client Abort Rate","D",&client_abort_rate, 
   "JTEST_CLIENT_ABORT_RATE",NULL},
  {"server_abort_rate",'G',"Server Abort Rate","D",&server_abort_rate, 
   "JTEST_SERVER_ABORT_RATE",NULL},
  {"extra_headers",'n',"Number of Extra Headers","I",&extra_headers,
   "JTEST_EXTRA_HEADERS",NULL},
  {"alternates",'N',"Number of Alternates","I",&alternates,
   "JTEST_ALTERNATES",NULL},
  {"client_rate", 'e', "Clients Per Sec", "I", &client_rate,
   "JTEST_CLIENT_RATE",NULL},
  {"abort_retry_speed", 'o', "Abort/Retry Speed", "I", 
   &abort_retry_speed, "JTEST_ABORT_RETRY_SPEED",NULL},
  {"abort_retry_bytes", ' ', "Abort/Retry Threshhold (bytes)", "I", 
   &abort_retry_bytes, "JTEST_ABORT_RETRY_THRESHHOLD_BYTES",NULL},
  {"abort_retry_secs", ' ', "Abort/Retry Threshhold (secs)", "I", 
   &abort_retry_secs, "JTEST_ABORT_RETRY_THRESHHOLD_SECS",NULL},
  {"reload_rate", 'W', "Reload Rate", "D",
   &reload_rate, "JTEST_RELOAD_RATE",NULL},
  {"compd_port",'O',"Compd port","I",&compd_port, "JTEST_COMPD_PORT",NULL},
  {"compd_suite",'1',"Compd Suite","F",&compd_suite, "JTEST_COMPD_SUITE",NULL},
  {"vary_user_agent",'2',"Vary on User-Agent (use w/ alternates)","I",
   &vary_user_agent,"JTEST_VARY_ON_USER_AGENT",NULL},
  {"content_type",'3',"Server Content-Type (1 html, 2 jpeg)","I",
   &server_content_type,"JTEST_CONTENT_TYPE",NULL},
  {"request_extension",'4',"Request Extn (1\".html\" 2\".jpeg\" 3\"/\")","I",
   &request_extension,"JTEST_REQUEST_EXTENSION",NULL},
  {"no_cache",'5',"Send Server no-cache","I",&no_cache,"JTEST_NO_CACHE",NULL},
  {"zipf_bucket",'7',"Bucket size (of 1M buckets) for Zipf","I",
   &zipf_bucket_size,"JTEST_ZIPF_BUCKET_SIZE",NULL},
  {"zipf",'8',"Use a Zipf distribution with this alpha (say 1.2)","D",
   &zipf,"JTEST_ZIPF",NULL},
  {"evo_rate",'9',"Evolving Hotset Rate (evolutions/hour)","D",
   &evo_rate,"JTEST_EVOLVING_HOTSET_RATE",NULL},
  {"debug",'d',"Debug Flag","F",&debug,"JTEST_DEBUG",NULL},
  {"help",'h',"Help",NULL,NULL,NULL,jtest_usage}
};
int n_argument_descriptions = SIZE(argument_descriptions);

struct FD {
  int fd;
  poll_cb read_cb;
  poll_cb write_cb;
  ink_hrtime start;
  ink_hrtime active;
  ink_hrtime ready;

  double doc;
  int doc_length;
  struct sockaddr_in name;

  int state;             // request parsing state
  int req_pos;           // request read position
  char * base_url;
  char * req_header;
  char * response;
  char * response_header;
  int length;
  int response_length;
  int response_remaining;
  int keepalive;
  int next;
  int nalternate;
  unsigned int ip;
  unsigned int binary:1;
  unsigned int ims:1;
  unsigned int drop_after_CL:1;
  unsigned int client_abort:1;
  unsigned int jg_compressed:1;
  int * count;
  int bytes;
  int ftp_data_fd;
  FTP_MODE ftp_mode;
  unsigned int ftp_peer_addr;
  unsigned short ftp_peer_port;

  void reset() {
    next = 0;
    fd = -1;
    read_cb = NULL;
    write_cb = NULL;
    state = 0;
    start = 0;
    active = 0;
    ready = 0;
    req_pos = 0;
    length = 0;
    if (!urls_mode)
      response = NULL;
    response_header[0] = 0;
    response_length = 0;
    response_remaining = 0;
    count = NULL;
    bytes = 0;
    doc = 0.0;
    doc_length = 0;
    ims = 0;
    drop_after_CL = ::drop_after_CL;
    client_abort = 0;
    jg_compressed = 0;
    ftp_mode = FTP_NULL;
    ftp_peer_addr = 0;
    ftp_peer_port = 0;
  }
  
  void close();
  FD() { 
    req_header = 0; base_url = 0; keepalive = 0; 
    response_header = 0; ftp_data_fd = 0; 
    reset(); 
  }
};

FD *fd = NULL;

void FD::close() {
  if (verbose) 
    printf("close: %d\n", fd);
  ::close(fd);
  if (is_done())
    done();
  keepalive = 0;
  ip = 0;
  if (count) (*count)--;
  if (count == &clients)
    current_clients--;
  reset();
  if (urls_mode) undefer_url();
  ftp_data_fd = 0;
}

// Library functions from libts

const char * SPACES = "                                                                               ";
int ink_code_md5(unsigned char *input, int input_length,
                 unsigned char *sixteen_byte_hash_pointer);

class           ParseRules
{
        public:
        ParseRules();
        enum {
                CHAR_SP = 32,
                CHAR_HT = 9,
                CHAR_LF = 10,
                CHAR_VT = 11,
                CHAR_NP = 12,
                CHAR_CR = 13
        };
        static int      is_char(char c);
        static int      is_upalpha(char c);
        static int      is_loalpha(char c);
        static int      is_alpha(char c);
        static int      is_digit(char c);
        static int      is_ctl(char c);
        static int      is_hex(char c);
        static int      is_ws(char c);
        static int      is_cr(char c);
        static int      is_lf(char c);
        static int      is_spcr(char c);
        static int      is_splf(char c);
        static int      is_wslfcr(char c);
        static int      is_tspecials(char c);
        static int      is_token(char c);
        static int      is_extra(char c);
        static int      is_safe(char c);
        static int      is_unsafe(char c);
        static int      is_national(char c);
        static int      is_reserved(char c);
        static int      is_unreserved(char c);
        static int      is_punct(char c);
        static int      is_end_of_url(char c);
        static int      is_eow(char c);
        static int      is_wildmat(char c);
        static int      is_sep(char c);
        static int      is_empty(char c);
        static int      is_alnum(char c);
        static int      is_space(char c);
        static int      is_control(char c);
        static int      is_mime_sep(char c);
        static int      is_http_field_name(char c);
        static int      is_http_field_value(char c);
        static int      is_escape(const char *seq);
        static int      is_uchar(const char *seq);
        static int      is_pchar(const char *seq);
        static int      strncasecmp_eow(const char *s1, const char *s2, int n);
        static const char *strcasestr(const char *s1, const char *s2);
        static int      strlen_eow(const char *s);
        static const char *strstr_eow(const char *s1, const char *s2);
        static char     ink_toupper(char c);
        static char     ink_tolower(char c);
        static const char *memchr(const char *s, char c, int max_length);
        static const char *strchr(const char *s, char c);
                        private:
                        ParseRules(const ParseRules &);
                        ParseRules & operator = (const ParseRules &);
};

extern const unsigned int parseRulesCType[];
extern const char parseRulesCTypeToUpper[];
extern const char parseRulesCTypeToLower[];

inline int ParseRules::is_char(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 0)) ? (1) : (0));
}
inline int ParseRules::is_upalpha(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 1)) ? (1) : (0));
}
inline int ParseRules::is_loalpha(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 2)) ? (1) : (0));
}
inline int ParseRules::is_alpha(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 3)) ? (1) : (0));
}
inline int ParseRules::is_digit(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 4)) ? (1) : (0));
}
inline int ParseRules::is_alnum(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 26)) ? (1) : (0));
}
inline int ParseRules::is_ctl(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 5)) ? (1) : (0));
}
inline int ParseRules::is_ws(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 6)) ? (1) : (0));
}
inline int ParseRules::is_hex(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 7)) ? (1) : (0));
}
inline int ParseRules::is_cr(char c) {
  return (c == CHAR_CR);
}
inline int ParseRules::is_lf(char c) {
  return (c == CHAR_LF);
}
inline int ParseRules::is_splf(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 19)) ? (1) : (0));
}
inline int ParseRules::is_spcr(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 18)) ? (1) : (0));
}
inline int ParseRules::is_wslfcr(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 20)) ? (1) : (0));
}
inline int ParseRules::is_extra(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 9)) ? (1) : (0));
}
inline int ParseRules::is_safe(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 10)) ? (1) : (0));
}
inline int ParseRules::is_unsafe(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 11)) ? (1) : (0));
}
inline int ParseRules::is_reserved(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 13)) ? (1) : (0));
}
inline int ParseRules::is_national(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 12)) ? (1) : (0));
}
inline int ParseRules::is_unreserved(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 14)) ? (1) : (0));
}
inline int ParseRules::is_punct(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 15)) ? (1) : (0));
}
inline int ParseRules::is_end_of_url(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 16)) ? (1) : (0));
}
inline int ParseRules::is_escape(const char *seq) {
  return (seq[0] == '%' && is_hex(seq[1]) && is_hex(seq[2]));
}
inline int ParseRules::is_uchar(const char *seq) {
  return (is_unreserved(seq[0]) || is_escape(seq));
}
inline int ParseRules::is_pchar(const char *seq) {
  if (*seq != '%')
    return ((parseRulesCType[*seq] & (1 << 8)) ? (1) : (0));
  else
    return is_hex(seq[1]) && is_hex(seq[2]);
}
inline int ParseRules::is_tspecials(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 17)) ? (1) : (0));
}
inline int ParseRules::is_token(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 22)) ? (1) : (0));
}
inline char ParseRules::ink_toupper(char c) {
  return parseRulesCTypeToUpper[(unsigned char) c];
}
inline char ParseRules::ink_tolower(char c) {
  return parseRulesCTypeToLower[(unsigned char) c];
}
inline int ParseRules::is_eow(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 21)) ? (1) : (0));
}
inline int ParseRules::is_wildmat(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 23)) ? (1) : (0));
}
inline int ParseRules::is_sep(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 24)) ? (1) : (0));
}
inline int ParseRules::is_empty(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 25)) ? (1) : (0));
}
inline int ParseRules::is_space(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 27)) ? (1) : (0));
}
inline int ParseRules::is_control(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 28)) ? (1) : (0));
}
inline int ParseRules::is_mime_sep(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 29)) ? (1) : (0));
}
inline int ParseRules::is_http_field_name(char c) {
  return ((parseRulesCType[(unsigned char) c] & (1 << 30)) ? (1) : (0));
}
inline int ParseRules::is_http_field_value(char c) {
  return ((parseRulesCType[(unsigned char) c] & (((unsigned int)1) << 31)) ? (1) : (0));
}
inline int ParseRules::strncasecmp_eow(
  const char *s1, const char *s2, int count)
{
  for (int i = 0; i < count; i++) {
    const char &a = s1[i];
    const char &b = s2[i];
    if (ink_tolower(a) != ink_tolower(b))
      return (is_eow(a) && is_eow(b));
  }
  return (1);
}
inline int ParseRules::strlen_eow(const char *s){
  for (int i = 0; 1; i++) {
    if (is_eow(s[i]))
      return (i);
  }
}
inline const char *ParseRules::strstr_eow(const char *s1, const char *s2) {
  int i1;
  int s2_len = strlen_eow(s2);
  for (i1 = 0; !is_eow(s1[i1]); i1++)
    if (ink_tolower(s1[i1]) == ink_tolower(s2[0]))
      if (strncasecmp_eow(&s1[i1], &s2[0], s2_len))
        return (&s1[i1]);
  return (0);
}
inline const char *ParseRules::
strcasestr(const char *s1, const char *s2) {
  int i1;
  int s2_len = strlen(s2);
  for (i1 = 0; s1[i1] != '\0'; i1++)
    if (ink_tolower(s1[i1]) == ink_tolower(s2[0]))
      if (strncasecmp_eow(&s1[i1], &s2[0], s2_len))
        return (&s1[i1]);
  return (0);
}
inline const char *ParseRules::memchr(const char *s, char c, int max_length) {
  for (int i = 0; i < max_length; i++)
    if (s[i] == c)
      return (&s[i]);
  return (0);
}
inline const char *ParseRules::strchr(const char *s, char c) {
  for (int i = 0; s[i] != '\0'; i++)
    if (s[i] == c)
      return (&s[i]);
  return (0);
}
inline int
ink_atoi(const char *str) {
  int num = 0;
  int negative = 0;
  while (*str && ParseRules::is_wslfcr(*str))
    str += 1;
  if (*str == '-') {
    negative = 1;
    str += 1;
  }
  while (*str && ParseRules::is_digit(*str))
    num = (num * 10) - (*str++ - '0');
  if (!negative)
    num = -num;
  return num;
}
inline unsigned int
ink_atoui(const char *str) {
  unsigned int num = 0;
  while (*str && ParseRules::is_wslfcr(*str))
    str += 1;
  while (*str && ParseRules::is_digit(*str))
    num = (num * 10) + (*str++ - '0');
  return num;
}
inline int64_t
ink_atoll(const char *str) {
  int64_t num = 0;
  int negative = 0;
  while (*str && ParseRules::is_wslfcr(*str))
    str += 1;
  if (*str == '-') {
    negative = 1;
    str += 1;
  }
  while (*str && ParseRules::is_digit(*str))
    num = (num * 10) - (*str++ - '0');
  if (!negative)
    num = -num;
  return num;
}

#define HRTIME_FOREVER  (10*HRTIME_DECADE)
#define HRTIME_DECADE   (10*HRTIME_YEAR)
#define HRTIME_YEAR     (365*HRTIME_DAY+HRTIME_DAY/4)
#define HRTIME_WEEK     (7*HRTIME_DAY)
#define HRTIME_DAY      (24*HRTIME_HOUR)
#define HRTIME_HOUR     (60*HRTIME_MINUTE)
#define HRTIME_MINUTE   (60*HRTIME_SECOND)
#define HRTIME_SECOND   (1000*HRTIME_MSECOND)
#define HRTIME_MSECOND  (1000*HRTIME_USECOND)
#define HRTIME_USECOND  (1000*HRTIME_NSECOND)
#define HRTIME_NSECOND  (1LL)
#define MAX_FILE_ARGUMENTS 100

char * file_arguments[MAX_FILE_ARGUMENTS] = { 0 };
char * program_name = (char *)"inktomi";
int n_file_arguments = 0;

static char * argument_types_keys = (char *)"ISDfFTL";
static char * argument_types_descriptions[] = {
  (char *)"int  ",
  (char *)"str  ",
  (char *)"dbl  ",
  (char *)"off  ",
  (char *)"on   ",
  (char *)"tog  ",
  (char *)"i64  ",
  (char *)"     "
};

void show_argument_configuration(ArgumentDescription * argument_descriptions,
                                 int n_argument_descriptions);

void usage(ArgumentDescription * argument_descriptions,
           int n_argument_descriptions,
           char * arg_unused);

void process_args(ArgumentDescription * argument_descriptions,
                  int n_argument_descriptions,
                  char **argv);

#ifdef RELEASE
#define ink_assert(EX) (void)(EX)
#define ink_debug_assert(EX)
#else
#define ink_assert(EX) (\
            void)((EX) || (_ink_assert(#EX, __FILE__, __LINE__)))
#define ink_debug_assert(EX) \
            (void)((EX) || (_ink_assert(#EX, __FILE__, __LINE__)))
#endif

#define ink_release_assert(EX) \
            (void)((EX) || (_ink_assert(#EX, __FILE__, __LINE__)))

int _ink_assert(const char * a, const char * f, int l);
void ink_fatal(int return_code, const char *message_format, ...);
void ink_warning(const char *message_format, ...);
#define min(_a,_b) ((_a)<(_b)?(_a):(_b))

inline ink_hrtime ink_get_hrtime()
{
#    if defined(FreeBSD)
      timespec ts;
      clock_gettime(CLOCK_REALTIME, &ts);
      return (ts.tv_sec * HRTIME_SECOND + ts.tv_nsec * HRTIME_NSECOND);
#    else
      timeval tv;
      gettimeofday(&tv,NULL);
      return (tv.tv_sec * HRTIME_SECOND + tv.tv_usec * HRTIME_USECOND);
#    endif
}

typedef struct {
  char sche[MAX_URL_LEN + 1];
  char host[MAX_URL_LEN + 1];
  char port[MAX_URL_LEN + 1];
  char path[MAX_URL_LEN + 1];
  char frag[MAX_URL_LEN + 1];
  char quer[MAX_URL_LEN + 1];
  char para[MAX_URL_LEN + 1];

  int sche_exists;
  int host_exists;
  int port_exists;
  int path_exists;
  int frag_exists;
  int quer_exists;
  int para_exists;

  int rel_url;
  int leading_slash;
  int is_path_name;
} InkWebURLComponents;

static  int ink_web_remove_dots(
  char *src, char *dest, int *leadingslash, int max_dest_len);

static int ink_web_unescapify_string(
  char *dest_in, char *src_in, int max_dest_len);

int ink_web_escapify_string(char *dest_in, char *src_in, int max_dest_len);

static void ink_web_decompose_url(
  char *src_url,
  char *sche, char *host, char *port, char *path,
  char *frag, char *quer, char *para,
  int *real_sche_exists, int *real_host_exists,
  int *real_port_exists, int *real_path_exists, 
  int *real_frag_exists, int *real_quer_exists,
  int *real_para_exists,
  int *real_relative_url, int *real_leading_slash);

static void ink_web_canonicalize_url(char *base_url, char *emb_url, 
                                     char *dest_url, int max_dest_url_len);

static void ink_web_decompose_url_into_structure(char *url, 
                                                 InkWebURLComponents *c);

static void remove_last_seg(char *src, char *dest)
{ 
  char *ptr;
  for (ptr = src + strlen(src) - 1; ptr >= src; ptr--)
    if (*ptr == '/') break;
  while (src <= ptr) *dest++ = *src++;
  *dest = '\0';
}

static inline void remove_multiple_slash(char *src, char *dest)
{ 
  char *ptr=NULL;
  
  for (ptr = src; *ptr;) {
      *(dest++) = *ptr;
      if (*ptr == '/')  {
        while ((*ptr== '/') && *ptr) {
          ptr++;
        }
      } else {
        ptr++;
      }
    }
  *dest = '\0';
}

static inline void append_string(char *dest, char *src, int *offset_ptr, 
                                 int max_len)
{
  int num = strlen(src);
  if (*offset_ptr + num > max_len)
    num = max_len-*offset_ptr;
  strncpy(dest+*offset_ptr, src, num+1);
  (*offset_ptr) += num;
}

// End Library functions

void show_version() {
  char b[] = VERSION_NUM;
  char * v = strchr(b,':');
  v += 2;
  *strchr(v,'$') = 0;
  printf(VERSION, v);
}

void jtest_usage(ArgumentDescription * argument_descriptions, 
                 int n_argument_descriptions, char * arg) 
{
  show_version();
  usage(argument_descriptions, n_argument_descriptions, arg);
}

static void panic(char * s) {
  fputs(s, stderr);
  exit(1);
}

static void panic_perror(char * s) {
  perror(s);
  exit(1);
}

int max_limit_fd() {
  struct rlimit rl;
  if (getrlimit(RLIMIT_NOFILE,&rl) >= 0) {
#ifdef OPEN_MAX
    // Darwin
    rl.rlim_cur = min(OPEN_MAX, rl.rlim_max);
#else
    rl.rlim_cur = rl.rlim_max;
#endif
    if (setrlimit(RLIMIT_NOFILE,&rl) >= 0) {
      if (getrlimit(RLIMIT_NOFILE,&rl) >= 0) {
        return rl.rlim_cur;
      }
    }
  }
  panic_perror("couldn't set RLIMIT_NOFILE\n");
  return -1;
}

int read_ready(int fd) {
  struct pollfd p;
  p.events = POLLIN;
  p.fd = fd;
  int r = poll(&p,1,0);
  if (r<=0) return r;
  if (p.revents & (POLLERR|POLLNVAL)) return -1;
  if (p.revents & (POLLIN|POLLHUP)) return 1;
  return 0;
}

static void poll_init(int sock) { 
  if (!fd[sock].req_header) 
    fd[sock].req_header = (char*)malloc(HEADER_SIZE * pipeline);
  if (!fd[sock].response_header)
    fd[sock].response_header = (char*)malloc(HEADER_SIZE);
  if (!fd[sock].base_url)
    fd[sock].base_url = (char*)malloc(HEADER_SIZE);
  fd[sock].reset(); 
}

static void poll_set(int sock, poll_cb read_cb, poll_cb write_cb = NULL) {
  if (verbose) printf("adding poll %d\n", sock);
  fd[sock].fd = sock;
  fd[sock].read_cb = read_cb;
  fd[sock].write_cb = write_cb;
  if (last_fd < sock)
    last_fd = sock;
}

static void poll_init_set(int sock, poll_cb read_cb, poll_cb write_cb = NULL) {
  poll_init(sock);
  poll_set(sock, read_cb, write_cb);
}

static int fast(int sock, int speed, int d) {
  if (!speed) return 0;
  int64_t t = now - fd[sock].start + 1;
  int target = (int)(((t / HRTIME_MSECOND) * speed) / 1000);
  int delta = d - target;
  if (delta > 0) {
    int mwait = (delta * 1000) / speed;
    fd[sock].ready = now + (mwait * HRTIME_MSECOND);
    return 1;
  } else
    fd[sock].ready = now;
  return 0;
}

static int faster_than(int sock, int speed, int d) {
  if (!speed) return 1;
  int64_t t = now - fd[sock].start + 1;
  int target = (int)(((t / HRTIME_MSECOND) * speed) / 1000);
  int delta = d - target;
  if (delta > 0)
    return 1;
  return 0;
}

static void get_path_from_req(char * buf,char ** purl_start, char ** purl_end)
{
  char * url_start = buf;
  char * url_end = NULL;
  if (!strncasecmp(url_start, "GET ", sizeof("GET ")-1)) {
    url_start += sizeof("GET ")-1;
    url_end = (char*)memchr(url_start,' ',70);
  } else
    url_end = (char*)memchr(url_start,0,70);
  if (!url_end) panic("malformed request\n");
  if (url_end - url_start > 10)
    if (!strncasecmp(url_start, "http://", sizeof("http://")-1)) {
      url_start += sizeof("http://")-1;
      url_start = (char*)memchr(url_start,'/',70);
    }
  *purl_start = url_start;  
  *purl_end = url_end;
}

static int send_response (int sock) {
  int err, towrite;

  if (fd[sock].req_pos >= 0) {
    char header[1024], *url_end = NULL, *url_start = NULL;
    int url_len = 0;
    char* content_type;
    switch (server_content_type) {
    case 1: content_type = "text/html"; break;
    case 2: content_type = "image/jpeg"; break;
    default:
      content_type = 
        (char*)((compd_suite || alternates) ? "image/jpeg" : "text/html");
      if (only_server && strstr(fd[sock].req_header, "Cookie:"))
        content_type = "image/jpeg";
    }
    if (!ftp && embed_url && fd[sock].response_length > 16) {
      get_path_from_req(fd[sock].req_header, &url_start, &url_end);
      *url_end = 0;
      url_len = url_end - url_start;
    }
    int print_len = 0;
    if (!ftp) {
      if (fd[sock].ims) {
        print_len = sprintf(
          header,"HTTP/1.0 304 Not-Modified\r\n"
          "Content-Type: %s\r\n"
          "Last-Modified: Mon, 05 Oct 2010 01:00:00 GMT\r\n"
          "%s"
          "\r\n",
          content_type,
          fd[sock].keepalive>0?"Connection: Keep-Alive\r\n":"");
        url_len = 0;
      } else
        print_len = sprintf(
          header,"HTTP/1.0 200 OK\r\n"
          "Content-Type: %s\r\n"
          "Cache-Control: max-age=630720000\r\n"
          "Last-Modified: Mon, 05 Oct 2010 01:00:00 GMT\r\n"
          "%s"
          "Content-Length: %d\r\n"
          "%s"
          "\r\n%s",
          content_type,
          fd[sock].keepalive>0?"Connection: Keep-Alive\r\n":"",
          fd[sock].response_length,
          no_cache?"Pragma: no-cache\r\nCache-Control: no-cache\r\n":"",
          url_start ? url_start : "");
    } else
      url_len = print_len = 
        sprintf(header, "ftp://%s:%d/%12.10f/%d", 
                local_host, server_port,
                fd[sock].doc, fd[sock].length);
    if (show_headers) printf("Response to Proxy: {\n%s}\n", header);
    int len = print_len - fd[sock].req_pos;
    ink_assert(len>0);
    do {
      err = write(sock, header + fd[sock].req_pos, len);
    } while ((err == -1) && (errno == EINTR));
    if (err <= 0) {
      if (!err) return -1;
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      return -1;
    }
    if (verbose) printf("wrote %d %d\n", sock, err);
    new_tbytes += err;
    fd[sock].req_pos += err;
    fd[sock].bytes += err;
    if (fd[sock].req_pos >= len)
      fd[sock].req_pos = -1;
    else
      return 0;
    fd[sock].response += url_len;
    fd[sock].length -= url_len;
    total_server_response_header_bytes += print_len - url_len;
    total_server_response_body_bytes += url_len;
  }

  /* then the response */
  towrite = server_speed?server_speed:MAX_RESPONSE_LENGTH;
  if (fd[sock].length < towrite)
    towrite = fd[sock].length;
  if (towrite > 0) {
    if (fast(sock,server_speed,fd[sock].bytes)) 
      return 0;
    do {
      err = write (sock, fd[sock].response, towrite);
    } while ((err == -1) && (errno == EINTR));
    if (err < 0) {
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      fprintf(stderr,"write errno %d length %d sock %d\n",errno,towrite,sock);
      errors++;
      return -1;
    }
    if (verbose) printf("wrote %d %d\n", sock, err);

    new_tbytes += err;
    total_server_response_body_bytes += err;
    fd[sock].response += err;
    fd[sock].length -= err;
    fd[sock].bytes += err;
  }
  
  if (fast(sock,server_speed,fd[sock].bytes)) 
    return 0;
  if (fd[sock].length <= 0 || !err) {
    if (fd[sock].response) 
      new_sops++;
    if (verbose) printf("write %d done\n", sock);
    if (fd[sock].keepalive > 0 && !ftp) {
      poll_init_set(sock, read_request);
      fd[sock].start = now;
      fd[sock].ready = now + server_delay * HRTIME_MSECOND;
      return 0;
    }
    return 1;
  }
    
  return 0;
}

char * strncasestr (char *s, char * find, int len) {
  int findlen = strlen(find);
  char * e = s + len;
  while (1) {
    char * x = (char*)memchr(s,*find,e - s);
    if (!x) { 
      if (ParseRules::is_upalpha(*find))
        x = (char*)memchr(s,ParseRules::ink_tolower(*find),e - s);
      else
        x = (char*)memchr(s,ParseRules::ink_toupper(*find),e - s);
      if (!x) break;
    }
    if (!strncasecmp(find, x, findlen)) return x;
    s = x + 1;
  }
  return NULL;
}

static char * check_keepalive(char * r, int length) {
  char * ka = strncasestr(r,"Connection:", length);
  if (ka) {
    int l = length - (ka - r);
    char * e = (char*)memchr(ka,'\n',l);
    if (!e) e = (char*)memchr(ka,'\r',l);
    if (strncasestr(ka,"close",e-ka)) 
      return NULL;
  }
  return ka;
}

static int check_alt(char * r, int length) {
  char * s = strncasestr(r, "Cookie:", length);
  if (!s) {
    s = strncasestr(r, "User-Agent:", length);
    if (s) s += sizeof("User-Agent:");
  } else
    s += sizeof("Cookie:");
  if (s) {
    int l = length - (s - r);
    char * e = (char*)memchr(s, '\n', l);
    if (!e) e = (char*)memchr(s, '\r', l);
    if (!(s = strncasestr(s, "jtest", e - s))) 
      return 0;
    s = (char*)memchr(s, '-', l);
    if (!s)
      return 0;
    s = (char*)memchr(s + 1, '-', l);
    if (!s)
      return 0;
    return ink_atoi(s + 1);
  }
  return 0;
}

static void make_response(int sock, int code) {
  fd[sock].response = fd[sock].req_header;
  fd[sock].length = sprintf( fd[sock].req_header, "%d\r\n", code);
  fd[sock].req_pos = 0;
  fd[sock].response_length = strlen(fd[sock].req_header);
  poll_set(sock, NULL, write_ftp_response);
}

static void make_long_response(int sock) {
  fd[sock].response = fd[sock].req_header;
  fd[sock].req_pos = 0;
  fd[sock].response_length = strlen(fd[sock].req_header);
  poll_set(sock, NULL, write_ftp_response);
}

static int send_ftp_data_when_ready(int sock) {
  if (fd[sock].state == STATE_FTP_DATA_READY && fd[sock].doc_length) {
    fd[sock].response = fd[sock].req_header;
    fd[sock].response_length = fd[sock].length = fd[sock].doc_length;
    if (verbose) printf("ftp data %d >-< %d\n", sock, fd[sock].ftp_data_fd);
    fd[sock].response = response_buffer + fd[sock].doc_length % 256;
    fd[sock].req_pos = 0;
    poll_set(sock, NULL, send_response);
  }
  return 0;
}

static int send_ftp_data(int sock, char * start, char * end) {
  int data_fd = fd[sock].ftp_data_fd;
  if (sscanf(start,"%d",&fd[data_fd].doc_length) != 1)
    return -1;
  fd[data_fd].doc = fd[sock].doc;
  send_ftp_data_when_ready(data_fd);
  return 0;
}

static int read_request(int sock) {
  if (verbose) printf("read_request %d\n", sock);
  int err = 0;
  int i;

  int maxleft = HEADER_SIZE - fd[sock].req_pos - 1;

  do {
    err = read (sock, &fd[sock].req_header[fd[sock].req_pos], 
                maxleft);
  } while ((err < 0) && (errno == EINTR));
  
  if (err < 0) {
    if (errno == EAGAIN || errno == ENOTCONN) return 0;
    if (fd[sock].req_pos || errno != ECONNRESET)
      perror ("read");
    return -1;
  } else if (err == 0) {
    if (verbose) printf("eof\n");
    return -1;
  } else {
    if (verbose) printf("read %d got %d\n", sock, err);
    total_proxy_request_bytes += err;
    new_tbytes += err;
    fd[sock].req_pos += err;
    fd[sock].req_header[fd[sock].req_pos] = 0;
    char *buffer = fd[sock].req_header;
    for (i = fd[sock].req_pos - err; 
         i < fd[sock].req_pos; i++) 
    {
      switch (fd[sock].state) {
        case 0:
          if (buffer[i] == '\r')
            fd[sock].state = 1;
          else if (buffer[i] == '\n')
            fd[sock].state = 2;
          break;
        case 1:
          if (buffer[i] == '\n')
            fd[sock].state = 2;
          else
            fd[sock].state = 0;
          break;
        case 2:
          if (buffer[i] == '\r')
            fd[sock].state = 3;
          else if (buffer[i] == '\n') {
            fd[sock].state = 3;
            goto L3;
          } else
            fd[sock].state = 0;
          break;
      L3:
        case 3:
          if (buffer[i] == '\n') {
            if (show_headers)
              printf("Request from Proxy: {\n%s}\n",buffer);
            char host[80];
            int port, length;
            float r;
            if (sscanf(buffer,"GET http://%[^:]:%d/%f/%d",
                       host,&port,&r,&length) == 4) {
            } else if (sscanf(buffer,"GET /%f/%d",&r,&length) == 2) {
            } else {
              if (verbose) printf("misscan: %s\n",buffer);
              fd[sock].close();
              return 0;
            }
            if (verbose) 
              printf("read_request %d got request %d\n", sock, length);
            char * ims = strncasestr(buffer,"If-Modified-Since:", i);
            if (drand48() > ims_rate) ims = NULL;
            fd[sock].ims = ims?1:0;
            if (!ims) {
              fd[sock].response_length = fd[sock].length = length;
              fd[sock].nalternate = check_alt(fd[sock].req_header, 
                                              strlen(fd[sock].req_header));
              fd[sock].response = response_buffer + length % 256 +
                fd[sock].nalternate;
            } else {
              fd[sock].nalternate = 0;
              if (verbose)
                printf("sending IMS 304: Not-Modified\n");
              fd[sock].response = NULL;
              fd[sock].response_length = fd[sock].length = 0;
            }
            fd[sock].req_pos = 0;
            if (!check_keepalive(fd[sock].req_header, 
                                 strlen(fd[sock].req_header)))
              fd[sock].keepalive = 0;
            else 
              fd[sock].keepalive--;
            if (fd[sock].length && drand48() < server_abort_rate) {
              fd[sock].length = (int)(drand48() * (fd[sock].length -1));
              fd[sock].keepalive = 0;
            }
            poll_set(sock,NULL,send_response);
            return 0;
          } else
            fd[sock].state = 0;
          break;
      }
    }
  }
  return 0;
}

static int send_compd_response(int sock) {
  int err = 0;

  struct {
    unsigned int code;
    unsigned int len;
  } compd_header;
  if (fd[sock].req_pos < sizeof(compd_header)) {
    compd_header.code = 0;
    compd_header.len = htonl((fd[sock].length * 2) / 3);
    do {
      err = write(sock, (char*)&compd_header + fd[sock].req_pos, 
                  sizeof(compd_header) - fd[sock].req_pos);
    } while ((err == -1) && (errno == EINTR));
    if (err <= 0) {
      if (!err) {
        if (verbose_errors) printf("write %d closed early\n", sock);
        goto Lerror;
      }
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      perror("write");
      goto Lerror;
    }
    if (verbose) printf("write %d %d\n", sock, err);

    new_tbytes += err;
    fd[sock].req_pos += err;
    fd[sock].bytes += err;
    fd[sock].response = response_buffer + (((fd[sock].length * 2) / 3) % 256);
  }
 
  if (fd[sock].req_pos < ((fd[sock].length * 2) / 3) + sizeof(compd_header)) {
    int towrite = cbuffersize;
    int desired = ((fd[sock].length * 2) / 3) + sizeof (compd_header) - fd[sock].req_pos;
    if (towrite > desired) {
      towrite = desired;
    }
    if (fast(sock,client_speed,fd[sock].bytes)) return 0;
    do {
      err = write (sock, fd[sock].response + fd[sock].req_pos - sizeof(compd_header) , towrite);
    } while ((err == -1) && (errno == EINTR));
    if (err < 0) {
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      fprintf(stderr,"write errno %d length %d sock %d\n",errno,towrite,sock);
      errors++;
      return -1;
    }
    if (verbose) printf("wrote %d %d\n", sock, err);

    new_tbytes += err;
    total_server_response_body_bytes += err;
    fd[sock].req_pos += err;
    fd[sock].bytes += err;
  }

  if (fd[sock].req_pos >= ((fd[sock].length * 2) / 3) + 4) 
    return -1;
  
  return 0;
Lerror:
  errors++;
  return 1;
}

static int read_compd_request(int sock) {
  if (verbose) printf("read_compd_request %d\n", sock);
  int err = 0;
  int i;

  if (fd[sock].req_pos < 4) {
    int maxleft = HEADER_SIZE - fd[sock].req_pos - 1;
    do {
      err = read (sock, &fd[sock].req_header[fd[sock].req_pos], 
                  maxleft);
    } while ((err < 0) && (errno == EINTR));
  
    if (err < 0) {
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      perror ("read");
      return -1;
    } else if (err == 0) {
      if (verbose) printf("eof\n");
      return -1;
    } else {
      if (verbose) printf("read %d got %d\n", sock, err);
      total_proxy_request_bytes += err;
      new_tbytes += err;
      fd[sock].req_pos += err;
      if (fd[sock].req_pos < 4)
        return 0;
      fd[sock].length = ntohl(*(unsigned int*)fd[sock].req_header);
    }
  }

  if (fd[sock].req_pos >= fd[sock].length + 4) 
    goto Lcont;
  
  {
    char buf[MAX_BUFSIZE];
    int toread = cbuffersize;
    if (fast(sock,client_speed,fd[sock].bytes)) return 0;
    do {
      err = read(sock, buf, toread);
    } while ((err == -1) && (errno == EINTR));
    if (err < 0) {
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      if (errno == ECONNRESET) {
        if (verbose || verbose_errors)
          perror ("read");
        errors++;
        return -1;
      }
      panic_perror("read");
    }
    if (!err) {
      if (verbose || verbose_errors)
        perror ("read");
      errors++;
      return -1;
    }
    total_proxy_request_bytes += err;
    new_tbytes += err;
    fd[sock].req_pos += err;
  }
    
  if (fd[sock].req_pos >= fd[sock].length + 4) 
    goto Lcont;
    
  return 0;

Lcont:
  fd[sock].req_pos = 0;
  fd[sock].keepalive = 0;
  poll_set(sock,NULL,send_compd_response);
  return 0;
}

static int read_ftp_request(int sock) {
  if (verbose) printf("read_ftp_request %d\n", sock);
  int err = 0;
  int i;

  int maxleft = HEADER_SIZE - fd[sock].req_pos - 1;

  do {
    err = read (sock, &fd[sock].req_header[fd[sock].req_pos], 
                maxleft);
  } while ((err < 0) && (errno == EINTR));
  
  if (err < 0) {
    if (errno == EAGAIN || errno == ENOTCONN) return 0;
    perror ("read");
    return -1;
  } else if (err == 0) {
    if (verbose) printf("eof\n");
    return -1;
  } else {
    if (verbose) printf("read %d got %d\n", sock, err);
    new_tbytes += err;
    fd[sock].req_pos += err;
    fd[sock].req_header[fd[sock].req_pos] = 0;
    char *buffer = fd[sock].req_header, *n;
    int res = 0;
    buffer[fd[sock].req_pos] = 0;
    if (verbose) printf("buffer [%s]\n", buffer);
#define STREQ(_x,_s) (!strncasecmp(_x,_s,sizeof(_s)-1))
    if (STREQ(buffer,"USER")) {
        res = 331; goto Lhere;
    } else if (STREQ(buffer,"PASS")) {
        res = 230; goto Lhere;
    } else if (STREQ(buffer,"CWD")) {
      // TS used to send "CWD 1.2110000000..."
      // TS now sends "CWD /1.2110000000^M\n", so skip 5 instead of 4
      fd[sock].doc = (buffer[4]=='/') ? atof(buffer + 5) : atof(buffer + 4);
      res = 250; goto Lhere;
    } else if (STREQ(buffer,"TYPE")) {
        res = 200;
    Lhere:
        n = (char*)memchr(buffer,'\n',fd[sock].req_pos);
        if (!n) return 0;
        make_response(sock,res);
        return 0;
    } else if (STREQ(buffer,"SIZE")) {
        fd[sock].length = 
          sprintf(fd[sock].req_header, "213 %d\r\n", atoi(buffer + 5));
        make_long_response(sock);
        return 0;
    } else if (STREQ(buffer,"MDTM")) {
      double err_rand = 1.0;
      if (ftp_mdtm_err_rate != 0.0) err_rand = drand48();
      if (err_rand < ftp_mdtm_err_rate) {
        fd[sock].length =
          sprintf (fd[sock].req_header, "550 mdtm file not found\r\n");
      } else {
        if (ftp_mdtm_rate == 0) {
          fd[sock].length =
            sprintf (fd[sock].req_header, "213 19900615100045\r\n");
        } else {
          time_t mdtm_now;
          time(&mdtm_now);
          if (mdtm_now-ftp_mdtm_last_update > ftp_mdtm_rate) {
            struct tm *mdtm_tm;
            ftp_mdtm_last_update = mdtm_now;
            mdtm_tm = localtime(&ftp_mdtm_last_update);
            sprintf(ftp_mdtm_str, "213 %.4d%.2d%.2d%.2d%.2d%.2d",
                    mdtm_tm->tm_year + 1900,
                    mdtm_tm->tm_mon + 1,
                    mdtm_tm->tm_mday,
                    mdtm_tm->tm_hour,
                    mdtm_tm->tm_min,
                    mdtm_tm->tm_sec);
          }
          fd[sock].length =
            sprintf (fd[sock].req_header, "%s\r\n", ftp_mdtm_str);
        }
      }
      make_long_response(sock);
      return 0;
    } else if (STREQ(buffer,"PASV")) {
        n = (char*)memchr(buffer,'\n',fd[sock].req_pos);
        if (!n) return 0;
        if ((fd[sock].ftp_data_fd = open_server(0, accept_ftp_data)) < 0)
          panic("could not open ftp data PASV accept port\n");
        fd[fd[sock].ftp_data_fd].ftp_data_fd = sock;
        if (verbose) printf("ftp PASV %d <-> %d\n", sock,fd[sock].ftp_data_fd);
        unsigned short p = fd[fd[sock].ftp_data_fd].name.sin_port;
        fd[sock].length = 
          sprintf(fd[sock].req_header, "227 (%u,%u,%u,%u,%u,%u)\r\n",
                  ((unsigned char*)&local_addr)[0],
                  ((unsigned char*)&local_addr)[1],
                  ((unsigned char*)&local_addr)[2],
                  ((unsigned char*)&local_addr)[3],
                  ((unsigned char*)&p)[0],
                  ((unsigned char*)&p)[1]);
        if (verbose) puts(fd[sock].req_header);
        make_long_response(sock);
        fd[sock].ftp_mode = FTP_PASV;
        return 0;
    } else if (STREQ(buffer,"PORT")) {
        // watch out for an endian problems !!!
        char *start, *stop;
        for (start = buffer; !ParseRules::is_digit(*start); start++);
        for (stop = start; *stop != ','; stop++);
        for (i = 0; i < 4; i++) {
          ((unsigned char*)&(fd[sock].ftp_peer_addr))[i] =
            strtol(start, &stop, 10);
          for (start = ++stop; *stop != ','; stop++);
        }
        ((unsigned char*)&(fd[sock].ftp_peer_port))[0] =
          strtol(start, &stop, 10);
        start = ++stop;
        ((unsigned char*)&(fd[sock].ftp_peer_port))[1] =
          strtol(start, NULL, 10);
        fd[sock].length = 
          sprintf(fd[sock].req_header, "200 Okay\r\n");
        if (verbose) puts(fd[sock].req_header);
        make_long_response(sock);
        fd[sock].ftp_mode = FTP_PORT;
        return 0;
    } else if (STREQ(buffer,"RETR")) {
        if (fd[sock].ftp_mode == FTP_NULL) {
          // default to PORT ftp
          struct sockaddr_in ftp_peer;
          int ftp_peer_addr_len = sizeof(ftp_peer);
          if (getpeername(sock, (struct sockaddr*)&ftp_peer,
#if 0
                          &ftp_peer_addr_len
#else
                          (socklen_t*)&ftp_peer_addr_len
#endif
            ) < 0) {
            perror("getsockname");
            exit(EXIT_FAILURE);
          }
          fd[sock].ftp_peer_addr = ftp_peer.sin_addr.s_addr;
          fd[sock].ftp_peer_port = ftp_peer.sin_port;
          fd[sock].ftp_mode = FTP_PORT;
        }
        if (fd[sock].ftp_mode == FTP_PORT) {
          if ((fd[sock].ftp_data_fd =
               make_client(fd[sock].ftp_peer_addr,fd[sock].ftp_peer_port)) < 0)
            panic("could not open ftp PORT data connection to client\n");
          fd[fd[sock].ftp_data_fd].ftp_data_fd = sock;
          fd[fd[sock].ftp_data_fd].state = STATE_FTP_DATA_READY;
          if (verbose)
            printf("ftp PORT %d <-> %d\n", sock, fd[sock].ftp_data_fd);
        }
        n = (char*)memchr(buffer,'\n',fd[sock].req_pos);
        if (!n) return 0;
        if (send_ftp_data(sock, buffer+5, n)<0) {
          errors++;
          *n = 0;
          if (verbose)
            printf("badly formed ftp request: %s\n", buffer);
          return 1;
        }
        fd[sock].response = fd[sock].req_header;
        fd[sock].length = sprintf( fd[sock].req_header, "150 %d bytes\r\n", 
                                   fd[fd[sock].ftp_data_fd].length);
        fd[sock].req_pos = 0;
        fd[sock].response_length = strlen(fd[sock].req_header);
        poll_set(sock, NULL, write_ftp_response);
        buffer = n+1;
        return 0;
    } else {
      if (verbose || verbose_errors) printf("ftp junk : %s\n", buffer);
      fd[sock].req_pos = 0;
      return 0;
    }
  }
}

static int accept_sock(int sock) {
  struct sockaddr_in clientname;
  int size = sizeof (clientname);
  int new_fd = 0;
  do {
    new_fd = accept(sock, (struct sockaddr *) &clientname,
#if 0
                    &size
#else
                    (socklen_t*)&size
#endif
      );
    if (new_fd < 0) {
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      if (errno == EINTR || errno == ECONNABORTED) continue;
      printf("accept socket was %d\n",sock);
      panic_perror("accept");
    }
  } while (new_fd < 0);

  if (fcntl (new_fd, F_SETFL, O_NONBLOCK) < 0)
    panic_perror("fcntl");

#if 0
#ifdef BUFSIZE                          //  make default
  int bufsize = BUFSIZE;
  if (setsockopt(new_fd,SOL_SOCKET,SO_SNDBUF,
                 (const char *)&bufsize,sizeof(bufsize)) < 0) {
    perror("setsockopt");
  }
  if (setsockopt(new_fd,SOL_SOCKET,SO_SNDBUF,
                 (const char *)&bufsize,sizeof(bufsize)) < 0) {
    perror("setsockopt");
  }
#endif
#endif
  int enable =1;
  if (setsockopt(new_fd,IPPROTO_TCP,TCP_NODELAY,
                 (const char *)&enable,sizeof(enable)) < 0) {
    perror("setsockopt");
  }
#ifdef PRINT_LOCAL_PORT
  struct sockaddr_in local_sa;
  size = sizeof(local_sa);
  getsockname(new_fd, (struct sockaddr*)&local_sa, &size);
  printf("local_sa.sin_port = %d\n", local_sa.sin_port);
#endif
  return new_fd;
}

static int accept_compd (int sock) {
  int new_fd = accept_sock(sock);
  servers++;
  new_servers++;
  poll_init_set(new_fd, NULL, read_compd_request);
  fd[new_fd].count = &servers;
  fd[new_fd].start = now;
  fd[new_fd].ready = now + server_delay * HRTIME_MSECOND;
  fd[new_fd].keepalive = server_keepalive?server_keepalive:INT_MAX;

  return 0;
}

static int accept_read (int sock) {
  int new_fd = accept_sock(sock);
  servers++;
  new_servers++;
  if (ftp) {
    poll_init_set(new_fd, NULL, write_ftp_response);
    make_response(new_fd, 220);
  } else
    poll_init_set(new_fd, read_request);
  fd[new_fd].count = &servers;
  fd[new_fd].start = now;
  fd[new_fd].ready = now + server_delay * HRTIME_MSECOND;
  fd[new_fd].keepalive = server_keepalive?server_keepalive:INT_MAX;

  return 0;
}

static int accept_ftp_data (int sock) {
  int new_fd = accept_sock(sock);
  servers++;
  new_servers++;
  poll_init(new_fd);
  fd[new_fd].ftp_data_fd = fd[sock].ftp_data_fd;
  fd[fd[sock].ftp_data_fd].ftp_data_fd = new_fd;
  fd[new_fd].state = STATE_FTP_DATA_READY;
  fd[new_fd].count = &servers;
  fd[new_fd].start = now;
  fd[new_fd].ready = now + server_delay * HRTIME_MSECOND;
  fd[new_fd].keepalive = server_keepalive?server_keepalive:INT_MAX;
  fd[new_fd].state = STATE_FTP_DATA_READY;
  fd[new_fd].doc = fd[sock].doc;
  fd[new_fd].doc_length = fd[sock].doc_length;
  if (verbose) 
    printf("accept_ftp_data %d for %d\n", new_fd, sock);
  send_ftp_data_when_ready(new_fd);
  return 1;
}

static int open_server(unsigned short int port, accept_fn_t accept_fn) {
  struct linger lngr;
  int sock;
  int one = 1;
  int err = 0;

  /* Create the socket. */
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror ("socket");
    exit (EXIT_FAILURE);
  }
  struct sockaddr_in & name = fd[sock].name;

  /* Give the socket a name. */
  name.sin_family = AF_INET;
  name.sin_port = htons (port);
  name.sin_addr.s_addr = htonl (INADDR_ANY);
  if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *) &one,
                  sizeof (one)) < 0) {
    perror((char*)"setsockopt");
    exit( EXIT_FAILURE);
  }
  if ((err=bind (sock, (struct sockaddr *) &name, sizeof (name))) < 0) {
    if (errno == EADDRINUSE)
      return -EADDRINUSE;
    perror("bind");
    exit( EXIT_FAILURE);
  }

  int addrlen = sizeof(name);
  if ((err = getsockname(sock, (struct sockaddr *) &name, 
#if 0
                         &addrlen
#else
                         (socklen_t*)&addrlen
#endif
    )) < 0) {
    perror("getsockname");
    exit( EXIT_FAILURE);
  }
  ink_assert(addrlen);

  /* Tell the socket not to linger on exit */
  lngr.l_onoff = 0;
  lngr.l_linger = 0;
  if (setsockopt (sock, SOL_SOCKET, SO_LINGER, (char*) &lngr, 
                  sizeof (struct linger)) < 0) {
    perror ("setsockopt");
    exit (EXIT_FAILURE);
  }

  if (listen (sock, 1024) < 0) {
    perror ("listen");
    exit (EXIT_FAILURE);
  }

  /* put the socket in non-blocking mode */
  if (fcntl (sock, F_SETFL, O_NONBLOCK) < 0) {
    perror ("fcntl");
    exit (EXIT_FAILURE);
  }

  if (verbose) 
    printf("opening server on %d port %d\n", sock, name.sin_port);

  poll_init_set(sock, accept_fn);

  return sock;
}

//   perform poll and invoke callbacks on active descriptors 
int poll_loop() {
  if (server_fd > 0) {
    while (read_ready(server_fd) > 0)
      accept_read(server_fd);
  }
  pollfd pfd[POLL_GROUP_SIZE];
  int ip = 0;
  int last_fd_in_block = 0;
  now = ink_get_hrtime();
  for (int i = 0 ; i <= last_fd ; i++) {
    if (fd[i].fd > 0 && (!fd[i].ready || now >= fd[i].ready)) {
      pfd[ip].fd = i;
      pfd[ip].events = 0;
      pfd[ip].revents = 0;
      if (fd[i].read_cb)
        pfd[ip].events |= POLLIN;
      if (fd[i].write_cb)
        pfd[ip].events |= POLLOUT;
      ip++;
    }
    if (ip >= POLL_GROUP_SIZE || i == last_fd) {
      int n = poll(pfd,ip,POLL_TIMEOUT);
      if (n > 0) {
        for (int j = 0; j < ip ; j++) {
          if (pfd[j].revents & (POLLIN|POLLERR|POLLHUP|POLLNVAL)) {
            if (verbose) printf("poll read %d %X\n",pfd[j].fd,pfd[j].revents);
            if (fd[pfd[j].fd].read_cb && fd[pfd[j].fd].read_cb(pfd[j].fd)) {
              fd[pfd[j].fd].close();
              continue;
            }
          }
          if (pfd[j].revents & (POLLOUT|POLLERR|POLLHUP|POLLNVAL)) {
            if (verbose) printf("poll write %d %X\n",pfd[j].fd,pfd[j].revents);
            if (fd[pfd[j].fd].write_cb && fd[pfd[j].fd].write_cb(pfd[j].fd)) {
              fd[pfd[j].fd].close();
              continue;
            }
          }
        }
      }
      ip = 0;
    }
  }
  return 0;
}

int gen_bfc_dist(double f = 10.0) {

  if (docsize >= 0) return docsize;

  double rand = 0.0;
  double rand2 = 0.0;
  bool f_given = f < 9.0;
  if (!f_given) {
    rand = drand48();
    rand2 = drand48();
  } else {
    rand = f;
    rand2 = (f * 13.0) - floor(f * 13.0);
  }

  int class_no;
  int file_no;

  if(rand < 0.35){
    class_no = 0;
  }else if(rand < 0.85){
    class_no = 1;
  }else if (rand < 0.99){
    class_no = 2;
  }else {
    class_no = 3;
    if (f_given) rand2 = (f * 113.0) - floor(f * 113.0);
  }
  
  if(rand2 < 0.018){
    file_no=0;
  }else if(rand2 < 0.091){
    file_no=1;
  }else if(rand2 < 0.237){
    file_no=2;
  }else if(rand2 < 0.432){
    file_no=3;
  }else if(rand2 < 0.627){
    file_no=4;
  }else if(rand2 < 0.783){
    file_no=5;
  }else if(rand2 < 0.887){
    file_no=6;
  }else if(rand2 < 0.945){
    file_no=7;
  }else if(rand2 < 1.000){
    file_no=8;
  }
  int size = 100;
  int i;
  for (i = 0; i < class_no; i++) {
    size = size * 10;
  }
  int increment = size;
  size = size * (file_no+1);
  // vary about the mean doc size for
  // that class/size
  if (!f_given)
    size += (int)((-increment * 0.5) + (increment * drand48()));
  if (verbose) printf("gen_bfc_dist %d\n",size);
  return size;
}

void build_response() {
  int maxsize = docsize > MAX_RESPONSE_LENGTH ? docsize : MAX_RESPONSE_LENGTH;
  response_buffer = (char*) malloc (maxsize + HEADER_SIZE);
  for (int i = 0 ; i < maxsize + HEADER_SIZE; i++)
    response_buffer[i] = i % 256;
}

static void put_ka(int sock) {
  int i = 0;
  for (; i < n_ka_cache; i++ ) 
    if (!ka_cache_head[i] || fd[ka_cache_head[i]].ip == fd[sock].ip) 
      goto Lpush;
  i = n_ka_cache++;
Lpush:  
  if (ka_cache_tail[i])
    fd[ka_cache_tail[i]].next = sock;
  else
    ka_cache_head[i] = sock;
  ka_cache_tail[i] = sock;
}

static int get_ka(unsigned int ip) {
  for (int i = 0; i < n_ka_cache; i++ ) 
    if (fd[ka_cache_head[i]].ip == ip) {
      int res = ka_cache_head[i];
      ka_cache_head[i] = fd[ka_cache_head[i]].next;
      if (res == ka_cache_tail[i]) {
        ink_assert(!ka_cache_head[i]);
        ka_cache_tail[i] = 0;
      }
      return res;
    }
  return -1;
}

static void defer_url(char * url) {
  if (n_defered_urls < MAX_DEFERED_URLS -1)
    defered_urls[n_defered_urls++] = strdup(url);
  else
    fprintf(stderr, "too many defered urls, dropping '%s'\n", url);
}

static int throttling_connections() {
  return client_rate && keepalive_cons && current_clients >= keepalive_cons;
}

static void done() {
  interval_report();
  exit(0);
}

static int is_done() { 
  return 
    (urls_mode && !current_clients && !n_defered_urls) ||
    (bandwidth_test && bandwidth_test_to_go <= 0 && !current_clients);
}

static void undefer_url(bool unthrottled) {
  if ((unthrottled || !throttling_connections()) && n_defered_urls) {
    --n_defered_urls;
    char * url =  defered_urls[n_defered_urls];
    make_url_client(url,0,true,unthrottled);
    free(url);
    if (verbose) 
      printf("undefer_url: made client %d clients\n", current_clients);
  } else
    if (verbose) printf("undefer_url: throttle\n");
  if (is_done())
    done();
}

static void init_client(int sock) {
  poll_init(sock);
  fd[sock].start = now;
  fd[sock].ready = now;
  fd[sock].count = &clients;
  poll_set(sock,NULL,write_request);
}

static unsigned int get_addr(char * host) {
  unsigned int addr = inet_addr(host);
  struct hostent *host_info = NULL;

  if (!addr || (-1 == (int)addr)) {
    host_info = gethostbyname(host);
    if (!host_info) {
      perror ("gethostbyname");
      return (unsigned int)-1;
    }
    addr = *((unsigned int*) host_info->h_addr);
  }

  return addr;
}

char * find_href_end (char *start, int len)
{
  char *end = start;
  if (!start) return NULL;

  while (*end && len > 0) {
      if (*end == '\"') break;  /* " */
      if (*end == '\'') break;                 
      if (*end == '>')  break;
      if (*end == ' ')  break;
      if (*end == '\t') break;
      if (*end == '\n') break;
      if (*end == '<')  break;
      if(*end & 0x80) break; /* hi order bit! */
      len--;
      end++;
    }

  if (*end == 0 || len == 0) 
    return NULL;
  else
    return end;
} // find_href_end

char * find_href_start(char * tag, char *base, int len)
{
  int taglen = strlen(tag);
  if (base == NULL) return NULL;

  char *start = base;
  char *end = base + len;

 Lagain:
  {
    start = strncasestr(start, tag, len);
    if ((start == NULL) || (end-start < 6)) {
      return NULL;
    }
    start += taglen;
    len -= taglen;
  } // block

  while (ParseRules::is_ws (*start) && (end - start > 1)) {
    start++;
    len--;
  }
  if (*start == '=' && (end - start > 1)) {
    start++;
    len--;
  } else {
    goto Lagain;
  }
  while (ParseRules::is_ws(*start) && (end - start > 1)) {
    start++;
    len--;
  }
  //
  // Optional quotes:  href="x" or href='x' or href=x
  //
  if ((*start == '\"'||(*start == '\'')) && (end - start > 1)) { /*"'*/
    start++;
    len--;
  }
  while (ParseRules::is_ws(*start) && (end - start > 1)) {
    start++;
    len--;
  }

  return start;
} // find_href_start

int compose_url(char * new_url, char * base, char *input) {
  char sche[8],host[512],port[10],path[512],frag[512],quer[512],para[512];
  char curl[512];
  int xsche,xhost,xport,xpath,xfrag,xquer,xpar,rel,slash;
  ink_web_decompose_url(base,sche,host,port,path,frag,quer,para,
                        &xsche,&xhost,&xport,&xpath,&xfrag,&xquer,
                        &xpar,&rel,&slash);
  strcpy(curl, "http://");
  strcat(curl,host);
  if (xport) {
    strcat(curl,":");
    strcat(curl,port);
  }
  strcat(curl,"/");
  strcat(curl,path);

  ink_web_canonicalize_url(curl, input, new_url, 512);
  return 0;
} // compose_urls

void compose_all_urls( char * tag, char * buf, char * start, char * end, 
                       int buflen, char * base_url)
{
  char old;
  while ((start = find_href_start(tag, end, buflen - (end - buf)))) {
    char newurl[512];
    end = (char *) find_href_end(start, min(buflen - (start-buf), 512 - 10)); 
    if (!end) { 
      end = start + strlen(tag);
      continue;
    }
    old = *end;
    *end = 0;
    compose_url(newurl, base_url, start);
    make_url_client(newurl,base_url);
    *end = old;
  } // while
}
//
// Input is a NULL-terminated string (buf of buflen)
//       also, a read-write base_url
//
void extract_urls(char * buf, int buflen, char * base_url) {
  //if (verbose) printf("EXTRACT<<%s\n>>", buf);
  char old;
  char *start = NULL;
  char *end = NULL;
  char old_base[512];
  strcpy(old_base, base_url);

  start = strncasestr(buf, "<base ", buflen);
  if (start) {
    end = (char*)memchr(start, '>', buflen - (start - buf));
    if (end) {
      char * rover = strncasestr(start, "href", end - start);
      if (rover) {
        rover += 4;
        while (rover < end && 
               (ParseRules::is_ws(*rover) || *rover == '=' || *rover == '\'' 
                || *rover == '\"'))    /* " */
          rover++;
        start = rover;
        while (rover < end && 
               !(ParseRules::is_ws(*rover) || *rover == '\'' 
                 || *rover == '\"'))
          rover++;
        old = *rover;
        *rover = 0;
        compose_url(base_url,old_base,start);
        // fixup unqualified hostnames (e.g. http://internal/foo)
        char * he = strchr(base_url + 8, '/'); 
        if (!memchr(base_url,'.',he-base_url)) {
          char t[512]; strcpy(t,base_url);
          char * old_he = strchr(old_base + 8, '.');
          if (old_he) {
            char * old_hee = strchr(old_he, '/');
            if (old_hee) {
              memcpy(base_url,t,(he-base_url));
              memcpy(base_url + (he-base_url), old_he, (old_hee-old_he));
              memcpy(base_url + (he-base_url) + (old_hee-old_he),
                     t+(he-base_url), strlen(t+(he-base_url)));
              base_url[(he-base_url) + (old_hee-old_he) +
                      strlen(t+(he-base_url))] = 0;
            }}}}}}

  end = buf;
  if (follow)
    compose_all_urls("href", buf, start, end, buflen, base_url);
  if (fullpage) {
    char *tags[] = { "src", "image", "object", "archive", "background", 
                     // "location", "code" 
    };
    for (int i = 0 ; i < sizeof(tags)/sizeof(tags[0]) ; i++)
      compose_all_urls(tags[i], buf, start, end, buflen, base_url);
  }
} // extract_urls

void follow_links(int sock) {
  if (urls_mode) {
    if (fd[sock].binary) return;
    int l = fd[sock].response_remaining;
    char * r = fd[sock].response, *p = r, *n = r;
    if (r)
      extract_urls(r,l,fd[sock].base_url);
    if (l < MAX_BUFSIZE) {
      while (n) {
        n = (char*)memchr(p,'\n',l - (p - r));
        if (!n) n = (char*)memchr(p,'\r',l - (p - r));
        if (n) p = n + 1;
      }
      int done = p - r, remaining = l - done;
      if (done) {
        memmove(r, p, remaining);
        fd[sock].response_remaining = remaining;
      }
    } else  // bail
      fd[sock].response_length = 0;
  }
}

static int verify_content(int sock, char * buf, int done) {
  if (urls_mode && !check_content)
    return 1;
  int l = fd[sock].response_length;
  char * d = response_buffer + (l % 256) + fd[sock].nalternate;
  int left = fd[sock].length;
  if (left > 0) {
    if (embed_url && !fd[sock].jg_compressed) {
      if (l == left && left > 64) {
        char *url_end = NULL, *url_start = NULL;
        get_path_from_req(fd[sock].base_url,&url_start,&url_end);
        if (url_end - url_start < done) {
          if (memcmp(url_start,buf,url_end - url_start)) 
            return 0;
        }
      }
      // skip past the URL which is embedded in the document
      // to confound the fingerprinting code
      if (l - left < 64) {
        int skip = 64 - (l - left);
        left -= skip;
        done -= skip;
        buf += skip;
        if (done < 0)
          done = 0;
      }
    }
    if (!check_content)
      return 1;
    if (done > left)
      done = left;
    if (memcmp(buf, d + (fd[sock].response_length - left), done))
      return 0;
  }
  return 1;
}

#define ZIPF_SIZE (1<<20)
static double *zipf_table = NULL;
void build_zipf() {
  zipf_table = (double*)malloc(ZIPF_SIZE * sizeof(double));
  for (int i = 0; i < ZIPF_SIZE; i++)
    zipf_table[i] = 1.0 / pow(i+2, zipf);
  for (int i = 1; i < ZIPF_SIZE; i++)
    zipf_table[i] = zipf_table[i-1] + zipf_table[i];
  double x = zipf_table[ZIPF_SIZE-1];
  for (int i = 0; i < ZIPF_SIZE; i++)
    zipf_table[i] = zipf_table[i] / x;
}

int get_zipf(double v) {
  int l = 0, r = ZIPF_SIZE-1, m;
  do {
    m = (r + l) / 2;
    if (v < zipf_table[m])
      r = m - 1;
    else
      l = m + 1;
  } while (l < r);
  if (zipf_bucket_size == 1)
    return m;
  double x = zipf_table[m], y = zipf_table[m+1];
  m += (v - x) / (y - x);
  return m;
}

static int read_response_error(int sock) {
  errors++;
  fd[sock].close();
  if (!urls_mode)
    make_bfc_client(proxy_addr, proxy_port);
  return 0;
}

static int read_for_error(int sock) {
  char b[8192];
  int err = read(sock, &b, 8192);
  if (err < 0)
    return read_response_error(sock);
  return 0;
}

static int read_response(int sock) {
  int err = 0;

  if (fd[sock].req_pos >= 0) {
    if (!fd[sock].req_pos) 
      memset(fd[sock].req_header,0,HEADER_SIZE);
    do {
      int l = HEADER_SIZE - fd[sock].req_pos - 1;
      if (l <= 0) { 
        if (verbose || verbose_errors) 
          printf("header too long '%s'", fd[sock].req_header);
        return read_response_error(sock);
      }
      err = read(sock, fd[sock].req_header + fd[sock].req_pos, 
                 HEADER_SIZE - fd[sock].req_pos - 1);
    } while ((err == -1) && (errno == EINTR));
    if (err <= 0) {
      if (!err) {
        if (verbose_errors) 
          printf("read_response %d closed during header for '%s' after %d%s\n",
                 sock, fd[sock].base_url, fd[sock].req_pos,
                 (keepalive && (fd[sock].keepalive != keepalive) 
                  && !fd[sock].req_pos) ? " -- keepalive timeout" : "");
        return read_response_error(sock);
      }
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      if (errno == ECONNRESET) {
        if (!fd[sock].req_pos && keepalive > 0 && 
            fd[sock].keepalive != keepalive) {
          fd[sock].close();
          if (!urls_mode)
            make_bfc_client(proxy_addr, proxy_port);
          return 0;
        }
        if (verbose || verbose_errors)
          perror ("read");
        goto Ldone;
      }
      panic_perror("read");
    }
    if (verbose) 
      printf("read %d header %d [%s]\n", 
             sock, err, fd[sock].req_header);
    b1_ops++;

    strcpy(fd[sock].response_header, fd[sock].req_header);

    b1latency += ((ink_get_hrtime() - fd[sock].start) / HRTIME_MSECOND);
    new_cbytes += err;
    new_tbytes += err;
    fd[sock].req_pos += err;
    fd[sock].bytes += err;
    fd[sock].active = ink_get_hrtime();
    int total_read = fd[sock].req_pos;
    char * p = fd[sock].req_header;
    char * cl = NULL;
    int cli = 0;
    while ((p = strchr(p,'\n'))) {
      if (verbose) printf("read header end? [%s]\n", p);
      if (p[1] == '\n' || (p[1] == '\r' && p[2] == '\n')) {
        int off = 1 + (p[1] == '\r' ? 2 : 1);
        p += off;
        strncpy(fd[sock].response_header, fd[sock].req_header, p - fd[sock].req_header);
        fd[sock].response_header[p - fd[sock].req_header] = '\0';
        int lbody =  fd[sock].req_pos - (p - fd[sock].req_header);
        cl = strncasestr(fd[sock].req_header,"Content-Length:", 
                         p - fd[sock].req_header);
        if (cl) {
          cli = atoi(cl + 16);
            int expected_length = fd[sock].response_length;
            if (compd_suite) {
              if (strstr(fd[sock].req_header, "x-jg")) {
                fd[sock].jg_compressed = 1;
                expected_length = (fd[sock].response_length * 2) / 3;
              }
            } 
            if (fd[sock].response_length && verbose_errors &&
                expected_length != cli && !nocheck_length) 
              fprintf(stderr, "bad Content-Length expected %d got %d orig %d",
                      expected_length, cli, fd[sock].response_length);
            fd[sock].response_length = fd[sock].length = cli;
        }
        if (fd[sock].req_header[9] == '2') {
          if (!verify_content(sock,p,lbody)) {
            if (verbose || verbose_errors)
              printf("content verification error '%s'\n", fd[sock].base_url);
            return read_response_error(sock);
          }
        }
        total_proxy_response_body_bytes += lbody;
        total_proxy_response_header_bytes += p - fd[sock].req_header;
        fd[sock].length -= lbody;
        fd[sock].req_pos = -1;
        if (fd[sock].length && drand48() < client_abort_rate) {
          fd[sock].client_abort = 1;
          fd[sock].length = (int)(drand48() * (fd[sock].length -1)); 
          fd[sock].keepalive = 0;
          fd[sock].drop_after_CL = 1;
        }
        if (verbose) printf("read %d header done\n", sock);
        break;
      }
      p++;
    }
    if (!p) return 0;
    int hlen = p -  fd[sock].req_header;
    if (show_headers) {
      printf("Response From Proxy: {\n");
      for (char * c = fd[sock].req_header; c < p ; c++)
        putc(*c,stdout);
      printf("}\n");
    }
    if (obey_redirects && urls_mode && 
        fd[sock].req_header[9] == '3' &&
        fd[sock].req_header[10] == '0' &&
        (fd[sock].req_header[11] == '1' || fd[sock].req_header[11] == '2')) 
    {
      char * redirect = strstr(fd[sock].req_header,"http://");
      char * e = redirect?(char*)memchr(redirect,'\n',hlen):0;
      if (!redirect || !e)
        fprintf(stderr, "bad redirect '%s'",fd[sock].req_header);
      else {
        if (e[-1]=='\r') e--;
        *e = 0;
        make_url_client(redirect);
      }
      fd[sock].close();
      return 0;
    }
    if (fd[sock].req_header[9] != '2') {
      if (verbose_errors) {
        char * e = (char*)memchr(fd[sock].req_header, '\r', hlen);
        if (e) *e = 0;
        else {
          char * e = (char*)memchr(fd[sock].req_header, '\n', hlen);
          if (e) *e = 0;
          else *p = 0;
        }
        printf("error response %d: '%s':'%s'\n", sock, 
               fd[sock].base_url, fd[sock].req_header);
      }
      return read_response_error(sock);
    }
    char * r = fd[sock].req_header;
    int length = p - r;
    char * ka = check_keepalive(r, length);
    if (urls_mode) {
      fd[sock].response_remaining = total_read - length;
      if (fd[sock].response_remaining) 
        memcpy(fd[sock].response,p,fd[sock].response_remaining);
      if (check_content && !cl) {
        if (verbose || verbose_errors)
          printf("missiing Content-Length '%s'\n", fd[sock].base_url);
        return read_response_error(sock);
      }
    } else
      fd[sock].response = 0;      
    if (!cl || !ka)
      fd[sock].keepalive = -1;
    if (!cl)
      fd[sock].length = INT_MAX;
  }
  
  if (fd[sock].length <= 0 && 
      (fd[sock].keepalive > 0 || fd[sock].drop_after_CL))
    goto Ldone;

  {
    char * r = NULL;
    char buf[MAX_BUFSIZE];
    int toread = cbuffersize;
    if (urls_mode) {
      if (fd[sock].response_remaining + cbuffersize < MAX_BUFSIZE)
        r = fd[sock].response + fd[sock].response_remaining;
      else {
        toread = MAX_BUFSIZE - fd[sock].response_remaining;
        if (!toread) {
          if (verbose_errors || verbose)
            fprintf(stderr,"line exceeds buffer, unable to follow links\n");
          toread = cbuffersize;
          r = fd[sock].response;
          fd[sock].response_remaining = 0;
        } else
          r = fd[sock].response + fd[sock].response_remaining;
      }
    } else
      r = buf;
    if (fast(sock,client_speed,fd[sock].bytes)) return 0;
    if (fd[sock].bytes > abort_retry_bytes && 
        (((now - fd[sock].start + 1)/HRTIME_SECOND) > abort_retry_secs) && 
        !faster_than(sock,abort_retry_speed,fd[sock].bytes)) 
    {
      fd[sock].client_abort = 1;
      fd[sock].keepalive = 0;
      if (!urls_mode && !client_rate)
        make_bfc_client(proxy_addr, proxy_port);
      goto Ldone;
    }
    do {
      err = read(sock, r, toread);
    } while ((err == -1) && (errno == EINTR));
    if (err < 0) {
      if (errno == EAGAIN || errno == ENOTCONN) return 0;
      if (errno == ECONNRESET) {
        if (verbose || verbose_errors)
          perror ("read");
        goto Ldone;
      }
      panic_perror("read");
    }
    if (!err)
      goto Ldone;
    if (!verify_content(sock,buf,err)) {
      if (verbose || verbose_errors)
        printf("content verification error '%s'\n", fd[sock].base_url);
      return read_response_error(sock);
    }
    total_proxy_response_body_bytes += err;
    new_cbytes += err;
    new_tbytes += err;
    fd[sock].response_remaining += err;
    fd[sock].bytes += err;
    follow_links(sock);
    if (fd[sock].length != INT_MAX)
      fd[sock].length -= err;
    fd[sock].active = ink_get_hrtime();
    if (verbose) 
      printf("read %d got %d togo %d %d %d\n", sock, err, fd[sock].length,
             fd[sock].keepalive, fd[sock].drop_after_CL);
  }
  
  if (fd[sock].length <= 0 && 
      (fd[sock].keepalive > 0 || fd[sock].drop_after_CL))
    goto Ldone;

  return 0;
  
Ldone:
  int ok = false;
  if (!fd[sock].client_abort && 
      !(server_abort_rate > 0) &&
      fd[sock].length && fd[sock].length != INT_MAX) 
  {
    if (verbose || verbose_errors) 
      printf("bad length %d wanted %d after %d ms: '%s'\n", 
             fd[sock].response_length - fd[sock].length,
             fd[sock].response_length,
             (int)((ink_get_hrtime() - fd[sock].active)/HRTIME_MSECOND),
             fd[sock].base_url);
    return read_response_error(sock);
  }
  if (verbose) printf("read %d done\n", sock);
  new_ops++;
  double thislatency =((ink_get_hrtime() - fd[sock].start) / HRTIME_MSECOND);
  latency += (int)thislatency;
  lat_ops++;
  if (fd[sock].keepalive > 0) {
    fd[sock].reset();
    put_ka(sock);
    current_clients--;
    if (urls_mode) {
      undefer_url();
      return 0;
    }
  } else
    fd[sock].close();
  if (!urls_mode && !client_rate)
    make_bfc_client(proxy_addr, proxy_port);
  return 0;
}
  
static int write_request(int sock) {
  int err = 0;
  
  do {
    err = write(sock, fd[sock].req_header + fd[sock].req_pos, 
                fd[sock].length - fd[sock].req_pos);
  } while ((err == -1) && (errno == EINTR));
  if (err <= 0) {
    if (!err) {
      if (verbose_errors) printf("write %d closed early\n", sock);
      goto Lerror;
    }
    if (errno == EAGAIN || errno == ENOTCONN) return 0;
    perror("write");
    goto Lerror;
  }
  if (verbose) printf("write %d %d\n", sock, err);

  new_tbytes += err;
  total_client_request_bytes += err;
  fd[sock].req_pos += err;
  fd[sock].active = ink_get_hrtime();
  
  if (fd[sock].req_pos >= fd[sock].length) {
    if (verbose) printf("write complete %d %d\n", sock, fd[sock].length);
    fd[sock].req_pos = 0;
    fd[sock].length = fd[sock].response_length;
    poll_set(sock, read_response);
  }
  return 0;
Lerror:
  errors++;
#ifndef RETRY_CLIENT_WRITE_ERRORS
  if (!--nclients)
    panic("no more clients\n");
  return 1;
#else
  if (!urls_mode)
    make_bfc_client(proxy_host, proxy_port);
  fd[sock].close();
  return 0;
#endif
}

static int write_ftp_response(int sock) {
  int err = 0;
  
  do {
    err = write(sock, fd[sock].req_header + fd[sock].req_pos, 
                fd[sock].length - fd[sock].req_pos);
  } while ((err == -1) && (errno == EINTR));
  
  if (err <= 0) {
    if (!err) {
      if (verbose_errors) printf("write %d closed early\n", sock);
      goto Lerror;
    }
    if (errno == EAGAIN || errno == ENOTCONN) return 0;
    perror("write");
    goto Lerror;
  }
  if (verbose) printf("write %d %d\n", sock, err);

  new_tbytes += err;
  fd[sock].req_pos += err;
  
  if (fd[sock].req_pos >= fd[sock].length) {
    if (verbose) printf("write complete %d %d\n", sock, fd[sock].length);
    fd[sock].req_pos = 0;
    fd[sock].length = fd[sock].response_length;
    poll_set(sock, read_ftp_request);
  }
  return 0;
Lerror:
  errors++;
  return 1;
}

static int make_client (unsigned int addr, int port) {
  struct linger lngr;

  int sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    panic_perror ("socket");

  if (fcntl (sock, F_SETFL, O_NONBLOCK) < 0)
    panic_perror ("fcntl");

  /* tweak buffer size so that remote end can't close connection too fast */

#if 0
  int bufsize = cbuffersize;
  if (setsockopt(sock,SOL_SOCKET,SO_RCVBUF,
                 (const char *)&bufsize,sizeof(bufsize)) < 0)
    panic_perror("setsockopt");
  if (setsockopt(sock,SOL_SOCKET,SO_SNDBUF,
                 (const char *)&bufsize,sizeof(bufsize)) < 0)
    panic_perror("setsockopt");
#endif
  int enable =1;
  if (setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,
                 (const char *)&enable,sizeof(enable)) < 0)
    panic_perror("setsockopt");

  /* Tell the socket not to linger on exit */
  lngr.l_onoff = 1;
  lngr.l_linger = 0;
  if (!ftp) {  // this causes problems for PORT ftp -- ewong
    if (setsockopt (sock, SOL_SOCKET, SO_LINGER, (char*) &lngr, 
                    sizeof (struct linger)) < 0) {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }
  }

  /* Give the socket a name. */
  struct sockaddr_in name;
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = addr;
  
  if (verbose) printf("connecting to %u.%u.%u.%u:%d\n",
                      ((unsigned char*)&addr)[0], ((unsigned char*)&addr)[1], 
                      ((unsigned char*)&addr)[2], ((unsigned char*)&addr)[3],
                      port);
  
  while (connect (sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
    if (errno == EINTR) continue;
    if (errno == EINPROGRESS)
      break;
    if (verbose_errors) fprintf(stderr, "connect failed errno = %d\n", errno);
    errors++;
    return -1;
  }

  init_client(sock);
  fd[sock].ip = addr;
  clients++;
  current_clients++;
  new_clients++;
  return sock;
}

static void make_bfc_client (unsigned int addr, int port) {
  int sock = -1;
  if (bandwidth_test && bandwidth_test_to_go-- <= 0)
    return;
  if (keepalive) 
    sock = get_ka(addr);
  if (sock < 0) {
    sock = make_client(addr,port);
    fd[sock].keepalive = keepalive;
  } else {
    init_client(sock);
    current_clients++;
    fd[sock].keepalive--;
  }
  if (sock<0) 
    panic("unable to open client connection\n");
  double h = drand48();
  double dr = drand48();
  if (zipf == 0.0) {
    if (h < hitrate) {
      dr = 1.0 + (floor(dr * hotset) / hotset);
      fd[sock].response_length = gen_bfc_dist(dr - 1.0);
    } else
      fd[sock].response_length = gen_bfc_dist(dr);
  } else {
    unsigned long long int doc = get_zipf(dr);
    // Some large randomish number.
    unsigned long long int doc_len_int = doc * 0x14A4D0FB0E93E3A7LL;
    unsigned long int x = doc_len_int;
    double y = (double)x;
    y /= 0x100000000LL;  // deterministic random number between 0 and 1.0
    fd[sock].response_length = gen_bfc_dist(y);
    dr = doc;
  }
  if (verbose) printf("gen_bfc_dist %d\n", fd[sock].response_length);
  char eheaders[16384];
  *eheaders = 0;
  int nheaders = extra_headers;
  if (nheaders>0) {
    char * eh = eheaders;
    if (!vary_user_agent) {
      eh += sprintf(eh, "User-Agent: Mozilla/4.04 [en] (X11; I; Linux 2.0.31 i586)\r\n");
      nheaders--;
    }
    if (nheaders>0)
      eh += sprintf(eh, "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\r\n");
    while (--nheaders>0)
      eh += sprintf(eh, "Extra-Header%d: a lot of junk for header %d\r\n",
                    nheaders, nheaders);
  }
  char cookie[256];
  *cookie = 0;
  fd[sock].nalternate = (int)(alternates * drand48());
  if (alternates)
    if (!vary_user_agent)
      sprintf(cookie, "Cookie: jtest-cookie-%d\r\n", fd[sock].nalternate);
    else
      sprintf(cookie, "User-Agent: jtest-browser-%d\r\n", fd[sock].nalternate);
  char* extension;
  switch (request_extension) {
    case 1: extension = (char*)".html"; break;
    case 2: extension = (char*)".jpeg"; break;
    case 3: extension = (char*)"/"; break;
    default:
      extension = (char*)(compd_suite ? ".jpeg" : "");
  }
  char evo_str[20];
  evo_str[0] = '\0';
  if (evo_rate != 0.0) {
    double evo_index = dr + (((double)now)/HRTIME_HOUR) * evo_rate;
    sprintf(evo_str, ".%u", ((unsigned int)evo_index));
  }
  if (0 == hostrequest) {
    sprintf(fd[sock].req_header, 
            ftp ? 
            "GET ftp://%s:%d/%12.10f/%d%s%s HTTP/1.0\r\n"
            "%s"
            "%s"
            "%s"
            "%s"
            "\r\n" 
            :
            "GET http://%s:%d/%12.10f/%d%s%s HTTP/1.0\r\n"
            "%s"
            "%s"
            "%s"
            "%s"
            "\r\n"
            ,
            local_host, server_port, dr,
            fd[sock].response_length, evo_str, extension,
            fd[sock].keepalive?"Proxy-Connection: Keep-Alive\r\n":"",
            reload_rate > drand48() ? "Pragma: no-cache\r\n":"",
            eheaders, cookie
      );
  } else if (1 == hostrequest) {
    sprintf(fd[sock].req_header, 
            "GET /%12.10f/%d%s%s HTTP/1.0\r\n"
            "Host: %s:%d\r\n"
            "%s"
            "%s"
            "%s"
            "%s"
            "\r\n",
            dr, fd[sock].response_length, evo_str, extension,
            local_host, server_port,
            fd[sock].keepalive?"Connection: Keep-Alive\r\n":"",
            reload_rate > drand48() ? "Pragma: no-cache\r\n":"",
            eheaders, cookie);
  } else if (2 == hostrequest) {
    /* Send a non-proxy client request i.e. for Transparency testing */
    sprintf(fd[sock].req_header, 
            "GET /%12.10f/%d%s%s HTTP/1.0\r\n"
            "%s"
            "%s"
            "%s"
            "%s"
            "\r\n",
            dr, fd[sock].response_length, evo_str, extension,
            fd[sock].keepalive?"Connection: Keep-Alive\r\n":"",
            reload_rate > drand48() ? "Pragma: no-cache\r\n":"",
            eheaders, 
            cookie);
  }
  if (verbose) printf("request %d [%s]\n", sock, fd[sock].req_header);
  fd[sock].length = strlen(fd[sock].req_header); 
  {
    char * s = fd[sock].req_header;
    char * e = (char*)memchr(s, '\r', 512);
    char * url = fd[sock].base_url;
    memcpy(url,s,e-s);
    url[e-s] = 0;
    if (show_before) printf("%s\n", url);
  }
  if (show_headers) printf("Request to Proxy: {\n%s}\n", fd[sock].req_header);
}

#define RUNNING(_n) \
  total_##_n = (((total_##_n * (average_over-1))/average_over) + new_##_n); \
  running_##_n =  total_##_n / average_over; \
  new_##_n = 0;

#define RUNNING_AVG(_t,_n,_o) \
  _t = _o?((_t * (average_over-1) + _n/_o) / average_over):_t; \
  _n = 0;

void interval_report() {
  static int here = 0;
  now = ink_get_hrtime();
  if (!(here++ % 20))
    printf(
 " con  new     ops   1B  lat      bytes/per     svrs  new  ops      total   time  err\n");
  RUNNING(clients);
  RUNNING_AVG(running_latency,latency,lat_ops); lat_ops = 0;
  RUNNING_AVG(running_b1latency,b1latency,b1_ops); b1_ops = 0;
  RUNNING(cbytes);
  RUNNING(ops);
  RUNNING(servers);
  RUNNING(sops);
  RUNNING(tbytes);
  float t = (float)(now - start_time);
  uint64_t per = current_clients ? running_cbytes / current_clients : 0;
  printf("%4d %4d %7.1f %4d %4d %10" PRIu64"/%-6" PRIu64"  %4d %4d %4d  %9" PRIu64" %6.1f %4d\n",
         current_clients, // clients, n_ka_cache,
         running_clients,
         running_ops, running_b1latency, running_latency,
         running_cbytes, per,
         running_servers, 
         running_servers,
         running_sops, running_tbytes, 
         t/((float)HRTIME_SECOND),
         errors);
  if (is_done()) {
    printf("Total Client Request Bytes:\t\t%" PRIu64"\n", total_client_request_bytes);
    printf("Total Server Response Header Bytes:\t%" PRIu64"\n",
           total_server_response_header_bytes);
    printf("Total Server Response Body Bytes:\t%" PRIu64"\n",
           total_server_response_body_bytes);
    printf("Total Proxy Request Bytes:\t\t%" PRIu64"\n", total_proxy_request_bytes);
    printf("Total Proxy Response Header Bytes:\t%" PRIu64"\n",
           total_proxy_response_header_bytes);
    printf("Total Proxy Response Body Bytes:\t%" PRIu64"\n",
           total_proxy_response_body_bytes);
  }
}

#define URL_HASH_ENTRIES     url_hash_entries
#define BYTES_PER_ENTRY      3
#define ENTRIES_PER_BUCKET   16
#define OVERFLOW_ENTRIES     1024                           // many many

#define BUCKETS              (URL_HASH_ENTRIES / ENTRIES_PER_BUCKET)
#define BYTES_PER_BUCKET     (BYTES_PER_ENTRY * ENTRIES_PER_BUCKET)
#define URL_HASH_BYTES       \
            (BYTES_PER_ENTRY * (URL_HASH_ENTRIES + OVERFLOW_ENTRIES))
#define CHANCE_OF_COLLISION  \
            (((double)ENTRIES_PER_BUCKET)/((double)(1<<(8*3))))
#define WANT_FILESIZE        0

// NOTE: change to match BYTES_PER_ENTRY
#define ENTRY_TAG(_x)               \
  (((unsigned int)_x[0] << 16) +    \
   ((unsigned int)_x[1] << 8) +     \
   (unsigned int)_x[2])
#define SET_ENTRY_TAG(_x,_t) \
  _x[0] = _t >> 16;          \
  _x[1] = (_t >> 8) & 0xFF;  \
  _x[2] = _t & 0xFF;

#define MASK_TAG(_x)  (_x & ((1U << (BYTES_PER_ENTRY * 8)))-1)

#define BEGIN_HASH_LOOP                                            \
    unsigned int bucket = (i % BUCKETS);                           \
    unsigned int tag = MASK_TAG((unsigned int)(i / BUCKETS));      \
    if (!tag) tag++;                                               \
    unsigned char * base = bytes + bucket * BYTES_PER_BUCKET;      \
    unsigned char * last = bytes + (bucket+1) * BYTES_PER_BUCKET - \
                           BYTES_PER_ENTRY;                        \
    (void)last;                                                    \
                                                                   \
    for (unsigned int x = 0 ; x < ENTRIES_PER_BUCKET ; x++) {      \
      unsigned char * e = base + x * BYTES_PER_ENTRY;

#define BEGIN_OVERFLOW_HASH_LOOP                                     \
    for (unsigned int j = 0 ; j < ENTRIES_PER_BUCKET ; j++) {        \
      unsigned char * e = base + (URL_HASH_ENTRIES + j) * BYTES_PER_ENTRY;

#define END_HASH_LOOP \
    }

struct UrlHashTable {

  unsigned int    numbytes;
  unsigned char * bytes;
  int fd;
  
  void zero() { memset(bytes, 0, numbytes); }

  void alloc(unsigned int want);

  void set(uint64_t i) {
    BEGIN_HASH_LOOP {
      if (!ENTRY_TAG(e)) {
        SET_ENTRY_TAG(e,tag);
        return;
      }
    } END_HASH_LOOP;

    fprintf(stderr, "url hash table overflow: %X, %X\n", (int)(base-bytes), tag);

    BEGIN_OVERFLOW_HASH_LOOP {
      if (!ENTRY_TAG(e)) {
        SET_ENTRY_TAG(e,tag);
        return;
      }
    } END_HASH_LOOP;

    ink_fatal(1, "overview entries overflow");
  }

  void clear(uint64_t i) {
    BEGIN_HASH_LOOP {
      if (ENTRY_TAG(e) == tag) {
        if (e != last)
          SET_ENTRY_TAG(e,ENTRY_TAG(last));
        SET_ENTRY_TAG(last,0);
        return;
      }
    } END_HASH_LOOP;

    fprintf(stderr, "url hash table entry to clear not found: %X, %X\n", 
            (int)(base-bytes), tag);
  }

  int is_set(uint64_t i) {
    BEGIN_HASH_LOOP {
      if (ENTRY_TAG(e) == tag)
        return 1;
    } END_HASH_LOOP;

    if (ENTRY_TAG((last))) {
      BEGIN_OVERFLOW_HASH_LOOP {
        if (ENTRY_TAG(e) == tag)
          return 1;
      } END_HASH_LOOP;
    }
    return 0;
  }
  
  UrlHashTable();

  ~UrlHashTable();
};
UrlHashTable * uniq_urls = NULL;


UrlHashTable::UrlHashTable() {
  unsigned long len = 0;

  if (!url_hash_entries)
    return;

  if (*url_hash_filename) {
    ink_assert((fd = open(url_hash_filename,O_RDWR|O_CREAT, 0644))>0);
    len = lseek(fd, 0, SEEK_END);
  }

  if (url_hash_entries > 0) {
    // if they specify the number of entries round it up
    url_hash_entries = 
      (url_hash_entries + ENTRIES_PER_BUCKET - 1) & ~(ENTRIES_PER_BUCKET - 1);
    numbytes = URL_HASH_BYTES;

    // ensure it is either a new file or the correct size
    if (len != 0 && len != numbytes) 
      panic("specified size != file size\n");

  } else {

    // otherwise make sure the file is non-zero and then use its
    // size as the size
    if (!len) 
      panic("zero size URL Hash Table\n");
    if (len != URL_HASH_BYTES) {
      fprintf(stderr, 
              "FATAL: hash file length (%lu) != URL_HASH_BYTES (%lu)\n",
              len, (unsigned long)URL_HASH_BYTES);
      exit(1);
    }
    numbytes = len;
  }

  if (*url_hash_filename) {
    ink_assert( !ftruncate(fd,numbytes) );
    bytes = (unsigned char *)
      mmap(NULL,numbytes,PROT_READ|PROT_WRITE,
           MAP_SHARED|MAP_NORESERVE,
           fd, 0);
    if (bytes == (unsigned char*)MAP_FAILED || !bytes)
      panic("unable to map URL Hash file\n");
  } else {
    bytes = (unsigned char *)malloc(numbytes);
    ink_assert(bytes);
    zero();
  }
} // UrlHashTable::UrlHashTable


UrlHashTable::~UrlHashTable()
{
  ink_assert(!munmap((char*)bytes, numbytes));
  ink_assert(!close(fd));
} // UrlHashTable::~UrlHashTable


int seen_it(char * url) {
  if (!url_hash_entries)
    return 0;
  union {
    unsigned char md5[16];
    uint64_t i[2];
  } u;
  int l = 0;
  char * para = strrchr(url, '#');
  if (para) 
    l = para - url;
  else
    l = strlen(url);
  ink_code_md5((unsigned char*)url,l,u.md5);
  uint64_t x = u.i[0] + u.i[1];
  if (uniq_urls->is_set(x)) {
    if (verbose) printf("YES: seen it '%s'\n", url);
    return 1;
  }
  uniq_urls->set(x);
  if (verbose) printf("NO: marked it '%s'\n", url);
  return 0;
}

static int make_url_client(char * url,char * base_url, bool seen,
                           bool unthrottled) 
{
  int iport = 80;
  unsigned int ip = 0;
  char hostname[80],curl[512];
  char sche[8],host[512],port[10],path[512],frag[512],quer[512],para[512];
  int xsche,xhost,xport,xpath,xfrag,xquer,xpar,rel,slash;

  if (base_url) {
    ink_web_canonicalize_url(base_url, url, curl, 512);
    // hack for our own web server!
    if (curl[strlen(curl)-1] == 13)
      curl[strlen(curl)-1] = 0;
    if (curl[strlen(curl)-1] == 12)
      curl[strlen(curl)-1] = 0;
  } else
    strcpy(curl,url);
  if (!seen && seen_it(curl))
    return -1;
  ink_web_decompose_url(curl,sche,host,port,path,frag,quer,para,
                        &xsche,&xhost,&xport,&xpath,&xfrag,&xquer,
                        &xpar,&rel,&slash);
  if (follow_same) {
    if (!xhost || strcasecmp(host,current_host)) {
      if (verbose) printf("skipping %s\n",curl);
      return -1;
    }
  }
  if (!unthrottled && throttling_connections()) {
    defer_url(curl);
    return -1;
  }
  if (proxy_port) {
    iport = proxy_port;
    ip = proxy_addr;
  } else {
    if (xport) iport = atoi(port);
    if (!xhost) { 
      if (verbose) fprintf(stderr, "bad url '%s'\n", curl); 
      return -1;
    }
    ip = get_addr(host);
    if ((int)ip == -1) { 
      if (verbose || verbose_errors)
        fprintf(stderr, "bad host '%s'\n", host); 
      return -1;
    }
  }
  int sock = -1;
  if (keepalive) 
    sock = get_ka(ip);
  if (sock < 0) {
    sock = make_client(ip, iport);
    fd[sock].keepalive = keepalive;
  } else {
    init_client(sock);
    current_clients++;
    fd[sock].keepalive--;
  }
  if (sock < 0)
    panic("cannot make client\n");
  char eheaders[16384];
  *eheaders = 0;
  int nheaders = extra_headers;
  memset(&eheaders,0,16384);
  if (nheaders>0) {
    char * eh = eheaders;
    if (!vary_user_agent) {
      eh += sprintf(eh, "User-Agent: Mozilla/4.04 [en] (X11; I; Linux 2.0.31 i586)\r\n");
      nheaders--;
    }
    if (nheaders>0)
      eh += sprintf(eh, "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\r\n");
    while (--nheaders>0)
      eh += sprintf(eh, "Extra-Header%d: a lot of junk for header %d\r\n",
                    nheaders, nheaders);
  }
  if (proxy_port)
    sprintf(fd[sock].req_header, "GET %s HTTP/1.0\r\n"
            "%s"
            "%s"
            "Accept: */*\r\n"
            "%s"
            "\r\n", 
            curl,
            reload_rate > drand48() ? "Pragma: no-cache\r\n":"",
            fd[sock].keepalive?"Proxy-Connection: Keep-Alive\r\n":"",eheaders);
  else 
    sprintf(fd[sock].req_header, "GET /%s%s%s%s%s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "%s"
            "%s"
            "Accept: */*\r\n"
            "%s"
            "\r\n", 
            path,xquer?"?":"",quer,xpar?";":"",para,host,
            reload_rate > drand48() ? "Pragma: no-cache\r\n":"",
            fd[sock].keepalive?"Connection: Keep-Alive\r\n":"",eheaders);

  if (verbose) printf("curl = '%s'\n",curl);
  if (show_before) printf("%s\n", curl);
  if (urlsdump_fp) fprintf(urlsdump_fp, "%s\n", curl);
  if (show_headers) printf("Request to Proxy: {\n%s}\n",fd[sock].req_header);
  char * epath = path + strlen(path);
  fd[sock].binary = 
    !strncasecmp(path - 3, "gif", 3) || !strncasecmp(path - 3, "jpg", 3);
  fd[sock].response_length = 0;
  fd[sock].length = strlen(fd[sock].req_header);
  if (!fd[sock].response)
    fd[sock].response = (char*)malloc(MAX_BUFSIZE);
  strcpy(fd[sock].base_url, curl);
  return sock;
}

static FILE * get_defered_urls(FILE * fp) {
  char url[512];
  while (fgets(url,512,fp)) {
    if (n_defered_urls > MAX_DEFERED_URLS -2)
      return NULL;
    char * e = (char*)memchr(url,'\n', 512);
    if (e) *e = 0;
    make_url_client(url);
  }
  return fp;
}

int main(int argc, char *argv[]) {
  /* for QA -- we want to be able to tail an output file
   * during execution "nohup jtest -P pxy -p prt &"
   */
  setvbuf(stdout, (char*) NULL, _IOLBF, 0);

  fd = (FD*)malloc(MAXFDS * sizeof(FD));
  memset(fd,0,MAXFDS*sizeof(FD));
  process_args(argument_descriptions, n_argument_descriptions, argv);
  if (version) {
    show_version();
    exit(0);
  }
  if (!drand_seed) 
    srand48((long)time(NULL));
  else
    srand48((long)drand_seed);
  if (zipf != 0.0)
    build_zipf();
  int max_fds = max_limit_fd();
  if (verbose) printf ("maximum of %d connections\n",max_fds);
  signal(SIGPIPE,SIG_IGN);      
  start_time = now = ink_get_hrtime();
  
  urls_mode = n_file_arguments || *urls_file;
  nclients = client_rate? 0 : nclients;

  if (!local_host[0])
    ink_assert(!gethostname(local_host,80));
  local_addr = get_addr(local_host);
  if (!proxy_host[0])
    strcpy(proxy_host, local_host);
  if (proxy_port) {
    proxy_addr = get_addr(proxy_host);
  }

  if (!urls_mode) {
    if (compd_port) {
      build_response();
      open_server(compd_port, accept_compd);
    } else {
      if (!server_port)
        server_port = proxy_port + 1000;
      build_response();
      if (!only_clients) {
        for (int retry = 0 ; retry < 20 ; retry++) {
          server_fd = open_server(server_port + retry, accept_read);
          if (server_fd < 0) {
            if (server_fd == -EADDRINUSE) { 
              printf("here\n");
              continue;
            }
            panic_perror("open_server");
          }
          break;
        }
      }
      bandwidth_test_to_go = bandwidth_test;
      if (!only_server) {
        if (proxy_port) {
          for (int i = 0 ; i < nclients ; i++)
            make_bfc_client(proxy_addr, proxy_port);
        }
      }
    }
  } else {
    if (check_content)
      build_response();
    follow = follow_arg;
    follow_same = follow_same_arg;
    uniq_urls = new UrlHashTable;
    defered_urls = (char**)malloc(sizeof(char*) * MAX_DEFERED_URLS);
    average_over = 1;
    if (*urlsdump_file) {
      urlsdump_fp = fopen(urlsdump_file,"w");
      if (!urlsdump_fp)
        panic_perror("fopen urlsdump file");
    }
    if (*urls_file) {
      FILE * fp = fopen(urls_file,"r");
      if (!fp)
        panic_perror("fopen urls file");
      if (get_defered_urls(fp))
        fclose(fp);
      else
        urls_fp = fp;
    }
    int i;
    for (i = 0; i < n_file_arguments; i++) {
      char sche[8],host[512],port[10],path[512],frag[512],quer[512],para[512];
      int xsche,xhost,xport,xpath,xfrag,xquer,xpar,rel,slash;
      ink_web_decompose_url(file_arguments[i],sche,host,port,
                            path,frag,quer,para,
                            &xsche,&xhost,&xport,&xpath,&xfrag,&xquer,
                            &xpar,&rel,&slash);
      if (xhost) {
        strcpy(current_host,host);
      }
    }
    for (i = 0; i < n_file_arguments ; i++)
      make_url_client(file_arguments[i]);
  }

  int t = now / HRTIME_SECOND;
  int tclient = now / HRTIME_SECOND;
  int start = now / HRTIME_SECOND;
  while (1) {
    if (poll_loop()) break;
    int t2 = now / HRTIME_SECOND;
    if (urls_fp && n_defered_urls < MAX_DEFERED_URLS - DEFERED_URLS_BLOCK - 2){
      if (get_defered_urls(urls_fp)) {
        fclose(urls_fp);
        urls_fp = NULL;
      }
    }
    if ((!urls_mode || client_rate) && interval && t + interval <= t2) {
      t = t2;
      interval_report();
    }
    if (t2 != tclient) {
      for (int i = 0; i < client_rate * (t2 - tclient) ; i++)
        if (!urls_mode)
          make_bfc_client(proxy_addr, proxy_port);
        else
          undefer_url(true);
      tclient = t2;
    }
    if (test_time)
      if (t2 - start > test_time)
        done();
    if (is_done())
      done();
  }

  return 0;
}


/*---------------------------------------------------------------------------*
  
  int ink_web_decompose_url(...)
  
  This function takes an input URL in src_url and splits it into its
  component parts, including a scheme, host, port, path, fragment,
  query, and parameters. you must pass in buffers for each of these.
  If you pass in a NULL pointer for any of these, it will not be
  returned.
  
  The flags "sche_exists", etc. tell you if that part of the URL was
  found. Each unfound part (with a non-NULL buffer) will also contain
  the empty string '\0'.
  
  The flag "relative_url" indicates that the src_url did not start
  with a scheme. (This is kind of redundant with sche_exists but is
  the general way to do it.)
  
  The flag "leading_slash" indicates that the path began with a
  leading slash.
  
  mep - 4/15/96
  
  *---------------------------------------------------------------------------*/

void ink_web_decompose_url(char *src_url,
                           char *sche, char *host, char *port, char *path,
                           char *frag, char *quer, char *para,
                           int *real_sche_exists, int *real_host_exists,
                           int *real_port_exists, int *real_path_exists, 
                           int *real_frag_exists, int *real_quer_exists,
                           int *real_para_exists,
                           int *real_relative_url, int *real_leading_slash)
  /*
      * Input: src_url
      * Outputs: every other argument
      *
      * You may pass in NULL pointers for any of: sche, host, port, path,
      * frag, quer, or para, and they will not be returned.
      * 
      *
      * According to the HTML Sourcebook, a URL consists:
      *
      *   http://www.address.edu:80/path/subdir/file.ext?query;params#fragment
      *   aaaa   bbbbbbbbbbbbbbb cc dddddddddddddddddddd eeeee ffffff gggggggg
      *
      * where
      *   a = scheme
      *   b = host
      *   c = port
      *   d = path
      *   e = query
      *   f = params
      *   g = fragment
      *
      * Order of parsing is: fragment, scheme, host, port, params, query, path
      *
      * Note that the hostname:port part may contain something like:
      *   user@pass:hostname:port
      *   bbbbbbbbbbbbbbbbbb cccc
      * i.e., the port is the thing after the _last_ colon in this part
      *
      */
{
  char *start = src_url;
  int len = strlen(src_url);
  char *end = start + len;
  char *ptr = start;
  char *ptr2, *temp, *temp2;
  char *sche1, *sche2;
  char *host1, *host2;
  char *port1, *port2;
  char *path1, *path2;
  char *frag1, *frag2;
  char *quer1, *quer2;
  char *para1, *para2;
  int fail = 0;
  int num;
  int sche_exists, host_exists, port_exists, path_exists, frag_exists, 
    quer_exists, para_exists;
  int leading_slash;
  
  sche_exists = host_exists = port_exists = path_exists = 0;
  frag_exists = quer_exists = para_exists = 0;
  sche1 = sche2 = host1 = host2 = port1 = port2 = path1 = path2 = NULL;
  frag1 = frag2 = quer1 = quer2 = para1 = para2 = NULL;
  leading_slash = 0;
  
  temp2 = ptr;
  /* strip fragments "#" off the end */
  while (ptr < end) {
    if (*ptr == '#') {
      frag1 = ptr+1;
      frag2 = end;
      frag_exists = 1;
      end = ptr;
    }
    ptr++;
  }
  ptr = temp2;

  /* decide if there is a sche, i.e. if it's an absolute url */
  /* find end of sche */
  fail = 0;
  temp2 = ptr;
  while ((ptr < end) && !fail) {
    if (*ptr == ':') {
      sche1 = start;
      sche2 = ptr;
      ptr++; /* to continue to parse, skip the : */
      sche_exists = 1;
      fail = 1;
    } else if ((!ParseRules::is_alpha(*ptr) &&
                (*ptr != '+') && (*ptr != '.') && (*ptr != '-')) ||
               (ptr == end)) {
      sche_exists = 0;
      fail = 1;
    } else {
      ptr++;
    }
  }
  if (sche_exists == 0)
    ptr = temp2;
  
  /* find start of host */
  fail = 0;
  temp2 = ptr;
  while ((ptr < end-1) && !fail) {
    if (*(ptr+0) == '/') {
      if (*(ptr+1) == '/') {
        host1 = ptr+2;
        ptr += 2; /* skip "//" */
        host_exists = 1;
        fail = 1;
      } else {
        /* this is the start of a path, not a host */
        host_exists = 0;
        fail = 1;
      }
    } else {
      ptr++;
    }
  }
  
  /* find end of host */
  if (host_exists == 1) {
    while ((ptr < end) && (host2 == NULL)) {
      if (*ptr == '/') {
        /* "/" marks the start of the path */
        host2 = ptr; /* just so we quit out of the loop */
      } else {
        ptr++;
      }
    }
    if (host2 == NULL)
      host2 = end;
    
    if (host_exists == 1) {
      temp = host2-1;
      /* remove trailing dots from host */
      while ((temp > host1) && (*temp == '.')) {
        temp--;
        host2--;
      }
      
      /* find start & end of port */
      ptr2 = host1;
      temp = host2;
      while (ptr2 < temp) {
        if (*ptr2 == ':') {
          port1 = ptr2+1;
          port2 = temp;
          host2 = ptr2;
          port_exists = 1;
        }
        ptr2++;
      }
    }
  }
  if (host_exists == 0)
    ptr = temp2;
  
  temp2 = ptr;
  /* strip query "?" off the end */
  while (ptr < end) {
    if (*ptr == '?') {
      quer1 = ptr+1;
      quer2 = end;
      quer_exists = 1;
      end = ptr;
    }
    ptr++;
  }
  ptr = temp2;
  
  temp2 = ptr;
  /* strip parameters ";" off the end */
  while (ptr < end) {
    if (*ptr == ';') {
      para1 = ptr+1;
      para2 = end;
      para_exists = 1;
      end = ptr;
    }
    ptr++;
  }
  ptr = temp2;

  /* the path is the remainder of the string */
  /* don't include any leading slash */
  if (ptr < end) {
    if (*ptr == '/') {
      leading_slash = 1;
      path1 = ptr+1;
      path2 = end;
      path_exists = 1;
    } else {
      path1 = ptr;
      path2 = end;
      path_exists = 1;
    }
  }
  else {
    path1 = end;
    path2 = end;
    path_exists = 0;
  } 
  
  if (sche_exists != 1) 
    *real_relative_url = 1; 
  else
    *real_relative_url = 0;
  
  /* extract strings for scheme, host, port, path, etc */
  
  if (sche != NULL) {
    if (sche_exists) {
      num = sche2-sche1;
      if (num > MAX_URL_LEN-1) num = MAX_URL_LEN-1;
      strncpy(sche,sche1,num+1);
      *(sche+num) = '\0';
      
      /* make scheme lowercase */
      char * p = sche;
      while (*p) {
        *p = ParseRules::ink_tolower(*p);
        p++;
      }
    } else {
      *sche = 0;
    }
  }
  
  if (host != NULL) {
    if (host_exists) {
      num = host2-host1;
      if (num > MAX_URL_LEN-1) num = MAX_URL_LEN-1;
      strncpy(host,host1,num+1);
      *(host+num) = '\0';
      
      /* make hostname lowercase */
      char * p = host;
      while (*p) {
        *p = ParseRules::ink_tolower(*p);
        p++;
      }
    } else {
      *host = 0;
    }
  }
  
  if (port != NULL) {
    if (port_exists) {
      num = port2-port1;
      if (num > MAX_URL_LEN-1) num = MAX_URL_LEN-1;
      strncpy(port,port1,num+1);
      *(port+num) = '\0';
    } else {
      *port = 0;
    }
  }
  
  if (path != NULL) {
    if (path_exists) {
      num = path2-path1;
      if (num > MAX_URL_LEN-1) num = MAX_URL_LEN-1;
      strncpy(path,path1,num+1);
      *(path+num) = '\0';
    } else {
      *path = 0;
    }
  }
  
  if (frag != NULL) {
    if (frag_exists) {
      num = frag2-frag1;
      if (num > MAX_URL_LEN-1) num = MAX_URL_LEN-1;
      strncpy(frag,frag1,num+1);
      *(frag+num) = '\0';
    } else {
      *frag = 0;
    }
  }
  
  if (quer != NULL) {
    if (quer_exists) {
      num = quer2-quer1;
      if (num > MAX_URL_LEN-1) num = MAX_URL_LEN-1;
      strncpy(quer,quer1,num+1);
      *(quer+num) = '\0';
    } else {
      *quer = 0;
    }
  }
  
  if (para != NULL) {
    if (para_exists) {
      num = para2-para1;
      if (num > MAX_URL_LEN-1) num = MAX_URL_LEN-1;
      strncpy(para,para1,num+1);
      *(para+num) = '\0';
    } else {
      *para = 0;
    }
  }
  *real_sche_exists = sche_exists;
  *real_host_exists = host_exists;
  *real_port_exists = port_exists;
  *real_path_exists = path_exists;
  *real_frag_exists = frag_exists;
  *real_quer_exists = quer_exists;
  *real_para_exists = para_exists;
  *real_leading_slash = leading_slash; 
} /* End ink_web_decompose_url */


/*---------------------------------------------------------------------------*
  
  void ink_web_dump_url_components(FILE *fp, InkWebURLComponents *c)
  
  This routine writes a readable representation of the URL components
  pointed to by <c> on the file pointer <fp>.
  
  *---------------------------------------------------------------------------*/

void ink_web_dump_url_components(FILE *fp, InkWebURLComponents *c)
{
  fprintf(fp,"sche:'%s', exists %d\n",c->sche,c->sche_exists);
  fprintf(fp,"host:'%s', exists %d\n",c->host,c->host_exists);
  fprintf(fp,"port:'%s', exists %d\n",c->port,c->port_exists);
  fprintf(fp,"path:'%s', exists %d\n",c->path,c->path_exists);
  fprintf(fp,"quer:'%s', exists %d\n",c->quer,c->quer_exists);
  fprintf(fp,"frag:'%s', exists %d\n",c->frag,c->frag_exists);
  fprintf(fp,"para:'%s', exists %d\n",c->para,c->para_exists);
  
  fprintf(fp,"rel_url:%d\n",c->rel_url);
  fprintf(fp,"leading_slash:%d\n",c->leading_slash);
  
  fprintf(fp,"\n");
} /* End ink_web_dump_url_components */


/*---------------------------------------------------------------------------*
  
  int ink_web_canonicalize_url(...)
  
  Inputs: base_url, emb_url, max_dest_url_len.
  Output: dest_url.
  
  This function takes a base url and an embedded url, and produces an
  absolute url as specified in RFC 1808, "Relative Uniform Resource
  Locators".
  
  A base url is often the url of a document and an embedded url is an
  incomplete reference to a secondary document, often in the same
  directory. Together they completely specify an absolute reference to
  the secondary document.
  
  For instance, 
     base_url "http://inktomi.com/~mep"
     emb_url: "path1/path2/foo.html"
  
  becomes
  
     dest_url: "http://inktomi.com/~mep/path1/path2/foo.html"
  
  This function also applies "ink_web_escapify()" to the dest_url.
  
  You must supply the buffer dest_url and its size, max_dest_url_len.
  
  
  mep - 4/15/96
  
  *---------------------------------------------------------------------------*/

void ink_web_canonicalize_url(char *base_url, char *emb_url, char *dest_url,
                              int max_dest_url_len)
{
  int doff;
  InkWebURLComponents base,emb;
  char temp[MAX_URL_LEN + 1],temp2[MAX_URL_LEN + 1];
  int leading_slash,use_base_sche,use_base_host,
    use_base_path,use_base_quer,use_base_para,use_base_frag;
  int host_last = 0;
  
  doff = 0;
  
  /* Initialize Component Values */
  
  leading_slash = 0;
  
  /* Decompose The Base And Embedded URLs */
  
  ink_web_decompose_url_into_structure(base_url,&base);
  ink_web_decompose_url_into_structure(emb_url,&emb);
  
  /* Print Out Components */
  
  /* Select Which Components To Use From Base & Embedded URL */
  
  dest_url[0] = '\0';
  
  use_base_sche = 1;
  use_base_host = 1;
  use_base_path = 0;
  use_base_quer = 0;
  use_base_para = 0;
  use_base_frag = 0;
  
  if (!emb.sche_exists && !emb.path_exists && 
      !emb.host_exists && !emb.quer_exists && 
      !emb.frag_exists && !emb.para_exists)
  {
    /* 2a: if the embedded URL is empty, take everything from the base */
      
    use_base_sche = 1;
    use_base_host = 1;
    use_base_path = 1;
    use_base_quer = 1;
    use_base_para = 1;
    use_base_frag = 1;
  }
  else if (emb.sche_exists &&
           ((strcasecmp(emb.sche,"telnet") == 0) ||
            (strcasecmp(emb.sche,"mailto") == 0) ||
            (strcasecmp(emb.sche,"news") == 0)))
  {
    char * p = emb_url, * q = dest_url;
    while (*p)
      *q++ = ParseRules::ink_tolower(*p++);
    return;
  }
  else if (emb.sche_exists &&
           !(((strcasecmp(emb.sche,"http") == 0) && !emb.host_exists)))
    
  {
    /* 2b: not good enough, because things like 'http:overview.html' */
      
    use_base_sche = 0;
    use_base_host = 0;
    use_base_path = 0;
    use_base_quer = 0;
    use_base_para = 0;
    use_base_frag = 0;
  }
  else
  {
    use_base_sche = 1;
      
    /* step 3 - if emb_host non-empty, skip to 7 */
      
    if (emb.host_exists)
    {
      use_base_host = 0;
    }
    else
    {
      use_base_host = 1;
          
      /* step 4 - if emb_path preceeded by slash, skip to 7 */
          
      if (emb.leading_slash != 1)
      {
        /* step 5 */
              
        if (!emb.path_exists)
        {
          use_base_path = 1;
                  
          if (emb.para_exists)
          {
            /* 5a - if emb_para non-empty, skip to 7 */
                      
            use_base_para = 0;
          }
          else
          {
            /* otherwise use base_para */
                      
            use_base_para = 1;
                      
            if (emb.quer_exists)
            {
              /* 5b - if emb_quer non-empty, skip to 7 */
                          
              use_base_quer = 0;
            }
            else
            {
              /* otherwise use base query */
                          
              use_base_quer = 1;
            }
          }
        }
        else
        {
          use_base_path = 0;
                  
          /* step 6 */
          /* create combined path */
          /* remove last segment of base_path */
                  
          remove_last_seg(base.path, temp);
          remove_multiple_slash(temp, temp2);
                  
          /* append emb_path */
                  
          sprintf(temp2,"%s%s",temp2,emb.path);

          /* remove "." and ".." */
                  
          ink_web_remove_dots(temp2,emb.path,
                              &leading_slash,MAX_URL_LEN);
          emb.path_exists = 1;
          emb.leading_slash = base.leading_slash;
        } /* 5 */
      } /* 4 */
    } /* 3 */
  }
  
  /* step 7 - combine parts */
  
  if (use_base_sche)
  {
    if (base.sche_exists)
    {
      append_string(dest_url, base.sche, &doff, MAX_URL_LEN);
      append_string(dest_url, ":", &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  else
  {
    if (emb.sche_exists)
    {
      append_string(dest_url, emb.sche, &doff, MAX_URL_LEN);
      append_string(dest_url, ":", &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  
  if (use_base_host)
  {
    if (base.host_exists)
    {
      append_string(dest_url, "//", &doff, MAX_URL_LEN);
      append_string(dest_url, base.host, &doff, MAX_URL_LEN);
      if ((base.port_exists) && (strcmp(base.port,"80") != 0))
      {
        append_string(dest_url, ":", &doff, MAX_URL_LEN);
        append_string(dest_url, base.port, &doff, MAX_URL_LEN);
      }
      host_last = 1;
    }
  }
  else
  {
    if (emb.host_exists)
    {
      append_string(dest_url, "//", &doff, MAX_URL_LEN);
      append_string(dest_url, emb.host, &doff, MAX_URL_LEN);
      if ((emb.port_exists) && (strcmp(emb.port,"80") != 0))
      {
        append_string(dest_url, ":", &doff, MAX_URL_LEN);
        append_string(dest_url, emb.port, &doff, MAX_URL_LEN);
      }
      host_last = 1;
    }
  }
  
  if (use_base_path)
  {
    if (base.path_exists)
    {
      if (base.leading_slash)
        append_string(dest_url, "/", &doff, MAX_URL_LEN);

      ink_web_unescapify_string(temp, base.path, MAX_URL_LEN);
      ink_web_escapify_string(base.path, temp, max_dest_url_len);
      append_string(dest_url, base.path, &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  else
  {
    if (emb.path_exists)
    {
      if (emb.leading_slash)
        append_string(dest_url, "/", &doff, MAX_URL_LEN);
      ink_web_unescapify_string(temp, emb.path, MAX_URL_LEN);
      ink_web_escapify_string(emb.path, temp, max_dest_url_len);
      append_string(dest_url, emb.path, &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  
  if (use_base_para)
  {
    if (base.para_exists)       {
      append_string(dest_url, ";", &doff, MAX_URL_LEN);
      append_string(dest_url, base.para, &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  else
  { 
    if (emb.para_exists)
    {
      append_string(dest_url, ";", &doff, MAX_URL_LEN);
      append_string(dest_url, emb.para, &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  
  if (use_base_quer)
  {
    if (base.quer_exists)
    {
      append_string(dest_url, "?", &doff, MAX_URL_LEN);
      append_string(dest_url, base.quer, &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  else
  {
    if (emb.quer_exists)
    {
      append_string(dest_url, "?", &doff, MAX_URL_LEN);
      append_string(dest_url, emb.quer, &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  
  if (use_base_frag)
  {
    if (base.frag_exists)
    {
      append_string(dest_url, "#", &doff, MAX_URL_LEN);
      append_string(dest_url, base.frag, &doff, MAX_URL_LEN);
      host_last = 0;
    }
  } 
  else {
    if (emb.frag_exists)
    {
      append_string(dest_url, "#", &doff, MAX_URL_LEN);
      append_string(dest_url, emb.frag, &doff, MAX_URL_LEN);
      host_last = 0;
    }
  }
  
  if (host_last) append_string(dest_url, "/", &doff, MAX_URL_LEN);
}

/*---------------------------------------------------------------------------*
  
  int ink_web_decompose_url_into_structure(char *url, InkWebURLComponents *c)
  
  This routine takes a URL and violently tears apart its molecular structure,
  placing the URL components in the InkWebURLComponents structure pointed to
  by <c>.  Flags in the structure indicate whether individual fields are
  valid or not.
  
  *---------------------------------------------------------------------------*/

void ink_web_decompose_url_into_structure(char *url, InkWebURLComponents *c)
{
  ink_web_decompose_url(url,
                        c->sche, c->host, c->port,
                        c->path, c->frag, c->quer, c->para,
                        &(c->sche_exists), &(c->host_exists),
                        &(c->port_exists), &(c->path_exists),
                        &(c->frag_exists), &(c->quer_exists),
                        &(c->para_exists), &(c->rel_url),
                        &(c->leading_slash));

  c->is_path_name = 1;
  if (c->sche_exists &&
      ((strcasecmp(c->sche,"mailto") == 0) ||
       (strcasecmp(c->sche,"telnet") == 0) ||
       (strcasecmp(c->sche,"news") == 0)))
  {
    c->is_path_name = 0;
  }
} /* End ink_web_decompose_url_into_structure */

/*---------------------------------------------------------------------------*
  
  int ink_web_remove_dots(char *src, char *dest, int *leadingslash,
                          int max_dest_len)
  
  This routine takes a path and interprets "." and ".." segments, returning
  an appropriately parsed path. It is a warning to pass a path that resolves
  to a leading "..". Inputs are the src path and the length of the dest
  buffer. Return values are a string written into the dest buffer and 
  the leadingslash flag, which indicates if the src (and the dest) have a
  leading slash, and are therefore not relative paths.
  
  Basically, these sequences: "<a><path-segment>..<b>" and "<a>.<b>" both
  turn into "<a><b>" where <a> is beginning-or-string or a complete segment,
  and <b> is end-of-string or a complete segment.
  
  e.g.
  path1/../path2  -> path2
  /path1/../path2 -> /path2
  /path1/path2/.. -> /path1
  path1/./path2   -> path1/path2
  path1/path2/.   -> path1/path2
  ./path1/path2   -> path1/path2
  ./path1         -> path1
  /./path1        -> /path1
  
  It is also a warning to pass a path whose returned value needs to be
  truncated to fit into max_dest_len characters.
  
  mep - 4/15/96
  
  *---------------------------------------------------------------------------*/

/* types of path segment */
#define NORMAL 0
#define DOT    1
#define DOTDOT 2
#define ZAP    3
#define ERROR  4

/* We statically allocate this many - if we need more, we dynamically */
/* allocate them. */
#define STATIC_PATH_LEVELS 256

int ink_web_remove_dots(char *src, char *dest, int *leadingslash,
                        int max_dest_len)
{
  char *ptr, *end;
  int free_flag = 0;
  int scount, segstart, zapflag, doff, num;
  int temp, i;
  int error = 0;
  
  /* offsets to each path segment */
  char **seg, *segstatic[STATIC_PATH_LEVELS];
  
  /* type of each segment is a ".." */
  int *type, typestatic[STATIC_PATH_LEVELS];
  
  *leadingslash = 0;
  
  /* first quickly count the "/"s to get lower bound on # of path levels */
  ptr = src;
  end = src + strlen(src);
  scount = 0;
  while (ptr < end) {
    if (*ptr++ == '/') scount++;
  }
  scount++; /* adding one to this makes it a lower bound for any case */
  
  if (scount <= STATIC_PATH_LEVELS) {
    /* we can use the statically allocated ones */
    seg = segstatic;
    type = typestatic;
  } else {
    /* too many levels of path - must dynamically allocate */
    seg = (char**)malloc(scount*sizeof(char *));
    type = (int*)malloc(scount*sizeof(int));
    free_flag = 1;
  }
  
  /* Determine starts of each path segment.
   * A segment is defined as:
   * "foo/" in the string "<a>foo/<b>", where:
   *    <a> is <start-of-string>, or a single "/"
   *    <b> is <end-of-string>, or another segment.
   * "foo" can be "." or ".."
   * Makes my head hurt just to think about it.
   *
   */
  ptr = src; scount = 0;
  /* a segstart starts with start-of-string or a '/' */
  segstart = 1; 
  while (ptr < end) {
    if (*ptr == '/') {
      /* include leading '/' in first segment */
      if (ptr == src)
        *leadingslash = 1;
      segstart = 1;
    } else if (segstart == 1) {
      seg[scount++] = ptr;
      segstart = 0;
    } else {
      /* this is neither a "/" nor the first char of another segment */
    }
    ptr++;
  }
  /* Now scount is an accurate count of the segments we have found, */
  /* not just that lower bound we quickly got before */
  
  /* now figure out if segments are "..", ".", or normal */
  /* ZAP the "."s in place */
  for(i=0;i<scount;i++) {
    ptr = seg[i];
    if (*ptr == '.') {
      if ((ptr == end-1) || (*(ptr+1) == '/')) {
        /* it's a "." */
        type[i] = DOT;
      } else if (((ptr == end-2) && (*(ptr+1) == '.')) ||
                 ((ptr < end-2) && (*(ptr+1) == '.') && (*(ptr+2) == '/'))) {
        /* it's a ".." */
        type[i] = DOTDOT;
      } else {
        type[i] = NORMAL;
      }
    } else {
      /* it's not a special segment */
      type[i] = NORMAL;
    }
  }
  /* now ZAP each DOT, and each NORMAL following a DOTDOT */
  for(i=0;i<scount;i++) {
    if (type[i] == DOT) {
      type[i] = ZAP;
    } else if (type[i] == DOTDOT) {
      /* got a DOTDOT, count back to find first NORMAL segment */
      temp = i-1;
      zapflag = 0;
      while ((temp >= 0) && (zapflag == 0)) {
        if (type[temp] == NORMAL) {
          /* found a NORMAL one, ZAP this pair */
          type[temp] = ZAP;
          type[i] = ZAP;
          zapflag = 1;
        } else {
          temp--;
        }
      }
      if (zapflag == 0) {
        type[i] = ERROR;
        error = 1;
      }
    }
  }
  
  /* now write out the fixed path */
  doff = 0;
  *dest = 0;
  if (*leadingslash) {
    strncpy(dest+doff,"/",2);
    doff++;
  }
  for(i=0;i<scount;i++) {
    if ((type[i] == NORMAL) || (type[i] == ERROR)) {
      if (i==scount-1) {
        num = (int)(end-seg[i]);
      } else {
        num = (int)(seg[i+1]-seg[i]);
      }
      
      /* truncate if nec. */
      if (doff+num > max_dest_len) {
        num = max_dest_len-doff;
      }
      
      strncpy(dest+doff,seg[i],num+1);
      doff += num;
    } else if (type[i] == DOT) {
      /* if you get here, it indicates an algorithmic error in this routine */
      panic("ink_web_remove_dots - single dot remaining in string"); 
    } else if (type[i] == DOTDOT) {
      /* if you get here, it indicates an algorithmic error in this routine */
      panic("ink_web_remove_dots - double dot remaining in string"); 
    }
  }
  
  if (free_flag) {
    free(seg);
    free(type);
  }
  
  return(error);
}

/*---------------------------------------------------------------------------*
  
  int ink_web_unescapify_string(...)
  
  Takes a string that has has special characters turned to %AB format
  and converts them back to single special characters. See
  ink_web_escapify_string() above.
  
  mep - 4/15/96
  
  *---------------------------------------------------------------------------*/

int ink_web_unescapify_string(char *dest_in, char *src_in, int max_dest_len)
{
  char *src = src_in;
  char *dest = dest_in;
  char *hexdigits="0123456789ABCDEFabcdef";
  char *dontunescapify="#;/?+=&:@%";
  char *c1, *c2;
  int quit = 0;
  int dcount = 0;
  int num = 0;
  int dig1 = 0;
  int dig2 = 0;
  
  while ((*src != 0) && !quit) {
    if (*src == '%') {
      /* found start of an escape sequence, unescape it */
      if ((*(src+1) != 0) && (*(src+2) != 0)) {
        c1 = strchr(hexdigits,*(src+1));
        c2 = strchr(hexdigits,*(src+2));
        if ((c1 == NULL) || (c2 == NULL)) {
          ink_warning("got escape sequence but no hex digits in:%s", src_in);
          if (dcount+1 < max_dest_len) {
            *(dest++) = *src;
            dcount++;
          } else {
            ink_warning("ink_web_unescapify_string had to truncate:%s",src_in);
            quit = 1;
          }
        } else {
          /* check if hex digits lowercase */
          dig1 = (int)(c1-hexdigits);
          dig2 = (int)(c2-hexdigits);
          if (dig1 > 15) dig1 -= 6;
          if (dig2 > 15) dig2 -= 6;
          /* this is the ascii char */
          num = 16*dig1+dig2;

          if (!strchr(dontunescapify,num)) {
            /* unescapify the escape sequence you found */
            if (dcount+1 < max_dest_len) {
              *(dest++) = num;
              dcount++;
              src += 2;
            } else {
              ink_warning("ink_web_escapify_string had to truncate:%s",src_in);
              quit = 1;
            }
          } else {
            /* don't unescapify these, just pass the escape sequence */
            if (dcount+3 < max_dest_len) {
              *(dest++) = '%';
              *(dest++) = hexdigits[dig1];
              *(dest++) = hexdigits[dig2];
              dcount += 3;
              src += 2;
            } else {
              ink_warning("ink_web_unescapify_string had to truncate:%s",
                          src_in);
              quit = 1;
            }
          }
        }
      } else {
        ink_warning("got escape sequence but no hex digits (too near end of string) in:%s", src_in);
        if (dcount+1 < max_dest_len) {
          *dest++ = *src;
          dcount++;
        } else {
          ink_warning("ink_web_unescapify_string had to truncate:%s",src_in);
          quit = 1;
        }
      }
    } else {
      if (dcount+1 < max_dest_len) {
        *dest++ = *src;
        dcount++;
      } else {
        ink_warning("ink_web_unescapify_string had to truncate:%s",src_in);
        quit = 1;
      }
    }
    src++;
  }
  /* terminate string */
  if (dcount < max_dest_len)
    *dest = 0;
  else
    *(dest_in+max_dest_len) = 0;
  
  return(quit);
}

/*---------------------------------------------------------------------------*
  
  int ink_web_escapify_string(...)
  
  This functions takes an input src_in and converts all special
  characters to %<hexdigit><hexdigit> form. 
  
  Special characters are everything that is not:
  #$-_.+!*'(),;/?:@=&   or
  <alpha-char>  or
  <digit-char>
  
  e.g. "abcd fghi[klmn^" -> "abcd%20fghi%5Bklmn%5E"
  
  You must supply the buffer dest_in, with a size of max_dest_len. If
  the unescapified string grows larger than this, it will be truncated
  and you will get a warning.
  
  mep - 4/15/96
  
  *---------------------------------------------------------------------------*/

int ink_web_escapify_string(char *dest_in, char *src_in, int max_dest_len) {
  int d1,d2;
  char *hexdigits="0123456789ABCDEF";
  char *dontescapify="#;/?+=&:@~.-_%";
  char *src = src_in;
  char *dest = dest_in;
  int dcount = 0;
  int quit = 0;

  while ((*src != 0) && (dcount < max_dest_len) && (quit == 0)) {
    if ((char*)memchr(dontescapify,*src,INT_MAX) ||
        ParseRules::is_alpha(*src) ||
        ParseRules::is_digit(*src)) {
      /* this is regular character, don't escapify it */
      if (dcount+1 < max_dest_len) {
        *dest++ = *src;
        dcount++;
      } else {
        ink_warning("ink_web_escapify_string (1) had to truncate:'%s'",src_in);
        quit = 1;
      }
    } else {
      d1 = *src/16;
      d2 = *src%16;
      if (dcount+3 < max_dest_len) {
        *dest++ = '%'; 
        *dest++ = hexdigits[d1];
        *dest++ = hexdigits[d2];
        /*      fprintf(stderr,"%d %d  %c %c\n",d1,d2,hexdigits[d1],hexdigits[d2]);*/
        dcount += 3;
      } else {
        ink_warning("ink_web_escapify_string (2) had to truncate:'%s'",src_in);
        quit = 1;
      }
    }
    src++;
  }
  /* terminate string */
  if (dcount < max_dest_len)
    *dest = 0;
  else
    *(dest_in+max_dest_len-1) = 0;
  
  return(quit);
}

//////////////////////////////////////////////////////////////////////////
// imported from libts
//////////////////////////////////////////////////////////////////////////

void ink_die_die_die(int retval)
{
    abort();
    _exit(retval);
    exit(retval);
}
    
/*---------------------------------------------------------------------------*

  void ink_fatal(int return_code, char *message_format, ...)

  This routine prints/logs an error message given the printf
  format string in <message_format>, and the optional arguments.
  The program is then terminated with return code <return_code>.

 *---------------------------------------------------------------------------*/

void ink_fatal_va(int return_code, const char *message_format, va_list ap)
{
    char extended_format[4096],message[4096];

    sprintf(extended_format,"FATAL: %s",message_format);
    vsprintf(message,extended_format,ap);
    fprintf(stderr,"%s\n",message);
    ink_die_die_die(return_code);
} /* End ink_fatal_va */


void ink_fatal(int return_code, const char *message_format, ...)
{
    va_list ap;
    va_start(ap,message_format);
    ink_fatal_va(return_code,message_format,ap);
    va_end(ap);
} /* End ink_fatal */

void ink_warning(const char *message_format, ...)
{
    va_list ap;
    char extended_format[4096],message[4096];

    va_start(ap,message_format);
    sprintf(extended_format,"WARNING: %s",message_format);
    vsprintf(message,extended_format,ap);
    fprintf(stderr,"%s\n",message);
    va_end(ap);
} /* End ink_warning */

int _ink_assert(const char * a, const char * f, int l) {
  char buf1[81];
  char buf2[256];

#ifndef NO_ASSERTS
  strncpy( buf1, f,80);
  sprintf(buf2,"%s:%d: failed assert `",buf1,l);
  strncat(buf2,a,100);
  strcat(buf2,"`");
  ink_fatal(1,buf2);
#endif /* NO_ASSERTS */

  return (0);
}


static void process_arg(ArgumentDescription * argument_descriptions,
                        int n_argument_descriptions,
                        int i, 
                        char ***argv)
{
  char * arg = NULL;
  if (argument_descriptions[i].type) {
    char type = argument_descriptions[i].type[0];
    if (type=='F'||type=='f') 
      *(int *)argument_descriptions[i].location = type=='F'?1:0;
    else if (type=='T')
      *(int *)argument_descriptions[i].location = 
        !*(int *)argument_descriptions[i].location;
    else {
      arg = *++(**argv) ? **argv : *++(*argv);
      if (!arg) usage(argument_descriptions, n_argument_descriptions, NULL);
      switch (type) {
        case 'I': 
          *(int *)argument_descriptions[i].location = atoi(arg);
          break;
        case 'D': 
          *(double *)argument_descriptions[i].location = atof(arg);
          break;
        case 'L':
          *(int64_t *)argument_descriptions[i].location = ink_atoll(arg);
          break;
        case 'S': strncpy((char *)argument_descriptions[i].location,arg,
                          atoi(argument_descriptions[i].type+1));
          break;
        default:
          ink_fatal(1,(char *)"bad argument description");
          break;
      }
      **argv += strlen(**argv)-1;
    }
  }
  if (argument_descriptions[i].pfn) 
    argument_descriptions[i].pfn(argument_descriptions, 
                                 n_argument_descriptions,
                                 arg);
}


void show_argument_configuration(ArgumentDescription * argument_descriptions,
                                 int n_argument_descriptions) 
{
  int i = 0;
  printf("Argument Configuration\n"); 
  for (i=0;i<n_argument_descriptions;i++) {
    if (argument_descriptions[i].type) {
      printf("  %s%s",argument_descriptions[i].description,
             &SPACES[strlen(argument_descriptions[i].description)+45]);
      switch (argument_descriptions[i].type[0]) {
      case 'F':
      case 'f':
      case 'T':
        printf(*(int*)argument_descriptions[i].location?"TRUE":"FALSE");
        break;
      case 'I':
        printf("%d",*(int*)argument_descriptions[i].location);
        break;
      case 'D':
        printf("%f",*(double*)argument_descriptions[i].location);
        break;
      case 'L':
#if defined(FreeBSD)
        printf("%" PRId64"",*(int64_t*)argument_descriptions[i].location);
#else
        printf("%" PRId64"",*(int64_t*)argument_descriptions[i].location);
#endif
        break;
      case 'S':
        printf("%s",(char*)argument_descriptions[i].location);
          break;
      default: ink_fatal(1,(char *)"bad argument description"); break;
      }
      printf("\n");
    }
  }
}

void process_args(ArgumentDescription * argument_descriptions,
                  int n_argument_descriptions,
                  char **argv) 
{
  int i = 0;
  //
  // Grab Environment Variables
  //
  for (i=0;i<n_argument_descriptions;i++)
    if (argument_descriptions[i].env) {
      char type = argument_descriptions[i].type[0];
      char * env = getenv(argument_descriptions[i].env);
      if (!env) continue;
      switch (type) {
      case 'f':
      case 'F':
      case 'I':
        *(int *)argument_descriptions[i].location = atoi(env);
        break;
      case 'D':
        *(double *)argument_descriptions[i].location = atof(env);
        break;
      case 'L':
        *(int64_t *)argument_descriptions[i].location = ink_atoll(env);
        break;
      case 'S':
        strncpy((char *)argument_descriptions[i].location,env,
                atoi(argument_descriptions[i].type+1));
        break;
      }
    }

  //
  // Grab Command Line Arguments
  //
  program_name = argv[0];
  while ( *++argv ) {
    if ( **argv == '-' ) {
      if ((*argv)[1] == '-') {
        for (i=0;i<n_argument_descriptions;i++)
          if (!strcmp(argument_descriptions[i].name,(*argv)+2)) {
            *argv += strlen(*argv)-1;
            process_arg(argument_descriptions,n_argument_descriptions,i,&argv);
            break;
          }
          if (i>=n_argument_descriptions) 
            usage(argument_descriptions,n_argument_descriptions,NULL);
      } else {
        while ( *++(*argv) ) 
          for (i=0;i<n_argument_descriptions;i++) 
            if (argument_descriptions[i].key==**argv) { 
              process_arg(argument_descriptions,n_argument_descriptions,
                          i,&argv);
              break;
            }
            if (i>=n_argument_descriptions) 
              usage(argument_descriptions,n_argument_descriptions,NULL);
      }
    } else {
      if (n_file_arguments>MAX_FILE_ARGUMENTS) ink_fatal(1,(char *)"too many files");
      file_arguments[n_file_arguments++] = *argv;
      file_arguments[n_file_arguments] = NULL;
    }
  }
}

void usage(ArgumentDescription * argument_descriptions,
           int n_argument_descriptions,
           char * dummy) 
{
  (void)argument_descriptions; (void)n_argument_descriptions; (void)dummy;
  fprintf(stderr,"Usage: %s [--SWITCH [ARG]]\n",program_name);
  fprintf(stderr,"  switch__________________type__default___description\n");
  for (int i=0;i<n_argument_descriptions;i++) {
    fprintf(stderr,"  -%c, --%s%s%s",
            argument_descriptions[i].key,
            argument_descriptions[i].name,
            (strlen(argument_descriptions[i].name) + 61 < 81) ?
             &SPACES[strlen(argument_descriptions[i].name)+61] : "",
            argument_types_descriptions[
              argument_descriptions[i].type ?
                strchr(argument_types_keys,
                       argument_descriptions[i].type[0]) -
                argument_types_keys :
                strlen(argument_types_keys)
            ]);
    switch(argument_descriptions[i].type?argument_descriptions[i].type[0]:0) {
      case 0: fprintf(stderr, "          "); break;
      case 'L':
        fprintf(stderr, 
#if defined(FreeBSD)
                " %-9" PRId64"",
#else
                " %-9" PRId64"",
#endif
                *(int64_t*)argument_descriptions[i].location);
        break;
      case 'S':
        if (*(char*)argument_descriptions[i].location) {
          if (strlen((char*)argument_descriptions[i].location) < 10)
            fprintf(stderr, " %-9s", 
                    (char*)argument_descriptions[i].location);
          else {
            ((char*)argument_descriptions[i].location)[7] = 0;
            fprintf(stderr, " %-7s..", 
                    (char*)argument_descriptions[i].location);
          }
        } else
          fprintf(stderr, " (null)   ");
        break;
      case 'D':
        fprintf(stderr, " %-9.3f",
                *(double*)argument_descriptions[i].location);
        break;
      case 'I':
        fprintf(stderr, " %-9d", *(int *)argument_descriptions[i].location);
        break;
      case 'T': case 'f': case 'F': 
        fprintf(stderr, " %-9s",
                *(int *)argument_descriptions[i].location?"true ":"false");
        break;
    }
    fprintf(stderr," %s\n",argument_descriptions[i].description);
  }
  _exit(1);
}

const unsigned int parseRulesCType[256] = {
        0xD1210821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xBB130861,
        0x5B390821,
        0xD8010821,
        0xD8010821,
        0x5B350821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xD0010821,
        0xAB1F0841,
        0xC140C301,
        0x20028801,
        0xC2408801,
        0xC0404501,
        0xC0408801,
        0xC040A101,
        0xC040C301,
        0xA002C301,
        0xA002C301,
        0xC0C0C301,
        0xC040C501,
        0x2102C301,
        0xC040C501,
        0xC040C501,
        0xA002A001,
        0xC4404191,
        0xC4404191,
        0xC4404191,
        0xC4404191,
        0xC4404191,
        0xC4404191,
        0xC4404191,
        0xC4404191,
        0xC4404191,
        0xC4404191,
        0x8102A101,
        0xA002A001,
        0xA0028801,
        0xC002A101,
        0xA0028801,
        0xA082A001,
        0xE002A101,
        0xC440418B,
        0xC440418B,
        0xC440418B,
        0xC440418B,
        0xC440418B,
        0xC440418B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xC440410B,
        0xA082D101,
        0xA082D101,
        0xA002D101,
        0xC040D101,
        0xC040C501,
        0xC040D101,
        0xC440418D,
        0xC440418D,
        0xC440418D,
        0xC440418D,
        0xC440418D,
        0xC440418D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xC440410D,
        0xA002D101,
        0xC040D101,
        0xA002D101,
        0xC040D101,
        0xD0010821,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000,
        0xC0000000
};
const char      parseRulesCTypeToUpper[256] = {
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
        31,
        32,
        33,
        34,
        35,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        48,
        49,
        50,
        51,
        52,
        53,
        54,
        55,
        56,
        57,
        58,
        59,
        60,
        61,
        62,
        63,
        64,
        65,
        66,
        67,
        68,
        69,
        70,
        71,
        72,
        73,
        74,
        75,
        76,
        77,
        78,
        79,
        80,
        81,
        82,
        83,
        84,
        85,
        86,
        87,
        88,
        89,
        90,
        91,
        92,
        93,
        94,
        95,
        96,
        65,
        66,
        67,
        68,
        69,
        70,
        71,
        72,
        73,
        74,
        75,
        76,
        77,
        78,
        79,
        80,
        81,
        82,
        83,
        84,
        85,
        86,
        87,
        88,
        89,
        90,
        123,
        124,
        125,
        126,
        127,
        -128,
        -127,
        -126,
        -125,
        -124,
        -123,
        -122,
        -121,
        -120,
        -119,
        -118,
        -117,
        -116,
        -115,
        -114,
        -113,
        -112,
        -111,
        -110,
        -109,
        -108,
        -107,
        -106,
        -105,
        -104,
        -103,
        -102,
        -101,
        -100,
        -99,
        -98,
        -97,
        -96,
        -95,
        -94,
        -93,
        -92,
        -91,
        -90,
        -89,
        -88,
        -87,
        -86,
        -85,
        -84,
        -83,
        -82,
        -81,
        -80,
        -79,
        -78,
        -77,
        -76,
        -75,
        -74,
        -73,
        -72,
        -71,
        -70,
        -69,
        -68,
        -67,
        -66,
        -65,
        -64,
        -63,
        -62,
        -61,
        -60,
        -59,
        -58,
        -57,
        -56,
        -55,
        -54,
        -53,
        -52,
        -51,
        -50,
        -49,
        -48,
        -47,
        -46,
        -45,
        -44,
        -43,
        -42,
        -41,
        -40,
        -39,
        -38,
        -37,
        -36,
        -35,
        -34,
        -33,
        -32,
        -31,
        -30,
        -29,
        -28,
        -27,
        -26,
        -25,
        -24,
        -23,
        -22,
        -21,
        -20,
        -19,
        -18,
        -17,
        -16,
        -15,
        -14,
        -13,
        -12,
        -11,
        -10,
        -9,
        -8,
        -7,
        -6,
        -5,
        -4,
        -3,
        -2,
        -1
};
const char      parseRulesCTypeToLower[256] = {
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
        31,
        32,
        33,
        34,
        35,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        48,
        49,
        50,
        51,
        52,
        53,
        54,
        55,
        56,
        57,
        58,
        59,
        60,
        61,
        62,
        63,
        64,
        97,
        98,
        99,
        100,
        101,
        102,
        103,
        104,
        105,
        106,
        107,
        108,
        109,
        110,
        111,
        112,
        113,
        114,
        115,
        116,
        117,
        118,
        119,
        120,
        121,
        122,
        91,
        92,
        93,
        94,
        95,
        96,
        97,
        98,
        99,
        100,
        101,
        102,
        103,
        104,
        105,
        106,
        107,
        108,
        109,
        110,
        111,
        112,
        113,
        114,
        115,
        116,
        117,
        118,
        119,
        120,
        121,
        122,
        123,
        124,
        125,
        126,
        127,
        -128,
        -127,
        -126,
        -125,
        -124,
        -123,
        -122,
        -121,
        -120,
        -119,
        -118,
        -117,
        -116,
        -115,
        -114,
        -113,
        -112,
        -111,
        -110,
        -109,
        -108,
        -107,
        -106,
        -105,
        -104,
        -103,
        -102,
        -101,
        -100,
        -99,
        -98,
        -97,
        -96,
        -95,
        -94,
        -93,
        -92,
        -91,
        -90,
        -89,
        -88,
        -87,
        -86,
        -85,
        -84,
        -83,
        -82,
        -81,
        -80,
        -79,
        -78,
        -77,
        -76,
        -75,
        -74,
        -73,
        -72,
        -71,
        -70,
        -69,
        -68,
        -67,
        -66,
        -65,
        -64,
        -63,
        -62,
        -61,
        -60,
        -59,
        -58,
        -57,
        -56,
        -55,
        -54,
        -53,
        -52,
        -51,
        -50,
        -49,
        -48,
        -47,
        -46,
        -45,
        -44,
        -43,
        -42,
        -41,
        -40,
        -39,
        -38,
        -37,
        -36,
        -35,
        -34,
        -33,
        -32,
        -31,
        -30,
        -29,
        -28,
        -27,
        -26,
        -25,
        -24,
        -23,
        -22,
        -21,
        -20,
        -19,
        -18,
        -17,
        -16,
        -15,
        -14,
        -13,
        -12,
        -11,
        -10,
        -9,
        -8,
        -7,
        -6,
        -5,
        -4,
        -3,
        -2,
        -1
};

#define UINT4 unsigned int

typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

static void MD5Init (MD5_CTX *);
static void MD5Update (MD5_CTX *, unsigned char *, unsigned int);
static void MD5Final (unsigned char [16], MD5_CTX *);

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void MD5Transform (UINT4 [4], unsigned char [64]);
static void Encode (unsigned char *, UINT4 *, unsigned int);

#define MD5_memcpy memcpy
#define MD5_memset memset

static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }

/* MD5 initialization. Begins an MD5 operation, writing a new context.
 */

static void MD5Init (MD5_CTX *context) {
  context->count[0] = context->count[1] = 0;

  /* Load magic initialization constants.
   */
  context->state[0] = 0x67452301;
  context->state[1] = 0xefcdab89;
  context->state[2] = 0x98badcfe;
  context->state[3] = 0x10325476;
}

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* MD5 block update operation. Continues an MD5 message-digest
     operation, processing another message block, and updating the
     context.
 */
static void MD5Update (MD5_CTX *context, unsigned char * input, unsigned int inputLen) {
  unsigned int i, index, partLen;

  /* Compute number of bytes mod 64 */
  index = (unsigned int)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((UINT4)inputLen << 3))
      < ((UINT4)inputLen << 3))
    context->count[1]++;
  context->count[1] += ((UINT4)inputLen >> 29);

  partLen = 64 - index;

  /* Transform as many times as possible.
   */
  if (inputLen >= partLen) {
    MD5_memcpy
      ((POINTER)&context->buffer[index], (POINTER)input, partLen);
    MD5Transform (context->state, context->buffer);

    for (i = partLen; i + 63 < inputLen; i += 64)
      MD5Transform (context->state, &input[i]);

    index = 0;
  }
  else
    i = 0;

  /* Buffer remaining input */
  MD5_memcpy
    ((POINTER)&context->buffer[index], (POINTER)&input[i],
     inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
     the message digest and zeroizing the context.
 */
static void MD5Final (unsigned char digest[16], MD5_CTX *context) {
  unsigned char bits[8];
  unsigned int index, padLen;

  /* Save number of bits */
  Encode (bits, context->count, 8);

  /* Pad out to 56 mod 64.
   */
  index = (unsigned int)((context->count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  MD5Update (context, PADDING, padLen);

  /* Append length (before padding) */
  MD5Update (context, bits, 8);

  /* Store state in digest */
  Encode (digest, context->state, 16);

  /* Zeroize sensitive information.
   */
  MD5_memset ((POINTER)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block.
 */
static void MD5Transform (UINT4 state[4], unsigned char block[64]) {
  UINT4 a, b, c, d, x[16];
  unsigned int i, j;

  /* Decode (x, block, 64); */
  for (i = 0, j = 0; j < 64; i++, j += 4)
    x[i] = (((UINT4) block[j]) |
            (((UINT4) block[j+1]) << 8) |
            (((UINT4) block[j+2]) << 16) |
            (((UINT4) block[j+3]) << 24));
  
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  
  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

  /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  /* Zeroize sensitive information.
   */
  /* No need to zeroize "sensitive" information. This is just zeroing
     out data on the stack. Totally unnecessary in traffic server. */
  /* MD5_memset ((POINTER)x, 0, sizeof (x)); */
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
     a multiple of 4.
 */
static void Encode (unsigned char *output, UINT4*input, unsigned int len) {
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
    output[j] = (unsigned char)(input[i] & 0xff);
    output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
    output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
    output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
  }
}

int ink_code_md5(unsigned char *input,
                 int input_length,
                 unsigned char *sixteen_byte_hash_pointer) {
  MD5_CTX context;

  MD5Init(&context);
  MD5Update(&context, input, input_length);
  MD5Final(sixteen_byte_hash_pointer, &context);

  return(0);
} /* End ink_code_md5 */
