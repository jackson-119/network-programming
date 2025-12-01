#include "Logger.h"
#include "UdpConnector.h"

UdpConnector::~UdpConnector() {
    stop = true;
    cv.notify_all();
    if (sendThread.joinable()) {
        sendThread.join();
    }

    if (socketFd.load() != -1) {
#ifdef _WIN32
        closesocket(socketFd.load());
        WSACleanup();
#else
        close(socketFd.load());
#endif
    }
}

void UdpConnector::sendMsgAsync(const std::string &msg) {
    if (!remoteAddr.has_value()) {
        logWarning("Try to send message without remote address");
        return;
    }
    sendMsgAsync(msg, remoteAddr.value());
}

void UdpConnector::sendMsgAsync(const std::string &msg,
                                const std::string &ip,
                                int port) {
    sockaddr addr;
    fillAddr(addr, ip, port);
    sendMsgAsync(msg, addr);
}

void UdpConnector::sendMsgAsync(const std::string &msg, const sockaddr &addr) {
    {
        std::lock_guard<std::mutex> lock(sendQueueMutex);
        sendMsgQueue.emplace(addr, msg);
    }
    cv.notify_one();
}

void UdpConnector::setRemoteAddr(const std::string &ip, int port) {
    if (!remoteAddr.has_value()) {
        remoteAddr = sockaddr();
    }
    fillAddr(remoteAddr.value(), ip, port);
}

void UdpConnector::sendLoop() {
    while (true) {
        std::unique_lock<std::mutex> lock(sendQueueMutex);
        cv.wait(lock, [this] { return !sendMsgQueue.empty() || stop; });

        if (stop) {
            break;
        }
        auto [addr, msg] = sendMsgQueue.front();
        sendMsgQueue.pop();
        lock.unlock();

        if (socketFd.load() == -1) {
            logWarning("Try to send message without socket");
            continue;
        }

        auto sentSize =
            sendto(socketFd.load(), msg.c_str(), msg.size(), 0, &addr, addrLen);
        if (sentSize < 0) {
            logWarning("Failed to send message");
        }
    }
}

void UdpConnector::recvLoop() {
    while (true) {
        if (stop) break;
        char buffer[65536];  // UDP最大数据包大小
        struct sockaddr recv_addr;
#ifdef _WIN32
        int tempAddrLen = sizeof(recv_addr);
#else
        socklen_t tempAddrLen = sizeof(recv_addr);
#endif
        auto recv_size = recvfrom(socketFd.load(),
                                  buffer,
                                  sizeof(buffer),
                                  0,
                                  &recv_addr,
                                  &tempAddrLen);
        if (recv_size < 0) {
            logWarning("Failed to receive message");
            continue;
        }
        std::string msg = std::string(buffer, recv_size);
        int port;
        char *ip =
            inet_ntoa(reinterpret_cast<sockaddr_in &>(recv_addr).sin_addr);
        port = ntohs(reinterpret_cast<sockaddr_in &>(recv_addr).sin_port);
        onRecvMsg(msg, ip, port);
    }
}

void UdpConnector::fillAddr(sockaddr &addr, const std::string &ip, int port) {
    sockaddr_in &sock_address = reinterpret_cast<sockaddr_in &>(addr);
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons(port);
    if (ip.size() == 0) {
        sock_address.sin_addr.s_addr = INADDR_ANY;
    } else {
        sock_address.sin_addr.s_addr = inet_addr(ip.c_str());
    }
}

void UdpConnector::fillAddr(sockaddr &addr,
                            const std::string &ip,
                            const std::string port) {
    fillAddr(addr, ip, std::stoi(port));
}

void UdpConnector::initialize() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        logError("Failed to initialize winsock");
        return;
    }
#endif

    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd.load() == -1) {
        logError("Failed to create socket");
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }
    constexpr int BUFFER_SIZE = 1024 * 1024;
    int result = setsockopt(socketFd.load(),
                            SOL_SOCKET,
                            SO_RCVBUF,
                            (const char *)&BUFFER_SIZE,
                            sizeof(BUFFER_SIZE));
    if (result < 0) {
        logError("Failed to set socket receive buffer size");
    }

    result = setsockopt(socketFd.load(),
                        SOL_SOCKET,
                        SO_SNDBUF,
                        (const char *)&BUFFER_SIZE,
                        sizeof(BUFFER_SIZE));
    if (result < 0) {
        logError("Failed to set socket send buffer size");
    }

    localPort = defaultLocalPort;
    sockaddr localAddr;
    while (localPort < 65535) {
        fillAddr(localAddr, "", localPort);
        if (bind(socketFd.load(), &localAddr, addrLen) == 0) {
            break;
        }
        localPort++;
    }
    if (localPort == 65535) {
        logError("Failed to bind socket");
        return;
    }

    sendThread = std::thread(&UdpConnector::sendLoop, this);
    recvThread = std::thread(&UdpConnector::recvLoop, this);
}
