#ifndef UDPCONNECTOR_H
#define UDPCONNECTOR_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>

#ifdef _WIN32
#include <Ws2tcpip.h>
#include <winsock2.h>

#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

const int defaultLocalPort = 22222;
const int defaultServerPort = 22222;
const std::string defaultServerIp = "127.0.0.1";

class UdpConnector {
  public:
    UdpConnector(const UdpConnector &) = delete;
    UdpConnector &operator=(const UdpConnector &) = delete;
    ~UdpConnector();

    static UdpConnector &getInstance() {
        static UdpConnector instance;
        return instance;
    }

    // 初始化本地socket，启动发送和接收线程
    void initialize();

    // 发送序列化后的消息
    void sendMsgAsync(const std::string &msg);
    void sendMsgAsync(const std::string &msg, const std::string &ip, int port);
    void sendMsgAsync(const std::string &msg, const sockaddr &addr);

    void setRemoteAddr(const std::string &ip, int port);
    int getLocalPort() const { return localPort; }

    void setOnRecvMsg(
        const std::function<void(const std::string &, const std::string &, int)>
            &onRecvMsg_) {
        onRecvMsg = onRecvMsg_;
    }

  private:
    UdpConnector() = default;

    // 执行底层发送和接受的线程函数
    void sendLoop();
    void recvLoop();

    // 将ip和port填充到sockaddr中
    void fillAddr(sockaddr &addr, const std::string &ip, int port);
    void fillAddr(sockaddr &addr,
                  const std::string &ip,
                  const std::string port);

  private:
    std::atomic<int> socketFd = -1;
    int localPort = defaultLocalPort;
    std::optional<sockaddr> remoteAddr;
#ifdef _WIN32
    const int addrLen = sizeof(sockaddr_in);
#else
    const socklen_t addrLen = sizeof(sockaddr_in);
#endif
    std::queue<std::pair<sockaddr, std::string>> sendMsgQueue;
    std::queue<std::pair<sockaddr, std::string>> recvMsgQueue;
    std::mutex sendQueueMutex;
    std::mutex recvQueueMutex;
    std::condition_variable cv;
    std::atomic<bool> stop = false;
    std::thread sendThread;
    std::thread recvThread;

    std::function<void(const std::string &, const std::string &, int)>
        onRecvMsg;
};

#endif  // UDPCONNECTOR_H
