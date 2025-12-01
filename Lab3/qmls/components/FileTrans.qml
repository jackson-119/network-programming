import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import FluentUI

Rectangle {
    id: fileTransferBar
    width: parent.width

    color: "transparent"
    property var currentFileInfo: null

    RowLayout {
        anchors.fill: parent
        spacing: 10

        // 文件选择SVG图标
        Item {
            id: fileIcon
            Layout.preferredWidth: 30
            Layout.preferredHeight: 30
            Layout.alignment: Qt.AlignVCenter

            Canvas {
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.fillStyle = "#020202";
                    ctx.beginPath();
                    ctx.moveTo(28.5, 6.5);
                    ctx.lineTo(13.37125, 6.5);
                    ctx.lineTo(11.7975, 3.55925);
                    ctx.quadraticCurveTo(10.03375, 2.5, 10.03425, 2.5);
                    ctx.lineTo(3.5, 2.5);
                    ctx.quadraticCurveTo(2.4, 2.5, 2.4, 3.5);
                    ctx.lineTo(2.4, 26.5);
                    ctx.quadraticCurveTo(2.4, 27.5, 3.5, 27.5);
                    ctx.lineTo(28.5, 27.5);
                    ctx.quadraticCurveTo(29.6, 27.5, 29.6, 26.5);
                    ctx.lineTo(29.6, 7.5);
                    ctx.quadraticCurveTo(29.6, 6.5, 28.5, 6.5);
                    ctx.closePath();
                    ctx.moveTo(3.5, 4.5);
                    ctx.lineTo(10.03425, 4.5);
                    ctx.lineTo(12.175, 8.5);
                    ctx.lineTo(28.5, 8.5);
                    ctx.lineTo(28.5, 11.56575);
                    ctx.lineTo(3.5, 11.56575);
                    ctx.lineTo(3.5, 4.5);
                    ctx.closePath();
                    ctx.moveTo(3.5, 25.5);
                    ctx.lineTo(3.5, 22.5);
                    ctx.lineTo(3.539, 11.56575);
                    ctx.lineTo(28.4615, 11.6);
                    ctx.lineTo(28.4615, 24.5);
                    ctx.lineTo(28.463, 25.5);
                    ctx.lineTo(3.5, 25.5);
                    ctx.closePath();
                    ctx.fill();
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: fileDialog.open()
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5

            // 文件名
            FluCopyableText {
                id: fileName
                Layout.fillWidth: true
                text: currentFileInfo ? currentFileInfo.fileName : "请选择文件"
                // elide: Text.ElideMiddle
                font.pixelSize: 12
            }

            // 进度条
            FluProgressBar {
                id: progressBar
                indeterminate: false
                Layout.fillWidth: true
                height: 15
                from: 0
                to: 100
                value: currentFileInfo ? currentFileInfo.progress : 0

                Text {
                    anchors.centerIn: parent
                    text: {
                        if (currentFileInfo) {
                            var percent = (currentFileInfo.recvedSize / currentFileInfo.totalSize * 100).toFixed(1);
                            var speed = (currentFileInfo.speed / 1024 / 1024).toFixed(2); // Convert to MB/s
                            return percent + "% - " + speed + " MB/s";
                        }
                        return "0% - 0 MB/s";
                    }
                    font.pixelSize: 10
                    color: "black"
                }
            }
        }

        // 打开文件按钮
        Button {
            id: openFileButton
            text: "打开"
            visible: currentFileInfo && currentFileInfo.recvedSize === currentFileInfo.totalSize
            Layout.preferredWidth: 60
            Layout.preferredHeight: 30
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                if (currentFileInfo) {
                    console.log("打开文件: " + currentFileInfo.localPath);
                    // 这里添加打开文件的逻辑
                }
            }
        }
    }

    function updateFileInfo(fileInfo) {
        currentFileInfo = fileInfo;
    }

    FileDialog {
        id: fileDialog
        title: "请选择文件或文件夹"
        fileMode: FileDialog.OpenFile
        onAccepted: {
            fileName.text = fileDialog.selectedFile.toString().split("/").pop();
            client.sendFileInfo({
                filePath: fileDialog.selectedFile.toString().replace("file://", ""),
                receiver: peerNickname,
                sender: myNickname,
                fileName: fileName.text,
                // TODO Dir Support
                isDir: false
            });
        }
    }

    function getTransferRate() {
        return (Math.random() * 10).toFixed(1);
    }

    Timer {
        id: fakeTransferTimer
        interval: 100
        repeat: true
        running: false
        onTriggered: {
            if (progressBar.value < 100) {
                progressBar.value += 1;
            } else {
                running = false;
                openFileButton.visible = true;
            }
        }
    }

    function startFakeTransfer() {
        progressBar.value = 0;
        openFileButton.visible = false;
        fakeTransferTimer.start();
    }
}
