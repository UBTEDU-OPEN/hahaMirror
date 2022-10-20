#include "time.h"

#include <arpa/inet.h>
#include <assert.h>
#include <endian.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

namespace common
{
namespace time
{
uint32_t getHDTimer()
{
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000L + t.tv_nsec / 1000000L;
}
uint64_t getCurrentNanoTime()
{
#if HAS_CLOCK_GETTIME
    struct timespec tim;
    clock_gettime(CLOCK_REALTIME, &tim);
    return static_cast<uint64_t>(tim.tv_sec) * 1000000000LL + tim.tv_nsec;
#else
    struct timeval timeofday;
    gettimeofday(&timeofday, NULL);
    return static_cast<uint64_t>(timeofday.tv_sec) * 1000000000LL +
           static_cast<uint64_t>(timeofday.tv_usec) * 1000LL;
#endif
}

uint64_t getCurrentTime()
{
#if HAS_CLOCK_GETTIME
    struct timespec tim;
    clock_gettime(CLOCK_REALTIME, &tim);
    return static_cast<uint64_t>(tim.tv_sec) * 1000000000LL + tim.tv_nsec;
#else
    struct timeval timeofday;
    gettimeofday(&timeofday, NULL);
    return static_cast<uint64_t>(timeofday.tv_sec);
#endif
}

uint64_t getCurrentMilliTime()
{
#if HAS_CLOCK_GETTIME
    struct timespec tim;
    clock_gettime(CLOCK_REALTIME, &tim);
    return static_cast<uint64_t>(tim.tv_sec) * 1000000000LL + tim.tv_nsec;
#else
    struct timeval timeofday;
    gettimeofday(&timeofday, NULL);
    return static_cast<uint64_t>(timeofday.tv_sec) * 1000LL +
           static_cast<uint64_t>(timeofday.tv_usec) / 1000LL;
#endif
}

std::string timeFormatYMD()
{
    time_t rawtime;
    struct tm* info;
    char buffer[80];
    ::time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, 80, "%Y%m%d", info);

    return std::string(buffer);
}

std::string timeFormatString()
{
    time_t rawtime;
    struct tm* info;
    char buffer[80];
    ::time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", info);

    return std::string(buffer);
}

#define VERSION_3 3
#define VERSION_4 4

#define MODE_CLIENT 3
#define MODE_SERVER 4

#define NTP_LI 0
#define NTP_VN VERSION_3
#define NTP_MODE MODE_CLIENT
#define NTP_STRATUM 0
#define NTP_POLL 4
#define NTP_PRECISION -6

#define NTP_HLEN 48

#define NTP_PORT 123
#define NTP_SERVER "192.168.1.150" //"182.92.12.11"

#define TIMEOUT 5

#define BUFSIZE 1500

#define JAN_1970 0x83aa7e80

#define NTP_CONV_FRAC32(x) (uint64_t)((x) * ((uint64_t) 1 << 32))
#define NTP_REVE_FRAC32(x) ((double) ((double) (x) / ((uint64_t) 1 << 32)))

#define NTP_CONV_FRAC16(x) (uint32_t)((x) * ((uint32_t) 1 << 16))
#define NTP_REVE_FRAC16(x) ((double) ((double) (x) / ((uint32_t) 1 << 16)))

#define USEC2FRAC(x) ((uint32_t) NTP_CONV_FRAC32((x) / 1000000.0))
#define FRAC2USEC(x) ((uint32_t) NTP_REVE_FRAC32((x) *1000000.0))

#define NTP_LFIXED2DOUBLE(x) \
    ((double) (ntohl(((struct l_fixedpt*) (x))->intpart) - JAN_1970 + \
               FRAC2USEC(ntohl(((struct l_fixedpt*) (x))->fracpart)) / 1000000.0))

using namespace std;
struct s_fixedpt
{
    uint16_t intpart;
    uint16_t fracpart;
};

struct l_fixedpt
{
    uint32_t intpart;
    uint32_t fracpart;
};

struct ntphdr
{
#if __BYTE_ORDER == __BID_ENDIAN
    unsigned int ntp_li : 2;
    unsigned int ntp_vn : 3;
    unsigned int ntp_mode : 3;
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ntp_mode : 3;
    unsigned int ntp_vn : 3;
    unsigned int ntp_li : 2;
#endif
    uint8_t ntp_stratum;
    uint8_t ntp_poll;
    int8_t ntp_precision;
    struct s_fixedpt ntp_rtdelay;
    struct s_fixedpt ntp_rtdispersion;
    uint32_t ntp_refid;
    struct l_fixedpt ntp_refts;
    struct l_fixedpt ntp_orits;
    struct l_fixedpt ntp_recvts;
    struct l_fixedpt ntp_transts;
};

static in_addr_t inet_host(const char* host)
{
    in_addr_t saddr;
    struct hostent* hostent;

    if ((saddr = inet_addr(host)) == INADDR_NONE)
    {
        if ((hostent = gethostbyname(host)) == NULL)
            return INADDR_NONE;

        memmove(&saddr, hostent->h_addr, hostent->h_length);
    }

    return saddr;
}

static int get_ntp_packet(void* buf, size_t* size) //构建并发送NTP请求报文
{
    struct ntphdr* ntp;
    struct timeval tv;

    if (!size || *size < NTP_HLEN)
        return -1;

    memset(buf, 0, *size);

    ntp = (struct ntphdr*) buf;
    ntp->ntp_li = NTP_LI;
    ntp->ntp_vn = NTP_VN;
    ntp->ntp_mode = NTP_MODE;
    ntp->ntp_stratum = NTP_STRATUM;
    ntp->ntp_poll = NTP_POLL;
    ntp->ntp_precision = NTP_PRECISION;

    gettimeofday(&tv, NULL); //把目前的时间用tv 结构体返回
    ntp->ntp_transts.intpart = htonl(tv.tv_sec + JAN_1970);
    ntp->ntp_transts.fracpart = htonl(USEC2FRAC(tv.tv_usec));

    *size = NTP_HLEN;

    return 0;
}

static double get_rrt(const struct ntphdr* ntp, const struct timeval* recvtv) //往返时延
{
    double t1, t2, t3, t4;

    t1 = NTP_LFIXED2DOUBLE(&ntp->ntp_orits);
    t2 = NTP_LFIXED2DOUBLE(&ntp->ntp_recvts);
    t3 = NTP_LFIXED2DOUBLE(&ntp->ntp_transts);
    t4 = recvtv->tv_sec + recvtv->tv_usec / 1000000.0;

    return (t4 - t1) - (t3 - t2);
}

static double get_offset(const struct ntphdr* ntp, const struct timeval* recvtv) //偏移量
{
    double t1, t2, t3, t4;

    t1 = NTP_LFIXED2DOUBLE(&ntp->ntp_orits);
    t2 = NTP_LFIXED2DOUBLE(&ntp->ntp_recvts);
    t3 = NTP_LFIXED2DOUBLE(&ntp->ntp_transts);

    t4 = recvtv->tv_sec + recvtv->tv_usec / 1000000.0;
    printf("t1: %f, t2: %f, t3: %f, t4: %f\n", t1, t2, t3, t4);

    return ((t2 - t1) + (t3 - t4)) / 2;
}

void updateNtpTime()
{
    char dateBuf[64] = {0};
    char cmd[128] = {0};
    tm* local;
    char buf[BUFSIZE];
    size_t nbytes;
    int sockfd, maxfd1;
    struct sockaddr_in servaddr;
    fd_set readfds;
    struct timeval timeout, recvtv, tv;
    double offset;

    // if (argc != 2) {
    // usage();
    // exit(-1);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(NTP_PORT);
    servaddr.sin_addr.s_addr = inet_host("192.169.1.150");

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket error");
        exit(-1);
    }

    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(struct sockaddr)) != 0)
    {
        perror("connect error");
        exit(-1);
    }
    nbytes = BUFSIZE;
    if (get_ntp_packet(buf, &nbytes) != 0)
    {
        fprintf(stderr, "construct ntp request error \n");
        exit(-1);
    }
    send(sockfd, buf, nbytes, 0);

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    maxfd1 = sockfd + 1;

    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    if (select(maxfd1, &readfds, NULL, NULL, &timeout) > 0)
    {
        if (FD_ISSET(sockfd, &readfds))
        {
            if ((nbytes = recv(sockfd, buf, BUFSIZE, 0)) < 0)
            {
                perror("recv error");
                exit(-1);
            }

            //计算C/S时间偏移量
            gettimeofday(&recvtv, NULL);
            struct ntphdr* t = (struct ntphdr*) buf;
            offset = get_offset((struct ntphdr*) buf, &recvtv);
            ////更新系统时间
            printf("offset: %f\n", offset);
            gettimeofday(&tv, NULL);

            tv.tv_sec += (int) offset; //+28800;
            tv.tv_usec += offset - (int) offset;
            printf("tv_sec: %lld, tv_usec: %lld\n", tv.tv_sec, tv.tv_usec);

            local = localtime((time_t*) &tv.tv_sec);
            strftime(dateBuf, 64, "%Y-%m-%d %H:%M:%S", local);
            sprintf(cmd, "system busybox date -s \"%s\"", dateBuf);

            uid_t uid = getuid();
            if (setuid(0))
            {
                printf("setuid failed\n");
                abort();
            }
            //...
            settimeofday(&tv, NULL);
            if (setuid(uid))
            { //恢复uid
            }

            printf("%s \n", ctime((time_t*) &tv.tv_sec));
        }
    }
    else
    {
        printf("not select event\n");
    }

    close(sockfd);
}

} // namespace time
} // namespace common
