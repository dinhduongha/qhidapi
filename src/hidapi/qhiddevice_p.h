#ifndef QHIDAPI_P_H
#define QHIDAPI_P_H
/*
Copyright (C) [year] by Simon Meaden <[simonmeaden@virginmedia.com]>

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <QObject>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QIODevice>

#include "qhiddeviceinfo.h"
#include "hidapi.h"

class QHidDevice;

class QHidDevicePrivate {
public:
    QHidDevicePrivate(ushort vendorId, ushort productId, QHidDevice *parent);
    ~QHidDevicePrivate();

    static QList<QHidDeviceInfo> enumerate(ushort vendorId=0x0, ushort productId=0x0);

    bool open(ushort vendor_id, ushort product_id, QString serial_number=QString());
    bool open(QString path);
    bool open();

    void close();

    qint64 read(char* data, qint64 length);
    qint64 read(char* data, qint64 maxSize, int milliseconds);
    int write(uint8_t* data, quint64 maxlen);


    bool setBlocking();
    bool setNonBlocking();
    QByteArray featureReport(uint reportId);
    int sendFeatureReport(quint8 reportId, QByteArray data);
    QString manufacturerString();
    QString productString();
    QString serialNumberString();
    QString indexedString(int index);
    QString error();

    static const int MAX_STR = 255;

    quint32 mVendorId, mProductId;
    QString mSerialNumber;

    QList<QHidDeviceInfo> mDeviceInfoList;
protected:
    QByteArray read();
    QByteArray read(int timeout);
    int write(QByteArray data, quint8 reportNumber);
    int write(QByteArray data);

private:
    QHidDevice *q_ptr;
    hid_device *m_device;
    Q_DECLARE_PUBLIC(QHidDevice)

};

#endif // QHIDAPI_P_H
