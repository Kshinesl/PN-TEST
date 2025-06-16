#ifndef AMPLIFIERCLIENT_H
#define AMPLIFIERCLIENT_H

#include <QObject>
#include <QTcpSocket>

class AmplifierClient : public QObject
{
    Q_OBJECT
public:
    explicit AmplifierClient(QObject *parent = nullptr);
    Q_INVOKABLE void connectToServer(const QString &ip, int port);
    Q_INVOKABLE void sendStartCommand(int type = 2); // 0x004
    Q_INVOKABLE void sendGetState();

signals:
    void connected();
    void disconnected();
    void responseReceived(int command, int code, QByteArray payload);
    Q_SIGNAL void responseReceivedHex(int commandCode, int code, QString payloadHex);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpSocket *socket;
    QByteArray buffer;
    void sendCommand(int cmd, const QByteArray &payload);
};

#endif // AMPLIFIERCLIENT_H
