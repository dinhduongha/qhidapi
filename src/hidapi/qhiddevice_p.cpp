#include "qhiddevice_p.h"
#include "qhiddevice.h"
/*
Copyright (C) [year] by Simon Meaden <[simonmeaden@virginmedia.com]>

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
static bool is_init = false;
QHidDevicePrivate::QHidDevicePrivate(ushort vendorId, ushort productId, QHidDevice *parent) :
    mVendorId(vendorId),
    mProductId(productId),
    q_ptr(parent)
{
    mDeviceInfoList = enumerate(vendorId, productId);
    m_device = nullptr;
}

QHidDevicePrivate::~QHidDevicePrivate()
{
    if (m_device != nullptr) {
        hid_close(m_device);
    }
}

/*!
 * \brief Get The Manufacturer String from a HID device.
 *
 * \return a QString containing the manufacturers name string, otherwise an empty QString.
 */
QString QHidDevicePrivate::manufacturerString()
{
    if (m_device != nullptr) {
        return QString();
    }

    wchar_t buf[MAX_STR];

    int rep = hid_get_manufacturer_string(m_device, buf, MAX_STR);

    if (rep != -1) {

        QString result = QString::fromWCharArray(buf);

        return result;
    }

    return QString();
}

/*!
 * \brief Get The Product String from a HID device.
 *
 * \return a QString containing the product name string, otherwise an empty QString.
 */
QString QHidDevicePrivate::productString()
{
    wchar_t buf[MAX_STR];

    if (m_device == nullptr) {
        return QString();
    }

    int rep = hid_get_product_string(m_device, buf, MAX_STR);

    if (rep != -1) {

        QString result = QString::fromWCharArray(buf);

        return result;
    }

    return QString();
}

/*!
 * \brief Get The Serial Number String from a HID device.
 *
 * \return a QString containing the Serial number string, otherwise an empty QString.
 */
QString QHidDevicePrivate::serialNumberString()
{
    wchar_t buf[MAX_STR];

    if (m_device == nullptr) {
        return QString();
    }

    int rep = hid_get_serial_number_string(m_device, buf, MAX_STR);

    if (rep != -1) {

        QString result = QString::fromWCharArray(buf);

        return result;
    }

    return QString();
}

/*!
 * \brief Get a string from a HID device, based on its string index.
 *
 * \param index The int index of the string to get.
 *
 * \return a QString containing the indexed string, otherwise an empty QString.
 */
QString QHidDevicePrivate::indexedString(int index)
{
    wchar_t buf[MAX_STR];

    if (m_device == nullptr) {
        return QString();
    }

    int rep = hid_get_indexed_string(m_device, index, buf, MAX_STR);

    if (rep != -1) {

        QString result = QString::fromWCharArray(buf);

        return result;
    }

    return QString();
}


/*!
 * \brief Enumerates the HID Devices.
 *
 * This function returns a list of all the HID devices attached to the system which match
 * vendorId and productId. If vendorId is set to 0 then any vendor matches. If productId
 * is set to 0 then any product matches. If vendorId and productId are both left out, then
 * all HID devices will be returned.
 *
 * \code
 *     enumerate();
 * \endcode
 * will return all devices.
 * \code
 *     enumerate(0xafaf);
 * \endcode
 * will return all devices by the selected manufacturer id.
 *\code
 *     enumerate(0xafaf, 0x0735);
 * \endcode
 * will return the all devices of the specified manufacturer and product id's.
 *
 * \param vendorId - an optional unsigned int vendor id
 * \param productId - an optional unsigned int product id.
 * \return a QList<HidDeviceInfo> containing all relevant devices, or an empty list if no devices match.
 */
QList<QHidDeviceInfo> QHidDevicePrivate::enumerate(ushort vendorId, ushort productId)
{
    QList<QHidDeviceInfo> deviceInfoList;
    hid_device_info *info = hid_enumerate(vendorId, productId);

    while (info != nullptr) {
        QHidDeviceInfo i;
        i.path = QString(info->path);
        i.vendorId = info->vendor_id;
        i.manufacturerString = QString::fromWCharArray(info->manufacturer_string);
        i.productId = info->product_id;
        i.productString = QString::fromWCharArray(info->product_string);
        i.releaseNumber = info->release_number;
        i.serialNumber = QString::fromWCharArray(info->serial_number);
#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
        i.usagePage = info->usage_page;
        i.usage = info->usage;
#endif
        i.interfaceNumber = info->interface_number;
        deviceInfoList.append(i);
        info = info->next;
    }

    hid_free_enumeration(info);

    return deviceInfoList;
}

/*!
 * \brief Open a HID device using a Vendor ID (VID), Product ID (PID) and optionally a serial number.
 *
 * Opens a HID device with the supplied VID, PID and optionally serial number string. If serial_number
 * is NULL, the first device with the specified VID and PID is opened. The method returns an id number
 * which should be retained as it is used to identify the device that you wish to access.
 *
 * \param vendorId The Vendor ID (VID) of the device to open.
 * \param productId The Product ID (PID) of the device to open.
 * \param serialNumber The Serial Number of the device to open (Optionally NULL).
 * \return returns an quint32 id number, or 0 if unsuccessful.
 */
//bool QHidDevicePrivate::open(ushort vendorId, ushort productId, QString serialNumber)
//{
//    mVendorId = vendorId;
//    mProductId = productId;
//    mSerialNumber = serialNumber;
//    return openNewProduct(vendorId, productId, serialNumber);
//}

/*!
 * \brief Closes the specified device if it exists, otherwise this command is ignored.
 */
void QHidDevicePrivate::close()
{
    if (m_device != nullptr) {
        hid_close(m_device);
    }
}

//hid_device *QHidDevicePrivate::findId(quint32 id) {
//    if (mIdDeviceMap.contains(id)) {
//        hid_device* dev = mIdDeviceMap.value(id);
//        return dev;
//    }
//    return NULL;
//}

/*!
 * \brief  Read an Input report from a HID device into a QByteArray.
 *
 * Input reports are returned to the host through the INTERRUPT IN endpoint. The first byte will contain the Report number if the device uses numbered reports.
 * It should be noted that the write function returns data as unsigned char values but a QByteArray stores signed char's. To recover the report as raw unsigned
 * characters you should use reinterpret_cast<uchar*>(bytearray.data()).
 *
 * \param id A quint32 device id.
 *
 * \return Returns the data in a QByteArray. If no packet was available to be read and the handle is in non-blocking mode, Returns the data in a QByteArray.
 */
QByteArray QHidDevicePrivate::read()
{

    if (m_device != nullptr) {
        unsigned char buf[65];

        int rep = hid_read(m_device, buf, 65);

        if (rep > 0) {
            QByteArray data(reinterpret_cast<char*>(buf), rep);
            return data;
        }
    }

    return QByteArray();
}

qint64 QHidDevicePrivate::read(char* data, qint64 maxSize)
{

    size_t length = (size_t)maxSize;
    if (m_device != nullptr) {

        int rep = hid_read(m_device, (uint8_t*)data, length);
        return rep;
    }

    return 0;
}

qint64 QHidDevicePrivate::read(char* data, qint64 maxSize, int milliseconds)
{

    size_t length = (size_t)maxSize;
    if (m_device != nullptr) {

        int rep = hid_read_timeout(m_device, (uint8_t*)data, length, milliseconds);
        return rep;
    }

    return 0;
}

/*!
 * \brief  Read an Input report from a HID device into a QByteArray,  with timeout.
 *
 * Input reports are returned to the host through the INTERRUPT IN endpoint.
 * The first byte will contain the Report number if the device uses numbered reports.
 * If no packet was availableto be read within timeout milliseconds and the device
 * is in non-blocking mode.
 *
 * It should be noted that the write function returns data as unsigned char values
 * but a QByteArray stores signed char's. To recover the report as raw unsigned
 * characters you should use reinterpret_cast<uchar*>(bytearray.data()).
 *
 * \param id A quint32 device id.
 * \param timeout timeout in milliseconds or -1 for blocking wait.
 *
 * \return Returns the data in a QByteArray.
 */
QByteArray QHidDevicePrivate::read(int timeout)
{
    if (m_device != nullptr) {
        unsigned char buf[65];

        int rep = hid_read_timeout(m_device, buf, 65, timeout);

        if (rep > 0) {
            QByteArray data(reinterpret_cast<char*>(buf), rep);
            return data;
        }
    }

    return QByteArray();
}

/*!
 * \brief Get a feature report from a HID device.
 *
 * Set the first byte of the returned data will contain the Report ID of the report to be read. Make sure to allow space for this extra byte in data[].
 * Upon return, the first byte will still contain the Report ID, and the report data will start in data[1].
 *
 * \param id A quint32 device id.
 * \param reportId the reportId.
 *
 * \return Returns the data in a QByteArray. Returns an empty QByteArray on error.

 */
QByteArray QHidDevicePrivate::featureReport(uint reportId)
{
    if (m_device != nullptr) {
        unsigned char buf[65];
        buf[0] = reportId;

        int rep = hid_get_feature_report(m_device, buf, sizeof(buf));

        if (rep > 0) {
            QByteArray data(reinterpret_cast<char*>(buf), rep);
            return data;
        }
    }

    return QByteArray();
}

/*!
 * \brief  Write an Feature report to a HID device.
 *
 * HID reports have a maximum length of 64 bytes, plus an the initial char being a report ID. For devices which only support a single report, this must be set to 0x0.
 * The remaining bytes contain the report data.
 *
 * Data will send the data on the first OUT endpoint, if one exists. If it does not, it will send the data through the Control Endpoint (Endpoint 0).
 *
 * \param id A quint32 device id.
 * \param the report id
 * \param The data to send, excluding the report number as the first byte.
 * \return the number of bytes written, or -1 on error.
 */
int QHidDevicePrivate::sendFeatureReport(quint8 reportId, QByteArray data)
{
    if (data.length() > 64) return -1;

    if (m_device != nullptr) {
        data.prepend(reportId);

        int rep = hid_send_feature_report(m_device, reinterpret_cast<uchar*>(data.data()), data.length());

        return rep;
    }

    return -1;
}

/*!
 * \brief  Write an Output report to a HID device.
 *
 * HID reports have a maximum length of 64 bytes, plus an the initial char being a report ID. For devices which only support a single report, this must be set to 0x0.
 * The remaining bytes contain the report data.
 *
 * Data will send the data on the first OUT endpoint, if one exists. If it does not, it will send the data through the Control Endpoint (Endpoint 0).
 *
 * \param the report id
 * \param The data to send, excluding the report number as the first byte.
 * \return the number of bytes written, or -1 on error.
 */
int QHidDevicePrivate::write(uint8_t* data, quint64 maxlen)
{

    if (m_device != nullptr) {

        int rep = hid_write(m_device, data, (size_t)maxlen);

        return rep;
    }

    return -1;
}


/*!
 * \brief  Write an Output report to a HID device.
 *
 * HID reports have a maximum length of 64 bytes, plus an the initial char being a report ID. For devices which only support a single report, this must be set to 0x0.
 * The remaining bytes contain the report data.
 *
 * Data will send the data on the first OUT endpoint, if one exists. If it does not, it will send the data through the Control Endpoint (Endpoint 0).
 *
 * \param the report id
 * \param The data to send, excluding the report number as the first byte.
 * \return the number of bytes written, or -1 on error.
 */
int QHidDevicePrivate::write(QByteArray data, quint8 reportNumber)
{
    if (data.length() > 64) return -1;

    if (m_device != nullptr) {
        data.prepend(reportNumber);

        int rep = hid_write(m_device, reinterpret_cast<uchar*>(data.data()), data.length());

        return rep;
    }

    return -1;
}

/*!
 * \brief  Write an Output report to a HID device.
 *
 * HID reports have a maximum length of 64 bytes, plus an the initial char being a report ID. For devices which only support a single report, this must be set to 0x0.
 * The remaining bytes contain the report data. In this version of write() it is assumed that the initial report character is already prepended to the supplied QByteArray.
 *
 * Data will send the data on the first OUT endpoint, if one exists. If it does not, it will send the data through the Control Endpoint (Endpoint 0).
 *
 * \param The data to send, excluding the report number as the first byte.
 *
 * \return the number of bytes written, or -1 on error.
 */
int QHidDevicePrivate::write(QByteArray data)
{
    if (data.length() > 65) return -1;

    if (m_device != nullptr) {
        int rep = hid_write(m_device, reinterpret_cast<uchar*>(data.data()), data.length());

        return rep;
    }

    return -1;
}

/*!
 * \brief Get a string describing the last error which occurred on the supplied device.
 *
 * \return This function returns a string containing the last error which occurred on the device or an empty QString if none has occurred.
 */
QString QHidDevicePrivate::error()
{
    if (m_device != nullptr) {
        QString r;

        const wchar_t *errorString = hid_error(m_device);
        r = QString::fromWCharArray(errorString);

        if (r == NULL) return QString();

        return r;
    }

    return QString();
}

/*!
 * \brief  Set the device handle to be blocking.
 *
 * In non-blocking mode calls to read() will return immediately with a value of 0 if there is no data to be read. In blocking mode, read() will wait (block)
 * until there is data to read before returning.
 *
 * Blocking can be turned on and off at any time.
 *
 * \return Returns true on success and false on error.
 */
bool QHidDevicePrivate::setBlocking()
{
    if (m_device != nullptr) {
        int rep = hid_set_nonblocking(m_device, 1);
        return !!rep;
    }
    return false;
}

/*!
 * \brief  Set the device handle to be non-blocking.
 *
 * In non-blocking mode calls to read() will return immediately with a value of 0 if there is no data to be read. In blocking mode, read() will wait (block)
 * until there is data to read before returning.
 *
 * Nonblocking can be turned on and off at any time.
 *
 * \param id  A quint32 device id.
 * \return Returns true on success and false on error.
 */
bool QHidDevicePrivate::setNonBlocking()
{
    if (m_device != nullptr) {
        int rep = hid_set_nonblocking(m_device, 0);
        return !!rep;
    }
    return false;
}

/*!
 * \brief Open a HID device by its path name.
 *
 * The path name be determined by calling hid_enumerate(), or a platform-specific path
 * name can be used (eg: /dev/hidraw0 on Linux).
 *
 * \param path - the path to the device
 * \return Returns true on success and false on error.
 */
bool QHidDevicePrivate::open(QString path)
{
    if (m_device != nullptr) {
        hid_close(m_device);
        m_device = nullptr;
    }

    // if not open it.
    hid_device *device = hid_open_path(path.toLocal8Bit().data());

    // sorry doesn't exist
    if (device == nullptr) return false;

    m_device = device;

    return true;
}

/*
 * Opens a new product for the supplied vendor/product/serial number.
 * returns the handle id if successful, otherwise  returns 0.
 */
bool QHidDevicePrivate::open(ushort vendorId, ushort productId, QString serialNumber)
{
    if (m_device != nullptr) {
        hid_close(m_device);
        m_device = nullptr;
    }
    mVendorId = vendorId;
    mProductId = productId;

    hid_device *device = nullptr;
    if (serialNumber.isEmpty()) {
        device = hid_open(vendorId, productId, NULL);
        if (device != nullptr) {
            m_device = device;
            return true;
        }
    } else {
        wchar_t* sn = new wchar_t[serialNumber.length() + 1];
        serialNumber.toWCharArray(sn);
        sn[serialNumber.length()] = 0x0;
        device = hid_open(vendorId, productId, sn/*, Q_NULLPTR*/);
        if (device != Q_NULLPTR) {
            m_device = device;
            return true;
        }
    }
    return false;
}

/*!
 * \brief Open a HID device.
 *
 * \return Returns true on success and false on error.
 */
bool QHidDevicePrivate::open()
{
    if (m_device != nullptr) {
        hid_close(m_device);
        m_device = nullptr;
    }
    // if not open it.
    hid_device *device = nullptr;
    if (mSerialNumber.isEmpty()) {
        device = hid_open(mVendorId, mProductId, NULL);
        if (device != nullptr) {
            m_device = device;
            return true;
        }
    } else {
        wchar_t* sn = new wchar_t[mSerialNumber.length() + 1];
        mSerialNumber.toWCharArray(sn);
        sn[mSerialNumber.length()] = 0x0;
        //device = hid_open(0x4d8, 0xf1fa, NULL);
        device = hid_open(mVendorId, mProductId, sn/*, Q_NULLPTR*/);
        if (device != Q_NULLPTR) {
            m_device = device;
            return true;
        }
    }

    // sorry doesn't exist
    if (device == nullptr) return false;

    m_device = device;

    return true;
}



