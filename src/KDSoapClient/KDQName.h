/*
    This file is part of KDE.

    SPDX-FileCopyrightText: 2005 Tobias Koenig <tokoe@kde.org>
                       based on wsdlpull parser by Vivek Krishna

    SPDX-License-Identifier: MIT
 */

#ifndef KDQNAME_H
#define KDQNAME_H

#include <QHash>
#include <QList>
#include <QMetaType>
#include <QString>

class KDSoapValue;

#include "KDSoapGlobal.h"

class KDSOAP_EXPORT KDQName
{
public:
    typedef QList<KDQName> List;

    KDQName();

    // Create a KDQName with prefix+localname
    explicit KDQName(const QString &name);

    // Create a KDQName with namespace+localname
    KDQName(const QString &nameSpace, const QString &localName);

    void operator=(const QString &name);

    QString localName() const;
    QString prefix() const;
    QString qname() const;

    void setNameSpace(const QString &nameSpace);
    QString nameSpace() const;

    bool operator==(const KDQName &) const;
    bool operator!=(const KDQName &) const;

    bool isEmpty() const;

    /**
     * Creates a KDQName from a KDSoapValue.
     */
    static KDQName fromSoapValue(const KDSoapValue &value);

    /**
     * Returns a KDSoapValue representation.
     */
    KDSoapValue toSoapValue(const QString &name, const QString &typeNameSpace = QString(), const QString &typeName = QString()) const;

private:
    void parse(const QString &);

    QString mNameSpace;
    QString mLocalName;
    QString mPrefix;
};

Q_DECLARE_METATYPE(KDQName)

inline uint qHash(const KDQName &qn)
{
    return qHash(qn.nameSpace()) ^ qHash(qn.localName());
}

QDebug operator<<(QDebug dbg, const KDQName &qn);

#endif
