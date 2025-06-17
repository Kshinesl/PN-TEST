#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>
#include <QtCore/QObject>
#include <QtCharts/QChart>
#include <QtQml/QQmlContext>
#include <QGuiApplication>
#include <QApplication>
#include <QQmlContext>
//#include "Samples.cpp"
#include "Samples.h"
#include <QDebug>
#include <vector>
#include <thread>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <QtConcurrent>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QBluetoothAddress>
#include <QtWebSockets/QtWebSockets>
#include "edflib.h"
//#include "inpout32.h"

//******************************************这部分是调用dll库的函数声明
QLibrary sampleDll("Sample.dll");
//QLibrary impoutDll("impout32.dll");

AMPLIFIER_INFO aMPLIFIER_INFO;
//声明了一个名为 aMPLIFIER_INFO 的变量，其类型是 AMPLIFIER_INFO
std::vector<CHANNEL_INFO> channelInfos;
// 声明一个名为 channelInfos 的数组，其元素类型是 CHANNEL_INFO
IMPED_TRIG_INFO iMPED_TRIG_INFO;

bool isOpen = false;
bool IsReading = false;

Amplifier::Amplifier(QObject* parent)
    : QObject(parent),lastByteTrigFlag(0),
    markingTimer(new QTimer(this))
{
    // 设置定时器，每隔50毫秒触发一次
    markingTimer->setInterval(50);  // 你可以根据需要调整时间间隔
    connect(markingTimer, &QTimer::timeout, this, &Amplifier::checkMarking);
}

void Amplifier::startMonitoringMarking()
{
    if (!markingTimer->isActive()) {
        markingTimer->start();  // 启动定时器
    }
}

void Amplifier::stopMonitoringMarking()
{
    markingTimer->stop();  // 停止定时器
}


void Amplifier::InitializeAmplifierSettings(){

    AmplifierInfo amplifier;
    aMPLIFIER_INFO.nAmpIndex = 13;
    aMPLIFIER_INFO.byteAMPType = 3;
    aMPLIFIER_INFO.nModeOfSign = 1;
    aMPLIFIER_INFO.nStimERPRealCH = -2;
    aMPLIFIER_INFO.nStimERPRecvCH = -1;
    aMPLIFIER_INFO.nStimERPValue = 0;
    aMPLIFIER_INFO.nTMSPulseWid = 20;
    aMPLIFIER_INFO.byteAMP1IP4 = 50;
    aMPLIFIER_INFO.byteAMP2IP4 = 49;
    aMPLIFIER_INFO.byteAMP3IP4 = 48;
    aMPLIFIER_INFO.byteAMP4IP4 = 47;
    aMPLIFIER_INFO.nAmpSampleRate = amplifier.SampleRate = 2048;//放大器的采样率
    aMPLIFIER_INFO.nSamplesAddRate = 1;
    aMPLIFIER_INFO.nAmpTotalChannels = amplifier.Channels = 64;//总通道数
    aMPLIFIER_INFO.nExtChannels = 0;
    // 临时计算（调用时不需要赋值）
    aMPLIFIER_INFO.nAmpChannels = amplifier.Channels = 64;
    aMPLIFIER_INFO.nTmsPulseVv = 0;
    aMPLIFIER_INFO.nFrames = 1;
    aMPLIFIER_INFO.nStimuChannelBefore = aMPLIFIER_INFO.nStimuChannelAfter = -1;

    // 初始化大小为 aMPLIFIER_INFO.nAmpTotalChannels：
    channelInfos.resize(aMPLIFIER_INFO.nAmpTotalChannels);

    // 初始化每个通道的信息
    for (int i = 0; i < aMPLIFIER_INFO.nAmpTotalChannels; i++) {
        channelInfos[i].bOn = true;
        channelInfos[i].bUnipolar = true;
        channelInfos[i].iSampleRate = 256; // 通道采样率
        std::string channelName = std::to_string(i + 1);  // 将索引转为字符串
        std::strncpy(channelInfos[i].strChannelName, channelName.c_str(), sizeof(channelInfos[i].strChannelName) - 1);
        std::strncpy(channelInfos[i].strSensorType, "EEG", sizeof(channelInfos[i].strSensorType) - 1);
        channelInfos[i].bIsPhotic = false;
        channelInfos[i].bIsStimERP = false;
        channelInfos[i].bIsStimTMS = false;
    }
    // 输出测试
    for (int i = 0; i < aMPLIFIER_INFO.nAmpTotalChannels; i++) {
        qDebug() << "Channel " << i + 1 << ": " << channelInfos[i].strChannelName
                  << ", Type: " << channelInfos[i].strSensorType
                  << ", Sample Rate: " << channelInfos[i].iSampleRate ;
    }
}


uint Amplifier::getAmplifierState(){
    typedef uint (*StateFunc)();
    StateFunc state = (StateFunc)sampleDll.resolve("GetAmplifierState");
    if (state) {
        qDebug() << "Amplifier-State:" << state();
        //返回3221225473是0xc0000001 的十进制表示，表示ERR_AMP_NO_ERRORS无错误
        return state();  // 返回 result
    }else{
        qDebug() << "Failed to resolve GetAmplifierState function";
        return 0;
    }  // 如果函数指针解析失败，返回 0
}

float Amplifier::getADUnit() {
    typedef float (*UnitFunc)();
    UnitFunc unit = (UnitFunc) sampleDll.resolve("GetADUnit");
    if (unit) {
        qDebug() <<"Amplifier-Unit:" << unit();
        return unit();
    }else{
        qDebug() << "Failed to resolve getADUnit function";
        return 0;
    }
}

void Amplifier::startAmplifier(uint32_t byteAmpIndex) {
    // 定义函数指针类型
    typedef void (*StartAmpFunc)(uint32_t);
    // 解析DLL中的函数
    StartAmpFunc startAmp = (StartAmpFunc)sampleDll.resolve("StartAmplifier");
    if (startAmp) {
        // 调用 StartAmplifier 函数
        byteAmpIndex = 0x03;
        startAmp(byteAmpIndex);
        qDebug() << "启动放大器" ;
    } else {
        // 如果解析失败，输出错误信息
        qDebug() << "启动放大器失败";
    }
}

void Amplifier::allstartAmplifier() {
    startAmplifier(0x03);
    qDebug() << "调用startAmplifier函数成功";
}


void Amplifier::stopAmplifier() {
    // 定义函数指针类型
    typedef void (*StopAmpFunc)();
    // 解析DLL中的函数
    StopAmpFunc stopAmp = (StopAmpFunc)sampleDll.resolve("StopAmplifier");
    if (stopAmp) {
        stopAmp();
        qDebug() << "关闭放大器" ;
    } else {
        // 如果解析失败，输出错误信息
        qDebug() << "关闭放大器失败";
    }
}

bool Amplifier::openSampleSocket(AMPLIFIER_INFO AmpInfo, CHANNEL_INFO* pChannelInfo, int nCpuCount){
    // 定义函数指针类型
    typedef bool (*OpenSocketFunc)(AMPLIFIER_INFO, CHANNEL_INFO*, int);
    // 解析DLL中的函数
    OpenSocketFunc openSocket = (OpenSocketFunc) sampleDll.resolve("OpenSampleSocket");
    if(openSocket){
        InitializeAmplifierSettings();
        isOpen = openSocket(aMPLIFIER_INFO, channelInfos.data(), 4);
        qDebug() << "打开放大器成功";
        if (!isOpen) {
            // 处理打开失败的情况
            qDebug() << "打开放大器失败";
            return false;
        }
        return true;
    }else {
        // 如果解析失败，输出错误信息
        qDebug() << "Failed to resolve OpenSampleSocket function";
        return false;
    }
}

void Amplifier::startSocketAndAmplifier() {
    bool result = openSampleSocket(aMPLIFIER_INFO, channelInfos.data(), 4);
    if (result) {
        qDebug() << "调用openSampleSocket函数成功" << result;
    } else {
        qDebug() << "调用openSampleSocket函数失败" << result;
    }
}

void* Amplifier::readBuffer(int* iSamples) {
    typedef void* (*ReadFunc)(int*);
    ReadFunc readbf = (ReadFunc) sampleDll.resolve("ReadBuffer");
    if (readbf) {
        void* dataPtr = readbf(iSamples);
        return dataPtr;
    } else {
        qDebug() << "Failed to resolve ReadBuffer function";
        return nullptr; // 解析失败返回 nullptr
    }
}

void Amplifier::readAmplifier() {
   // while(true){
        int iSamples = 0;
        void* dataPtr = readBuffer(&iSamples);
        qDebug() << "Sample count:" << iSamples; // 检查样本数量
            qDebug() << "Buffer内存地址: " << dataPtr;
        if (iSamples != 0) {
            short* dataArray = static_cast<short*>(dataPtr);
            for (int i = 0; i < iSamples; ++i) {
              //  qDebug() << dataArray[i];  // 输出每个样本的值
            }
        }else {
            qDebug() << "No samples read or iSamples is 0.";
        }
        //延时50ms输出一次
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
//}
}

void Amplifier::startReading() {
    QtConcurrent::run(this, &Amplifier::readAmplifier);  // 异步调用 readAmplifier
}

void Amplifier::getImpedTrigger(IMPED_TRIG_INFO* ImpedTrig)
{
    typedef void (*GetImpedTriFunc)(IMPED_TRIG_INFO*);
    GetImpedTriFunc getImpedTri = (GetImpedTriFunc) sampleDll.resolve("GetImpedTrigger");
    if(getImpedTri){
        getImpedTri(&iMPED_TRIG_INFO);
        // 检查并发射打标信息更新信号
        //qDebug() << "读取打标信息成功";
    }else {
        // 如果解析失败，输出错误信息
        qDebug() << "Failed to resolve getImpedTrigger function";

    }
}


void Amplifier::checkMarking()
{
    /*
    // 使用 srand 设置随机种子（可以在构造函数中调用一次，避免每次调用都重新设置种子）
    static bool isSeeded = false;
    if (!isSeeded) {
        srand(static_cast<unsigned int>(time(0)));  // 使用当前时间作为种子
        isSeeded = true;
    }
    // 模拟打标信息更新：随机决定是否打标
    int randomFlag = rand() % 2;  // 生成 0 或 1
    if (randomFlag == 0) {
        iMPED_TRIG_INFO.byteTrigFlag = 0;  // 不打标
    } else {
        iMPED_TRIG_INFO.byteTrigFlag = 1;  // 打标
    }
    */

    // 调用 DLL 获取打标信息
    getImpedTrigger(&iMPED_TRIG_INFO);  // 获取最新的打标信息
    //qDebug() << "当前的 byteTrigFlag: " << iMPED_TRIG_INFO.byteTrigFlag;
    //qDebug() << "fImpedValues: " << iMPED_TRIG_INFO.fImpedValues[0];
    // 检查打标信息并发射信号
    if (iMPED_TRIG_INFO.byteTrigFlag != 0) {
        QString markingMessage = QString("打标信息：%1").arg(iMPED_TRIG_INFO.byteTrigFlag);
        qDebug() << "发射打标信号：" << markingMessage;  // 添加调试输出
        emit markingUpdated(markingMessage);  // 发射信号
    } else {
        emit markingUpdated("无打标");
    }
}
//*****************************************模拟生成波形

DataProvider::DataProvider(QObject* parent)
    : QObject(parent), sampleRate(2048), numChannels(64), timer(nullptr), generating(false) {
    // 在构造函数中生成一次文件名，程序启动时创建
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyyMMdd_HHmmss");  // 格式化时间
    filename = QString("output_%1.edf").arg(timestamp);  // 保存为成员变量
    channelData.resize(numChannels);  // 初始化通道数据
    //startTimer();
    for (int ch = 0; ch < numChannels; ++ch) {
        channelData[ch].resize(sampleRate);  // 每个通道分配足够的空间
    }
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DataProvider::generateData);
}

void DataProvider::startGenerating() {
    generating = true;  // 开始生成数据
    timer->start(50);  // 每50ms生成一次数据
    if (!fileOpen) {
        // 打开文件
        handle = edfopen_file_writeonly(filename.toStdString().c_str(), EDFLIB_FILETYPE_EDFPLUS, numChannels);
        if (handle < 0) {
            qDebug() << "Failed to open EDF file!";
            return;
        }
        fileOpen = true;
    }
    qDebug() << "Data generation started.";
}

void DataProvider::stopGenerating() {
    generating = false;  // 停止生成数据
    timer->stop();  // 停止定时器
    if (fileOpen) {
        edfclose_file(handle);  // 关闭文件
        fileOpen = false;
        qDebug() << "Data generation stopped and file closed.";
    }
}

QList<QVariant> DataProvider::getData() const {
    QList<QVariant> result;
    // 将每个通道的数据添加到 QList 中
    for (const auto& channel : channelData) {
        result.append(QVariant::fromValue(channel)); // 将 QList<double> 转换为 QVariant
    }

    return result;
}

/*
void DataProvider::generateData() {
    channelData.clear();
    QVector<double> singleChannelData;

    static int timeIndex = 0; // 用于生成动态变化的波形
    for (int ch = 0; ch < numChannels; ++ch) {
        singleChannelData.clear();
        for (int i = 0; i < sampleRate / 10; ++i) {  // 模拟每个通道的数据
            double value = std::sin(2 * M_PI * (i + timeIndex) / sampleRate * (timeIndex)) ;  // 简单正弦波+噪声
            singleChannelData.append(value);
        }
        channelData.append(singleChannelData);
    }
    timeIndex++;  // 增加时间索引

    emit dataUpdated();  // 通知QML数据更新
    //qDebug() << "Generated Data:" << channelData;  // 输出生成的数据
}

*/

void DataProvider::generateData() {
     if (!generating) return;  // 如果不在生成数据，则直接返回


    // 假设你已经在放大器类中读取数据
    Amplifier amplifier;
    int iSamples = 0;
    void* dataPtr = amplifier.readBuffer(&iSamples);

    if (iSamples != 0 && dataPtr != nullptr) {
        short* dataArray = static_cast<short*>(dataPtr);
        int samplesPerChannel = iSamples / numChannels;

        QVector<QVector<double>> newData(numChannels);  // 用于保存本次生成的数据

        // 保留波形滚动的逻辑，使用 timeIndex 实现实时从左到右的滚动
        static int timeIndex = 0;  // 用于实现滚动效果
        static int globalTimeIndex = 0;  // 全局时间索引，用于数据保存时间戳

        for (int ch = 0; ch < numChannels; ++ch) {
            for (int i = 0; i < samplesPerChannel; ++i) {
                int index = (timeIndex + i) % sampleRate;  // 用模运算实现循环
                channelData[ch][index] = dataArray[ch + i * numChannels] * 0.5;  // 根据AD单位转换数据
                double value = dataArray[ch + i * numChannels] * 0.5;  // 根据AD单位转换数据
                newData[ch].append(value);  // 保存新生成的数据
            }
        }

        // 更新时间索引，使新数据逐步覆盖旧数据，实现滚动效果
        timeIndex = (timeIndex + samplesPerChannel) % sampleRate;

        // 保存本次生成的样本数据为excel格式
        //saveToCSV(newData,globalTimeIndex, samplesPerChannel);
        // 保存本次生成的数据为EDF格式
       saveToEDF(newData, globalTimeIndex, samplesPerChannel);

        // 更新全局时间索引，确保时间连续性
        globalTimeIndex += samplesPerChannel;
    }

    emit dataUpdated();  // 通知QML数据更新
}

//***************************************导出数据为excel格式

/*
void DataProvider::saveToCSV(const QVector<QVector<double>>& newData, int startTime, int samplesPerChannel) {
    QFile file(filename);
    // 只在文件第一次写入时添加表头
    static bool isHeaderWritten = false;
    //qDebug() << "Saving file to:" << QFileInfo(file).absoluteFilePath();

    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);

        // 如果表头未写入，写入表头
        if (!isHeaderWritten) {
        out << "Time (ms)";  // 第一列为时间
        for (int ch = 0; ch < numChannels; ++ch) {
            out << ",Channel " << ch + 1;  // 每个通道都标记为 Channel 1, Channel 2 等等
        }
        out << "\n";  // 换行
        isHeaderWritten = true;
        }


        // 写入数据
        for (int i = 0; i < samplesPerChannel; ++i) {
            // 写入时间戳，确保时间是连续的
            out << QString::number((startTime + i) * (1000.0 / sampleRate));  // 将时间转换为毫秒，写入第一列

            // 写入每个通道的数据
            for (int ch = 0; ch < numChannels; ++ch) {
                out << "," << QString::number(channelData[ch][i]);  // 写入每个通道的数据
            }
            out << "\n";  // 换行
        }

        file.close();
    } else {
        qDebug() << "Failed to open file for writing";
    }
}
*/
//***************************************导出数据为edf格式


void DataProvider::saveToEDF(const QVector<QVector<double>>& newData, int startTime, int samplesPerChannel) {
    // 打开EDF文件进行写入

    if (!fileOpen) {
        qDebug() << "File is not open!";
        return;
    }

    // 设置通道的采样频率、数字最大最小值、物理范围等
    for (int i = 0; i < numChannels; ++i) {
        edf_set_samplefrequency(handle, i, sampleRate);  // 设置采样频率
        edf_set_digital_maximum(handle, i, 32768);       // 设置数字最大值
        edf_set_digital_minimum(handle, i, -32768);      // 设置数字最小值
        edf_set_physical_maximum(handle, i, 1000.0);     // 设置物理最大值（例如，1000 uV）
        edf_set_physical_minimum(handle, i, -1000.0);    // 设置物理最小值（例如，-1000 uV）
        edf_set_physical_dimension(handle, i, "uV");     // 设置物理单位（uV）
        edf_set_label(handle, i, ("Channel " + QString::number(i + 1)).toStdString().c_str()); // 设置标签
    }

    // 写入数据
    for (int i = 0; i < samplesPerChannel; ++i) {
        // 准备按物理量写入数据
        QVector<double> channelDataForWrite(numChannels);
        for (int ch = 0; ch < numChannels; ++ch) {
            channelDataForWrite[ch] = newData[ch][i]; // 从新数据中取出每个通道的数据
        }

        // 将物理数据转换为数字量并写入
        if (edfwrite_physical_samples(handle, channelDataForWrite.data()) != 0) {
            qDebug() << "Failed to write samples to EDF file!";
            break;
        }
    }

    // 关闭文件
    //edfclose_file(handle);
    qDebug() << "Data saved to EDF file successfully!";
}


//****************************************串口相关

SerialPortHandler::SerialPortHandler(QObject *parent) : QObject(parent), serialPort(new QSerialPort(this)) {}

SerialPortHandler::~SerialPortHandler()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
}

bool SerialPortHandler::openPort(const QString &portName, int baudRate)
{
    serialPort->setPortName("COM3"); // 设置串口名称
    serialPort->setBaudRate(QSerialPort::Baud9600); // 设置波特率为9600
    serialPort->setDataBits(QSerialPort::Data8); // 设置数据位为8
    serialPort->setParity(QSerialPort::NoParity); // 设置无校验位
    serialPort->setStopBits(QSerialPort::OneStop); // 设置停止位为1
    serialPort->setFlowControl(QSerialPort::NoFlowControl); // 设置无流控

    if (serialPort->open(QIODevice::WriteOnly)) {
        qDebug() << "串口已打开：" << portName;
        return true;
    } else {
        qDebug() << "无法打开串口:" << serialPort->errorString();
/*
        HMODULE hDll = LoadLibrary("inpout32.dll");
        if (hDll == NULL) {
            qDebug() << "无法加载 inpout32.dll";
            return -1;
        }

        // 获取 Out32 函数的地址
        typedef void (*Out32Func)(unsigned short portAddress, unsigned char data);
        Out32Func Out32 = (Out32Func)GetProcAddress(hDll, "Out32");
        if (Out32 == NULL) {
            qDebug() << "无法找到 Out32 函数";
            return -1;
        }

        // 使用 Out32 函数向并口写数据
        Out32(0x378, 0xFF);  // 假设并口地址是0x378

        typedef unsigned char (*Inp32Func)(unsigned short portAddress);
        Inp32Func Inp32 = (Inp32Func)GetProcAddress(hDll, "Inp32");
        if (Inp32 != NULL) {
            unsigned char readData = Inp32(0x378); // 假设地址为0x378
            qDebug() << "Read data:" << readData;
        }
*/
        return false;
    }
}

void SerialPortHandler::closePort()
{
    if (serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "串口已关闭";
    }
}

void SerialPortHandler::sendData(const QString &data)
{
    if (serialPort->isOpen()) {
        QByteArray byteArray;

        // 将十六进制字符串转换为字节数组
        bool ok;
        for (int i = 0; i < data.length(); i += 2) {
            QString byteString = data.mid(i, 2);  // 取每两个字符
            byteArray.append(static_cast<char>(byteString.toUInt(&ok, 16)));  // 转换为16进制字节

            if (!ok) {
                qDebug() << "无效的十六进制数据：" << byteString;
                return;
            }
        }

        // 发送数据
        serialPort->write(byteArray);
        qDebug() << "发送数据：" << byteArray.toHex();  // 输出十六进制格式的发送数据
    } else {
        qDebug() << "串口未打开，无法发送数据";
    }
}


//******************************************USB相关
USBHandler::USBHandler(QObject *parent) : QObject(parent), context(nullptr) {
    if (libusb_init(&context) < 0) {
        qWarning() << "Failed to initialize libusb";
    }
}

USBHandler::~USBHandler() {
    if (context) {
        libusb_exit(context);
    }
}

bool USBHandler::sendData(int vendorId, int productId, const QByteArray &data) {
    libusb_device_handle *dev_handle = libusb_open_device_with_vid_pid(context, vendorId, productId);
    if (!dev_handle) {
        qWarning() << "Failed to open USB device";
        return false;
    }

    int transferred = 0;
    int endpoint = 0x01; // 假设发送端点为 0x01
    int result = libusb_bulk_transfer(dev_handle, endpoint,
                                      (unsigned char *)data.data(),
                                      data.size(), &transferred, 1000);
    libusb_close(dev_handle);

    if (result == 0 && transferred == data.size()) {
        qDebug() << "Data sent successfully";
        return true;
    } else {
        qWarning() << "Failed to send data";
        return false;
    }
}
//***********************************************蓝牙相关

BluetoothHandler::BluetoothHandler(QObject *parent) : QObject(parent)
{
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
}

BluetoothHandler::~BluetoothHandler()
{
    delete socket;
}

void BluetoothHandler::connectToDevice(const QString &deviceAddress)
{
    QBluetoothAddress address(deviceAddress);
    socket->connectToService(address, QBluetoothUuid(QBluetoothUuid::SerialPort));

    if (socket->state() == QBluetoothSocket::ConnectedState) {
        qDebug() << "Connected to" << deviceAddress;
    } else {
        qWarning() << "Failed to connect to" << deviceAddress;
    }
}

void BluetoothHandler::sendData(const QByteArray &data)
{
    if (socket->state() == QBluetoothSocket::ConnectedState) {
        socket->write(data);
        qDebug() << "Data sent:" << data;
    } else {
        qWarning() << "Not connected to any Bluetooth device.";
    }
}

void BluetoothHandler::disconnectFromDevice()
{
    if (socket->state() == QBluetoothSocket::ConnectedState) {
        socket->disconnectFromService();
        qDebug() << "Disconnected from device.";
    } else {
        qWarning() << "Not connected to any Bluetooth device.";
    }
}
//***********************************************并口相关

InpOutHandler::InpOutHandler(QObject *parent)
    : QObject(parent),
    hDll(nullptr),
    Out32(nullptr),
    Inp32(nullptr),
    isPortOpened(false)
{
}

InpOutHandler::~InpOutHandler()
{
    if (isPortOpened) {
        closePort(); // 在销毁对象时关闭并口
    }
}

void InpOutHandler::openPort()
{
    if (isPortOpened) {
        return; // 如果端口已经打开，则不再重复打开
    }

    // 尝试加载 inpout32.dll
    hDll = LoadLibrary("inpout32.dll");
    if (hDll == nullptr) {
        qDebug() << "无法加载 inpout32.dll";
        return;
    }

    // 获取 Out32 和 Inp32 函数地址
    Out32 = (Out32Func)GetProcAddress(hDll, "Out32");
    Inp32 = (Inp32Func)GetProcAddress(hDll, "Inp32");

    if (Out32 == nullptr || Inp32 == nullptr) {
        qDebug() << "无法找到 Out32 或 Inp32 函数";
        return;
    }

    isPortOpened = true;
    qDebug() << "并口已成功打开";
}

void InpOutHandler::closePort()
{
    if (isPortOpened) {
        FreeLibrary(hDll);  // 释放 DLL
        hDll = nullptr;
        Out32 = nullptr;
        Inp32 = nullptr;
        isPortOpened = false;
        qDebug() << "并口已关闭";
    }
}

void InpOutHandler::sendData(const QString &portAddress, const QString &data)
{
    // 首先确保并口已打开
    openPort();

    // 将 QML 字符串转换为整数
    bool ok;
    unsigned short port = portAddress.toUInt(&ok, 16);  // 假设传入的是十六进制字符串
    unsigned char byteData = data.toUInt(&ok, 16);  // 假设传入的是单个字节的十六进制数据

    if (ok && Out32 != nullptr) {
        // 调用 Out32 函数，发送数据
        Out32(port, byteData);
        qDebug() << "Data sent to port:" << port << "Data:" << byteData;

        // 读取数据并输出
        if (Inp32 != nullptr) {
            unsigned char readData = Inp32(port);
            qDebug() << "Read data:" << readData;
        }
    } else {
        qDebug() << "Invalid data or function not available";
    }
}



//*****************************************************
int main(int argc, char *argv[])
{
    //引入qml程序
   // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication a(argc, argv);
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

//************************************测试dll加载
    /*
    if (sampleDll.load()) {
        Amplifier amplifier;

        //函数getAmplifierState
        uint32_t amplifierState = amplifier.getAmplifierState();

        //函数openSampleSocket
        bool result = amplifier.openSampleSocket(aMPLIFIER_INFO, channelInfos.data(), 4);
        qDebug() << "Amplifier State: " << amplifierState;
        if (result) {
            qDebug() << "打开放大器成功" << result;
            amplifier.startAmplifier(0x02);
        } else {
            qDebug() << "打开放大器失败" << result;
        }


        //函数readBuffer
        while(true){
            int iSamples = 0;
            void* dataPtr = amplifier.readBuffer(&iSamples);
            qDebug() << "Sample count:" << iSamples; // 检查样本数量
                //qDebug() << "Buffer内存地址: " << dataPtr;
            if (iSamples != 0) {
                short* dataArray = static_cast<short*>(dataPtr);
                for (int i = 0; i < iSamples; ++i) {
                    qDebug() << dataArray[i];  // 输出每个样本的值
                }
            }else {
                qDebug() << "No samples read or iSamples is 0.";}
        //延时3000ms输出一次
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    else {
        QString str = sampleDll.errorString();
        qDebug() <<str;
        qDebug() << "dll加载失败";
    }
*/
//************************************


    //注册类到qml
    qmlRegisterType<Amplifier>("Amplifier.module", 1, 0, "Amplifier");
    qmlRegisterType<DataProvider>("DataProvider.module", 1, 0, "DataProvider");
    qmlRegisterType<SerialPortHandler>("SerialPort.module", 1, 0, "SerialPortHandler");
    qmlRegisterType<USBHandler>("Usb.module", 1, 0, "USBHandler");
    qmlRegisterType<BluetoothHandler>("Bluetooth.module", 1, 0, "BluetoothHandler");
     qmlRegisterType<InpOutHandler>("InpOut.module", 1, 0, "InpOutHandler");


    //加载qml文件
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/ab.qml")));
    if (engine.rootObjects().isEmpty())
    {    return -1;// 如果 QML 加载失败，退出程序
    }

    return a.exec();
}
