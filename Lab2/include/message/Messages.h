#ifndef MESSAGES_H
#define MESSAGES_H

#include "cereal/archives/portable_binary.hpp"
#include "cereal/types/string.hpp"

struct UserInfo {
    std::string nickName;
    std::string ip;
    int port;
    std::string avatar;
    std::string avatarFormat;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(nickName),
           CEREAL_NVP(ip),
           CEREAL_NVP(port),
           CEREAL_NVP(avatar));
    }
};

struct GroupInfo {
    std::string groupName;
    std::vector<std::string> members;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(groupName), CEREAL_NVP(members));
    }
};

#endif  // MESSAGES_H
