#ifndef SAMPLE_H
#define SAMPLE_H

#include <QtGlobal>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QLibrary>
#include <QObject>
#include <cstring>
#include <string>
#include <array>
#include <cstdint>
#include <QTimer>
#include <QObject>
#include <QVector>
#include <QDebug>
#include <QList>
#include <QVariant>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <libusb-1.0/libusb.h>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>
#include <Windows.h>

/*
class DataProvider : public QObject {
    Q_OBJECT
public:
    DataProvider(QObject* parent = nullptr);
    Q_INVOKABLE QList<QVariant> getData() const ;
    //Q_INVOKABLE  void saveToCSV(const QVector<QVector<double>>& newData,int startTime, int samplesPerChannel);
    Q_INVOKABLE  void saveToEDF(const QVector<QVector<double>>& newData, int startTime, int samplesPerChannel);

signals:
     void dataUpdated();

public slots:
    Q_INVOKABLE void generateData();
    Q_INVOKABLE void startGenerating(); // 开始生成数据
    Q_INVOKABLE void stopGenerating();  // 停止生成数据

private:
     void startTimer();
     void stopTimer();
     QVector<QVector<double>> channelData;
     int sampleRate;
     int numChannels;
     QTimer* timer;  // 定时器
     bool generating;  // 是否正在生成数据
     QString filename;  // 用于保存文件的文件名，只生成一次
     int handle;            // EDF 文件句柄
     bool fileOpen;  // 标志文件是否已经打开
};
*/

class SerialPortHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortHandler(QObject *parent = nullptr);
    ~SerialPortHandler();

    Q_INVOKABLE bool openPort(const QString &portName, int baudRate);
    Q_INVOKABLE void closePort();
    Q_INVOKABLE void sendData(const QString &data);

private:
    QSerialPort *serialPort;
};

class USBHandler : public QObject {
    Q_OBJECT

public:
    explicit USBHandler(QObject *parent = nullptr);
    ~USBHandler();

    Q_INVOKABLE bool sendData(int vendorId, int productId, const QByteArray &data);

private:
    libusb_context *context;
};

class BluetoothHandler : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothHandler(QObject *parent = nullptr);
    ~BluetoothHandler();

    Q_INVOKABLE void connectToDevice(const QString &deviceAddress);
    Q_INVOKABLE void sendData(const QByteArray &data);
    Q_INVOKABLE void disconnectFromDevice();

private:
    QBluetoothSocket *socket;
};

class InpOutHandler : public QObject
{
    Q_OBJECT
public:
    explicit InpOutHandler(QObject *parent = nullptr);
    ~InpOutHandler();

    Q_INVOKABLE void sendData(const QString &portAddress, const QString &data);

private:
    HMODULE hDll;  // 保存加载的 DLL 句柄
    bool isPortOpened; // 标记并口是否已经打开

    typedef void (*Out32Func)(unsigned short portAddress, unsigned char data);
    typedef unsigned char (*Inp32Func)(unsigned short portAddress);

    Out32Func Out32;  // Out32 函数指针
    Inp32Func Inp32;  // Inp32 函数指针

    void openPort();
    void closePort();
};

#endif // SAMPLE_H
