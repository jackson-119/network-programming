import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import FluentUI 

RowLayout {

    spacing: 10
    height: 50
    implicitHeight: 50  // 添加隐式高度

    FluTextBox {
        id: messageInput
        Layout.fillWidth: true
        placeholderText: "输入消息..."
        onAccepted: sendButton.clicked()
    }

    FluFilledButton {
        id: sendButton
        text: "发送"
        onClicked: {
            if (messageInput.text !== "") {
                // 这里应该添加发送消息的逻辑
                var nowTime = new Date();
                client.sendChatInfo({
                    receiver: peerNickname,
                    sender: myNickname,
                    content: messageInput.text,
                    timestamp: nowTime.getTime()
                });
                if (!allChatMessages.hasOwnProperty(peerNickname)) {
                    allChatMessages[peerNickname] = [];
                }
                allChatMessages[peerNickname].push({
                    avatar: allUsers[myNickname].avatar,
                    message: messageInput.text,
                    sentTime: nowTime,
                    isOwnMessage: true,
                    userName: myNickname
                });
                messageList.updateMessages(allChatMessages[peerNickname]);
            }
            messageInput.text = "";
        }
    }
}
