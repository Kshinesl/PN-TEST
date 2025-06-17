/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/

import QtQuick 2.9
import QtQuick.Window 2.0
import Amplifier.module 1.0
import DataProvider.module 1.0
import SerialPort.module 1.0
import Usb.module 1.0
import Bluetooth.module 1.0
import InpOut.module 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtCharts 2.0
import Qt.labs.platform 1.0
import QtWebSockets 1.0

Rectangle {
    id: rectangle
    width: 2400
    height: 1000


    property bool isReading: false  // 用于控制是否显示波形

    Text {
        id: label
        y: 15
        text: qsTr("Hello SampleProject")
        anchors.topMargin: 45
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter

        SequentialAnimation {
            id: animation

            ColorAnimation {
                id: colorAnimation1
                target: rectangle
                property: "color"
                to: "#2294c6"
                from: "#2294c6"
            }

            ColorAnimation {
                id: colorAnimation2
                target: rectangle
                property: "color"
                to: "#2294c6"
                from: "#2294c6"
            }
        }
    }

    Row {
        spacing: 20
        anchors.centerIn: parent

   Rectangle {
        id:ads
        color: "#ffffff"
        border.width: 1
        width: 500
        height: 900

        ColumnLayout {
                    spacing: 10
                    anchors.centerIn: parent

                    Text {
                        text: "并口传输："
                        font.pixelSize: 16
                    }

                    TextField {
                        id: portField
                        placeholderText: "输入并口地址"
                        font.pixelSize: 16
                        width: 300
                        height: 50
                    }

                    TextField {
                        id: dataField
                        placeholderText: "输入要发送的消息"
                        font.pixelSize: 16
                        width: 300
                        height: 50
                    }


                    Button {
                        Layout.alignment: Qt.AlignHCenter
                        text: "发送数据（并口）"
                        font.pixelSize: 16
                        Layout.preferredWidth: 150
                        height: 50
                        onClicked: {
                            inpOutHandler.sendData(portField.text, dataField.text)
                        }
                    }

                    Text {
                        text: "串口传输："
                        font.pixelSize: 16
                    }

                    TextField {
                        id: portNameField
                        placeholderText: "输入串口号 (e.g., COM1)"
                        font.pixelSize: 16
                        width: 300
                        height: 50
                    }

                    TextField {
                        id: messageField
                        placeholderText: "输入要发送的消息"
                        font.pixelSize: 16
                        width: 300
                        height: 50
                    }

                    Button {
                        Layout.alignment: Qt.AlignHCenter
                        text: "打开串口"
                        font.pixelSize: 16
                        Layout.preferredWidth: 150
                        height: 50
                        onClicked: {
                            serialPortHandler.openPort(portNameField.text, 9600)
                        }
                    }

                    Button {
                        Layout.alignment: Qt.AlignHCenter
                        text: "发送数据（串口）"
                        font.pixelSize: 16
                        Layout.preferredWidth: 150
                        height: 50
                        onClicked: {
                            serialPortHandler.sendData(messageField.text)
                        }
                    }

                    Button {
                        Layout.alignment: Qt.AlignHCenter
                        text: "关闭串口"
                        font.pixelSize: 16
                        Layout.preferredWidth: 150
                        height: 50
                        onClicked: {
                            serialPortHandler.closePort()
                        }
                    }

                    Text {
                        text: "USB传输："
                        font.pixelSize: 16
                    }

                    TextField {
                                id: dataInput
                                placeholderText: "输入要发送的消息"
                                font.pixelSize: 16
                                width: 300
                                height: 50
                            }

                            Button {
                                Layout.alignment: Qt.AlignHCenter
                                text: "发送数据（USB）"
                                font.pixelSize: 16
                                Layout.preferredWidth: 150
                                height: 50
                                onClicked: {
                                    // 调用 C++ 中的 sendData 方法
                                    var data = dataInput.text;
                                    var vendorId = 0x12D1;  // 示例 vendorId
                                    var productId = 0x107E; // 示例 productId
                                    usbHandler.sendData(vendorId, productId, data);
                                }
                            }

                            Text {
                                text: "蓝牙传输："
                                font.pixelSize: 16
                            }

                            TextField {
                                id: addressInput
                                font.pixelSize: 16
                                width: 300
                                height: 50
                                placeholderText: "输入设备地址"
                            }

                            Button {
                                Layout.alignment: Qt.AlignHCenter
                                text: "连接蓝牙"
                                font.pixelSize: 16
                                Layout.preferredWidth: 150
                                height: 50
                                onClicked: {
                                    bluetoothHandler.connectToDevice(addressInput.text)
                                }
                            }

                            TextField {
                                id: blueInput
                                font.pixelSize: 16
                                width: 300
                                height: 50
                                placeholderText: "输入要发送的消息"
                                        }


                            Button {
                                Layout.alignment: Qt.AlignHCenter
                                text: "发送数据（蓝牙）"
                                font.pixelSize: 16
                                Layout.preferredWidth: 150
                                height: 50
                                onClicked: {
                                    var data = blueInput.text
                                    bluetoothHandler.sendData(data)
                                }
                            }
                            Button {
                                Layout.alignment: Qt.AlignHCenter
                                text: "断开蓝牙连接"
                                font.pixelSize: 16
                                Layout.preferredWidth: 150
                                height: 50
                                onClicked: {
                                    bluetoothHandler.disconnectFromDevice()
                                }
                            }


                }

            InpOutHandler {
                id: inpOutHandler
            }
        // 创建一个 SerialPortHandler 对象，连接 C++ 代码
            SerialPortHandler {
                id: serialPortHandler
            }

        // 创建 USBHandler 对象并绑定到 QML
            USBHandler {
                id: usbHandler
            }

        // 创建 蓝牙Handler 对象并绑定到 QML
            BluetoothHandler {
                   id: bluetoothHandler
               }
           }

Rectangle{
    id:ans
    width:1800
    height:900
    Rectangle {
        id: rectangle1
        anchors.left:ans.left
        anchors.top:ans.top
        width: parent.width * 7/10
        height: parent.height
        color: "#ffffff"
        border.width: 1

        Canvas {
            id: canvas
            anchors.fill: parent
            onPaint: {
                if(!isReading) return; //在没有开始读取的时候，不进行波形绘制

                var context = canvas.getContext("2d");
                context.clearRect(0, 0, canvas.width, canvas.height);  // 清除画布

                // 设置绘图样式
                context.strokeStyle = "blue";  // 波形颜色
                context.lineWidth = 1;  // 波形线宽
                context.beginPath();  // 开始新的路径

                // 获取通道数据并绘制
                var allData = dataProvider.getData();
                if (allData.length === 0) {
                    console.log("No data available");  // 如果没有数据，打印日志
                              return;  // 如果没有数据，直接返回
                          }
                var step = canvas.width / (allData[0].length/5);  // 根据数据点数量计算步长
                var channelHeight = canvas.height / allData.length;  // 每个通道分配的高度
                for (var ch = 0; ch < allData.length; ch++) {
                    if (allData[ch].length === 0) {
                                       continue;  // 跳过没有数据的通道
                                   }
                    var channelOffset = ch * channelHeight;

                    for (var i = 0; i < allData[ch].length; i++) {
                        var x = i * step;  // 计算 X 坐标

                        var y = channelOffset + (1 - (allData[ch][i] + 1) / 2) * channelHeight * 0.01/5 ;  // 计算 Y 坐标，将数据值映射到画布高度
                        if (i === 0) {
                            context.moveTo(x, y);  // 移动到起点
                        } else {
                            context.lineTo(x, y);  // 绘制线段
                        }
                    }
                }

                context.stroke();  // 结束绘制
            }

            // 实例化 DataProvider
            DataProvider {
                id: dataProvider
                }
            // 连接 dataUpdated 信号
                   Connections {
                       target: dataProvider
                       onDataUpdated: {
                           if(isReading){
                           canvas.requestPaint();  // 触发画布重绘
                           }
                       }
                   }
                   Component.onCompleted: {
                      //    dataProvider.generateData();  // 启动数据生成
                      }
            }
    }

    Rectangle {
        id: rectangle2
        anchors.left:rectangle1.right
        anchors.leftMargin: 20
        anchors.top:rectangle1.top
        width: parent.width* 3/10
        height: rectangle1.height
        color: "#ffffff"
        border.width: 1

        Amplifier {
        id: amplifier
        }

        Rectangle {
            id: rectangle3
            x: 18
            y: 16
            width: rectangle2.width-35
            height: 30
            visible: true
            color: "#ffffff"
            border.width: 0

            Text {
                id: _text
                x: 30
                y: 16
                width: 23
                height: 8
                text: qsTr("放大器型号：")
                font.pixelSize: 16
            }

            ComboBox {
                id: comboBox
                x: 140
                y: 16
                width: 120
                height: 22
                displayText: ""
                textRole: qsTr("")
            }
        }

        Rectangle {
            id: rectangle4
            x: 18
            y: 100
            width: rectangle2.width-35
            height: 100
            visible: true
            color: "#ffffff"
            border.width: 0

            Button {
                id: button
                x: 25
                y: 0
                width: 200
                height: 50
                text: qsTr("连接放大器")
                font.pixelSize: 16
                onClicked: {
                    amplifier.startSocketAndAmplifier();
                    amplifier.allstartAmplifier();
                    isReading = false;  // 确保连接放大器时不开始绘制波形

                }
            }

            Button {
                id: button1
                x: 260
                y: 0
                width: 200
                height: 50
                text: qsTr("保存数据")
                font.pixelSize: 16
                onClicked: {
                    amplifier.stopAmplifier();
                    isReading = false;  // 停止放大器时也停止绘制波形
                }
            }

            Button {
                id: button2
                x: 25
                y: 98
                width: 200
                height: 50
                text: qsTr("开始读取")
                font.pixelSize: 16
                onClicked: {
                    amplifier.startReading();
                    dataProvider.startGenerating();  // 开始生成数据
                    isReading = true;  // 点击"开始读取"按钮后，才开始显示波形
                    amplifier.startMonitoringMarking()  // 启动定时获取打标信息
                }
            }

            Button {
                id: button3
                x: 260
                y: 98
                width: 200
                height: 50
                text: qsTr("停止读取")
                font.pixelSize: 16
                onClicked: {
                dataProvider.stopGenerating();  // 停止生成数据
                amplifier.stopAmplifier();
                isReading = false;  // 停止放大器时也停止绘制波形
                amplifier.stopMonitoringMarking()  // 停止定时获取打标信息
                }
            }

        }

        Rectangle {
            id: rectangle5
            x: 18
            y: 312
            width: rectangle2.width-35
            height: 90
            color: "#ffffff"
            border.width: 0

            ComboBox {
                id: comboBox2
                x: 30
                y: 26
                width: 101
                height: 25
                displayText: ""
                textRole: qsTr("")
            }

            ComboBox {
                id: comboBox3
                x: 30
                y: 114
                width: 101
                height: 25
                textRole: qsTr("")
                displayText: ""
            }

            Button {
                id: button4
                x: 160
                y: 0
                width: 200
                height: 50
                text: qsTr("阻抗测试")
                font.pixelSize: 16
            }

            Button {
                id: button5
                x: 160
                y: 94
                width: 200
                height: 50
                text: qsTr("设置阻抗门槛")
                font.pixelSize: 16
            }
        }

        Rectangle {
            id: rectangle6
            x: 18
            y: 522
            width: rectangle2.width-35
            height: 170
            color: "#ffffff"
            border.width: 0
            Button {
                id: button10
                x: 25
                y: 0
                width: 200
                height: 50
                text: qsTr("开启定标")
                font.pixelSize: 16
            }

            Button {
                id: button11
                x: 260
                y: 0
                width: 200
                height: 50
                text: qsTr("关闭定标")
                font.pixelSize: 16
            }

            Button {
                id: button21
                x: 25
                y: 98
                width: 200
                height: 50
                text: qsTr("放大器转换单位")
                font.pixelSize: 16
            }

            Button {
                id: button31
                x: 260
                y: 98
                width: 200
                height: 50
                text: qsTr("获取50HzFFT值")
                font.pixelSize: 16
            }

            Button {
                id: button12
                x: 25
                y: 196
                width: 200
                height: 50
                text: qsTr("读取阻抗和手工打标信息")
                font.pixelSize: 12
            }

            Button {
                id: button13
                x: 260
                y: 196
                width: 200
                height: 50
                text: qsTr("激活刺激器")
                font.pixelSize: 16
            }

             Text  {
                id: markingLabel
                x: 25
                y: 294
                //width: 200
                //height: 50
                text: "等待打标信息..."
                font.pixelSize: 16
                Connections {
                    target: amplifier  // amplifier 是你通过 QML 注册的 Amplifier 对象
                    onMarkingUpdated: {
                      // console.log("Received marking update: " + message);
                        markingLabel.text = message  // 更新界面显示的打标信息
                    }
                }
            }

             // 连接 Amplifier 的 markingUpdated 信号到更新 UI 的槽


        }

    }
}
}
    states: [
        State {
            name: "clicked"

            PropertyChanges {
                target: label
                text: qsTr("Button Checked")
            }
        }
    ]



}
