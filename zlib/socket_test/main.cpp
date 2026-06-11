#ifdef _WIN32
   #include <winsock2.h>
   #include <ws2tcpip.h>
   typedef  int socklen_t;
   #define  BEG_EINTR /*NO_OP*/
   #define  END_EINTR /*NO_OP*/
#else
   #include <netdb.h>
   #include <errno.h>
   #include <unistd.h>
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/tcp.h>
   typedef  int SOCKET;
   #define  INVALID_SOCKET -1
   #define  BEG_EINTR do {
   #define  END_EINTR } while(ec==EINTR && !ok);
   #define  closesocket(X) close(X)
#endif
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <CMD/CMDlineParser.h>
#include <COL/COLostream.h>
#include <COL/COLarray.h>
#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class Skt
{
public:
   typedef struct sockaddr_storage SADDR_STO;
   typedef struct sockaddr         SADDR_CMN;
   typedef struct sockaddr_in      SADDR_IP4;
   typedef struct sockaddr_in6     SADDR_IP6;
   Skt();
   SADDR_CMN*  addr() { return (SADDR_CMN*) &sa; } // Socket address
   SOCKET      desc() const { return sd;         } // Socket descriptor
   int         code() const { return ec;         } // Last error code
   bool        isOK() const { return ok;         } // Last operation status
   const char* func() const { return fn;         } // Last function name
   const char* emsg() const { return em.c_str(); } // Last error message
   bool        isV4() const { return sa.ss_family==AF_INET ; }
   bool        isV6() const { return sa.ss_family==AF_INET6; }
   int         size() const { return isV4() ? sizeof(SADDR_IP4)
                                            : sizeof(SADDR_IP6); }
   bool        valid() const { return sd!=INVALID_SOCKET; }
   void        setaddr(SADDR_STO& a) { sa = a; }
   bool create(bool v4, bool udp);
   bool bind();
   bool connect();
   bool close();
   bool listen(int backlog);
   bool accept(Skt& client);
   bool hostname(COLstring& host, int& port, bool numeric=true);
   bool selfname(COLstring& host, int& port, bool numeric=true);
   bool peername(COLstring& host, int& port, bool numeric=true);
   bool seterr(const char* fnc, bool isok);
   template<class T> bool setopt(int name, T  val, int len=sizeof(T));
   template<class T> bool getopt(int name, T& val, int len=sizeof(T));
   static bool init();
   COLarray<SADDR_STO> list(bool v4, bool udp,
                           const char* host=NULL,
                           const char* port=NULL);
private:
   SOCKET      sd; // Socket descriptor
   int         ec; // Last error code
   bool        ok; // Last operation status
   const char* fn; // Last function name
   COLstring   em; // Last error message
   SADDR_STO   sa; // Socket address
};

Skt::Skt() : sd(INVALID_SOCKET), ec(0), ok(true), fn(""), em()
             { ::memset(&sa,0,sizeof(sa)); }

bool Skt::create(bool v4, bool udp) {
   BEG_EINTR
   sd = ::socket(v4?AF_INET:AF_INET6, udp?SOCK_DGRAM:SOCK_STREAM, 0);
   seterr("::socket()", sd!=INVALID_SOCKET);
   END_EINTR
   return ok;
}

bool Skt::bind() {
   BEG_EINTR
   int rc = ::bind(sd, addr(), size());
   seterr("::bind()", rc==0);
   END_EINTR
   return ok;
}

bool Skt::connect() {
   BEG_EINTR
   int rc = ::connect(sd, addr(), size());
   seterr("::connect()",  rc==0);
   END_EINTR
   return ok;
}

bool Skt::close() {
   BEG_EINTR
   int rc = ::closesocket(sd);
   seterr("::close()", rc==0);
   END_EINTR
   return ok;
}

bool Skt::listen(int backlog) {
   BEG_EINTR
   int rc = ::listen(sd, backlog);
   seterr("::listen()", rc==0);
   END_EINTR
   return ok;
}

bool Skt::accept(Skt& client) {
   socklen_t siz = sizeof(client.sa);
   BEG_EINTR
   client.sd = ::accept(sd, client.addr(), &siz);
   seterr("::accept()", client.sd!=INVALID_SOCKET);
   END_EINTR
   client.ec = ec;
   client.ok = ok;
   client.fn = fn;
   client.em = em;
   return ok;
}

bool Skt::hostname(COLstring& host, int& port, bool numeric) {
   char hbuf[NI_MAXHOST+1]= { 0 };
   char pbuf[NI_MAXSERV+1]= { 0 };
   BEG_EINTR
   int rc = ::getnameinfo(addr(), size(),
                          (char*) &hbuf, sizeof(hbuf),
                          (char*) &pbuf, sizeof(pbuf),
                          (numeric?NI_NUMERICHOST:0) | NI_NUMERICSERV);
   seterr("::getnameinfo()", rc==0);
   END_EINTR
   host = ok?hbuf:"";
   port = ok?atoi(pbuf):-1;
   return ok;
}

bool Skt::selfname(COLstring& host, int& port, bool numeric) {
   SADDR_STO tmp = sa;
   socklen_t len = sizeof(sa);
   BEG_EINTR
   int rc1 = ::getsockname(sd, addr(), &len);
   seterr("::getsockname()", rc1==0);
   END_EINTR
   if(ok) hostname(host, port, numeric);
   else { host=""; port=-1; }
   sa = tmp;
   return ok;
}

bool Skt::peername(COLstring& host, int& port, bool numeric) {
   SADDR_STO tmp = sa;
   socklen_t len = sizeof(sa);
   BEG_EINTR
   int rc1 = ::getpeername(sd, addr(), &len);
   seterr("::getpeername()", rc1==0);
   END_EINTR
   if(ok) hostname(host, port, numeric);
   else { host=""; port=-1; }
   sa = tmp;
   return ok;
}

bool Skt::seterr(const char* fnc, bool isok) {
   ec = 0    ;
   ok = isok ;
   fn = fnc  ;
   em.clear();
   if(!ok)
   {
#ifdef _WIN32
      LPVOID buf;
      ec = ::WSAGetLastError();
      DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM     |
                    FORMAT_MESSAGE_IGNORE_INSERTS  ;
      if(::FormatMessage(flags,NULL,ec,0,(LPTSTR)&buf,0,NULL)<=0)
           em = "Unknown Error";
      else em = (LPCTSTR) buf;
      ::LocalFree(buf);
#else
      ec = errno;
      em = ::strerror(ec);
#endif
      (COLcout<<"\n"<<fn<<" failed "<<ec<<":"<<em<<"\n").flush();
   }
   return ok;
}

template<class T> bool Skt::setopt(int name, T val, int len) {
   int lev = (name==TCP_NODELAY) ? IPPROTO_TCP : SOL_SOCKET;
   BEG_EINTR
   int rc = ::setsockopt(sd, lev, name, (const char*)(&val), len);
   seterr("::getsockopt()",  rc==0);
   END_EINTR
   return ok;
}

template<class T> bool Skt::getopt(int name, T& val, int len) {
   int lev = (name==TCP_NODELAY) ? IPPROTO_TCP : SOL_SOCKET;
   socklen_t siz = len;
   BEG_EINTR
   int rc = ::getsockopt(sd, lev, name, (char*)(&val), &siz);
   if(rc==0) assert(siz==len);
   seterr("::getsockopt()", rc==0);
   END_EINTR
   return ok;
}

bool Skt::init() {
#ifdef _WIN32
   static bool ready = false;
   if(!ready) {
      WSADATA dat;
      WORD    ver = MAKEWORD(2/*Major*/, 0/*Minor*/);
      if(::WSAStartup(ver, &dat) || ver<dat.wVersion) {
         WSACleanup();
         return false;
      }
      ready = true;
   }
#endif
   return true;
}

COLarray<Skt::SADDR_STO> Skt::list(bool v4, bool udp,
                             const char* host, const char* port) {
   COLarray<SADDR_STO> data;
   struct addrinfo hint, *res=NULL, *p=NULL;
   ::memset(&hint, 0, sizeof(hint));
   hint.ai_family   = v4   ? AF_INET    : AF_INET6   ;
   hint.ai_socktype = udp  ? SOCK_DGRAM : SOCK_STREAM;
   hint.ai_flags    = host ? 0          : AI_PASSIVE ;
   BEG_EINTR
   int rc = ::getaddrinfo(host, port, &hint, &res);
   seterr("::getaddrinfo()", rc==0);
   END_EINTR
   if(!ok) res = NULL;
   for(p=res ; p ; p=p->ai_next) {
      SADDR_STO ad;
      ::memset(&ad, 0, sizeof(ad));
      switch(p->ai_family)
      {
         case AF_INET : *((SADDR_IP4*)(&ad))=*((SADDR_IP4*)(p->ai_addr)); break;
         case AF_INET6: *((SADDR_IP6*)(&ad))=*((SADDR_IP6*)(p->ai_addr)); break;
         default      : continue; break;
      }
      data.push_back(ad);
   }
   if(res) ::freeaddrinfo(res);
   return data;
}

const char* getparam(CMDlineParser& psr, const char* key, const char* def) {
   if(!psr.isFlagPresent(key)) return def;
   return psr.flagArgument(key).c_str();
}

int main(int argc, const char** argv) {
   CMDlineParser psr;
   COL_LOG_INIT(argc, argv);
   psr.setDescription("Socket Test");
   psr.addFlagWithArgument("host" ,"<server host>","Server host");
   psr.addFlagWithArgument("port" ,"<server port>","Server port");
   psr.addFlagWithArgument("limit","<#sockets>"   ,"Maximum # of sockets");
   psr.addFlagWithArgument("pause","<pause ms>"   ,"Pause x ms per socket");
   psr.addFlagWithoutArgument("connectOnly", "Connect as client but don't initiate http request");

   psr.parseArgs(argc, argv);
   if(psr.parsingErrorsPresent(COLcout)) {
      psr.showUsage(COLcout);
      return 1;
   }
   const char* host  =        getparam(psr, "host" , NULL  );
   const char* port  =        getparam(psr, "port" , "7777");
   const int   limit = ::atoi(getparam(psr, "limit", "5000"));
   const int   pause = ::atoi(getparam(psr, "pause", "0"   ));
   COLstring hst;
   char  buf[64];
   int num = 0, prt;
   if(!Skt::init()) return 2;
   COLarray<Skt::SADDR_STO> ads = Skt().list(true, false, host, port);
   if(ads.size()<=0) return 3;
   if(host==NULL)
   {
      Skt svr;
      ::memset(buf, 'A', sizeof(buf));
      svr.setaddr(ads[0]);
      if(!svr.create(true,false)     ||
         !svr.setopt(SO_REUSEADDR,1) || 
         !svr.bind()                 ||
         !svr.listen(20)) return 4;
      svr.hostname(hst, prt, true);
      (COLcout<<"Server mode "<<hst<<":"<<prt<<"\n").flush();
      for(num=0 ; num<limit ; )
      {
          Skt sock;
          if(!svr.accept(sock)) break;
          num++;
          sock.peername(hst, prt, true);
          if(num%20==0) (COLcout<<"#"<<num<<" ["<<hst<<":"<<prt<<"]  \r").flush();
          if(::send(sock.desc(),buf,sizeof(buf),0)!=sizeof(buf)) break;
      }
   }
   else
   {
      (COLcout << "Client mode\n").flush();
      const char* req = "GET /login.html HTTP/1.1\r\nConnection: Keep-Alive\r\n\r\n";
      int         len = ::strlen(req);
      for(num=0 ; num<limit ; )
      {
          Skt sock;
          sock.setaddr(ads[0]);
          if(!sock.create(true,false) || !sock.connect()) break;
          if(!psr.isFlagPresent("connectOnly")) {
             if(::send(sock.desc(),req,len,0)!=len) break;
             if(::recv(sock.desc(),buf,sizeof(buf),0)<=0) break;
          }
          num++;
          sock.selfname(hst, prt, true);
          if(num%20==0) (COLcout<<"#"<<num<<" ["<<hst<<":"<<prt<<"]  \r").flush();
          if(pause>0) COLsleep(pause);
      }
   }
   (COLcout<<"\nTotal# connections "<<num<<"\n").flush();
   return 0;
}
