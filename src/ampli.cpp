#include "Samples.h"
#include <QDebug>

uint Amplifier::getAmplifierState(){
        typedef int (*StateFunc)();
        StateFunc state = (StateFunc)sampleDll.resolve("GetAmplifierState");
        if (state) {
            int result = state();  // 调用函数并获取返回值
            qDebug() << "Amplifier State:" << result;
            return result;  // 返回 result
        }else{
            qDebug() << "Failed to resolve GetAmplifierState function";
            return 0;
        }  // 如果函数指针解析失败，返回 0
    }

 float Amplifier::getADUnit() {
            typedef float (*UnitFunc)();
            UnitFunc unit = (UnitFunc) sampleDll.resolve("GetADUnit");
            if (unit) {
                qDebug() << unit();
            }
            return 0;
    }
/*
class Amplifier : public QObject {
public:
    Q_INVOKABLE uint getAmplifierState() {
        typedef int (*StateFunc)();
        StateFunc state = (StateFunc)sampleDll.resolve("GetAmplifierState");
        if (state) {
            qDebug() << state();
        }
        return 0;
    }

    Q_INVOKABLE float getADUnit() {
        typedef float (*UnitFunc)();
        UnitFunc unit = (UnitFunc) sampleDll.resolve("GetADUnit");
        if (unit) {
            qDebug() << unit();
        }
        return 0;
    }

private:
    QLibrary sampleDll;
};
*/
