import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: messageListView
    clip: true
    model: ListModel {}
    delegate: MessageBubble {}
    spacing: 10

    // 更新消息的方法
    function updateMessages(messages) {
        if (messages) {
            model.clear();
            for (var i = 0; i < messages.length; i++) {
                model.append(messages[i]);
            }
        }
    }

    // 添加新消息的方法
    function addNewMessage(message) {
        model.append(message);
        positionViewAtEnd();
    }

    // 保持垂直方向从上到下
    verticalLayoutDirection: ListView.TopToBottom
    flickableDirection: Flickable.VerticalFlick
}
