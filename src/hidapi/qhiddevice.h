#ifndef QHIDAPI_H
#define QHIDAPI_H

/*
Copyright (C) [year] by Simon Meaden <[simonmeaden@virginmedia.com]>

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <QObject>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QByteArray>
#include <QList>
#include <QIODevice>

#include "qhidapi_global.h"
#include "qhiddeviceinfo.h"

class QHidDevicePrivate;

class QHIDAPISHARED_EXPORT QHidDevice : public QIODevice {

    Q_OBJECT

public:
    QHidDevice(ushort vendorId, QObject *parent=0);
    QHidDevice(ushort vendorId, ushort productId, QObject *parent=0);
    QHidDevice(QObject *parent=0);
    ~QHidDevice();

    static QList<QHidDeviceInfo> enumerate(ushort vendorId=0x0, ushort productId=0x0);

    bool open(ushort vendor_id, ushort product_id, QString serial_number=QString());
    bool open(QString path);
    bool open(OpenMode mode) override;
    void close() override;
    bool isSequential() const override { return true; }

    bool setBlocking();
    bool setNonBlocking();
    QByteArray featureReport(uint reportId);
    int sendFeatureReport(quint8 reportId, QByteArray data);
    QString manufacturerString();
    QString productString();
    QString serialNumberString();
    QString indexedString(int index);
    QString error();

    qint64  read(char* data, qint64 maxSize, int milliseconds);
    //QByteArray read(int milliseconds);

    static int init();
    static int exit();
    //bool waitForReadyRead(int msecs) override;
signals:

public slots:

protected slots:

protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char* data, qint64 maxlen) override;
private:
    QHidDevicePrivate *d_ptr;
    Q_DECLARE_PRIVATE(QHidDevice)
    Q_DISABLE_COPY(QHidDevice)
};

#endif // QHIDAPI_H
