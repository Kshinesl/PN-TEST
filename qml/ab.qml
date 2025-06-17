import QtQuick 2.0
import QtQuick.Window 2.0
import Amplifier.module 1.0
import DataProvider.module 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtWebSockets 1.0

Window {
    width: 2400
    height: 1000

    visible: true
    title: "SampleProject"

    Bpp01 {
        id: mainScreen01
    }


}


/*
Window {
    id: window
    width: 800
    height: 500

    visible: true
    title: qsTr("Hello World")

    Amplifier {
    id: amplifier
    }

    Component.onCompleted: {
        console.log("Amplifier State:", amplifier.getAmplifierState());
        console.log("AD Unit:", amplifier.getADUnit());
    }
}
*/

