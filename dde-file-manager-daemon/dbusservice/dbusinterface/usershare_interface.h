/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c UserShareInterface -p dbusinterface/usershare_interface usershare.xml
 *
 * qdbusxml2cpp is Copyright (C) 2016 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef USERSHARE_INTERFACE_H
#define USERSHARE_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.deepin.filemanager.daemon.UserShareManager
 */
class UserShareInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.deepin.filemanager.daemon.UserShareManager"; }

public:
    UserShareInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~UserShareInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<bool> addGroup(const QString &groupName)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(groupName);
        return asyncCallWithArgumentList(QStringLiteral("addGroup"), argumentList);
    }

    inline QDBusPendingReply<bool> setUserSharePassword(const QString &username, const QString &passward)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(username) << QVariant::fromValue(passward);
        return asyncCallWithArgumentList(QStringLiteral("setUserSharePassword"), argumentList);
    }

    inline QDBusPendingReply<bool> closeSmbShareByShareName(const QString &sharename)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(sharename);
        return asyncCallWithArgumentList(QStringLiteral("closeSmbShareByShareName"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace com {
  namespace deepin {
    namespace filemanager {
      namespace daemon {
        typedef ::UserShareInterface UserShareManager;
      }
    }
  }
}
#endif
