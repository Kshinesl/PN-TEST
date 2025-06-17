import QtQuick 2.0
import QtQuick.Window 2.0
import Amplifier.module 1.0
import DataProvider.module 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtWebSockets 1.0
StackView {
    id: stack
    focus: true

property string prompt_text


Screen01 {
    id: mainScreen
}

WebSocket {
    id: socket//客户端，需要后打开
    url: "ws://127.0.0.1:38861"
    active: true
    onStatusChanged:
    {
            if (socket.status == WebSocket.Error) {
                console.log("WebSocket 连接错误: " + socket.errorString)
            } else if (socket.status == WebSocket.Open) {
                console.log("WebSocket 连接成功")
                send({message: "Connected"})
            } else if (socket.status == WebSocket.Closed) {
                console.log("WebSocket 已关闭")
            }
        }
    //通过 onTextMessageReceived 事件监听来接收服务器的消息
    onTextMessageReceived:{
        console.log("收到服务器消息: ", message)
        prompt_text = message
    }
    function send(message) {
             socket.sendTextMessage(JSON.stringify(message))
               console.log("发送消息: ", JSON.stringify(message))
    }
}
}
