/****************************************************************************
**
** This file is part of the KD Soap project.
**
** SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDSOAPFAULTEXCEPTION_H
#define KDSOAPFAULTEXCEPTION_H

#include "KDSoapValue.h"
#include <QString>

/**
 * KDSoapFaultException is the base class for exceptions representing a fault element.
 * So far supporting only SOAP 1.1 version which mean the following attributes :
 * \<faultcode\>, \<faultstring\>, \<faultfactor\>, \<detail\>
 *
 * The \<detail\> tag optionally contains specific serialized fault information.
 * represented as an exception in the client side.
 */
class KDSOAP_EXPORT KDSoapFaultException
{
public:
    /**
     * Reconstructs the KDSoapFaultException object from a KDSoapValue.
     * \param mainValue KDSoapValue represents the fault tag element
     */
    void deserialize(const KDSoapValue &mainValue);

    /**
     * Returns the KDSoapValue representing the \<detail\> tag from the parameter.
     * \param faultValue Represent the \<fault\> tag in the SOAP protocol
     */
    static const KDSoapValue &faultDetails(const KDSoapValue &faultValue);

    /**
     * Returns the fault code.
     */
    QString faultCode() const;

    /**
     * Set the code of the fault.
     * \param faultCode Code of the fault
     */
    void setFaultCode(const QString &faultCode);

    /**
     * Returns the human readable fault string of the fault.
     */
    QString faultString() const;

    /**
     * Set the human-readable QString of the fault.
     * \param faultString Human-readable message
     */
    void setFaultString(const QString &faultString);

    /**
     * Returns the fault actor of the fault as a QString
     */
    QString faultActor() const;

    /**
     * Set the fault actor with a string.
     * \param faultActor information about who caused the fault to happen
     */
    void setFaultActor(const QString &faultActor);

    /**
     * Returns the \<detail\> tag of the fault element as a KDSoapValue.
     * When the fault is generic, return an empty KDSoapValue.
     * When the fault is specific the KDSoapValue optionally contains specific serialized fault information.
     */
    KDSoapValue detailValue() const;

    /**
     * Set the KDSoapValue of the fault.
     * \param detailValue KDSoapValue representing the \<detail\> tag of a fault message
     */
    void setDetailValue(const KDSoapValue &detailValue);

    /**
     * Constructor
     */
    KDSoapFaultException();

    /**
     * Copy constructor
     */
    KDSoapFaultException(const KDSoapFaultException &cpy);

    /**
     * Constructs a KDSoapFaultException according to the SOAP 1.1 attriutes.
     *
     * \param faultCode a code for identifying the fault
     * \param faultString a human readable explanation of the fault
     * \param faultActor information about who caused the fault to happen
     */
    KDSoapFaultException(const QString &faultCode, const QString &faultString, const QString &faultActor = QString());

    /**
     * Assignment operator
     */
    KDSoapFaultException &operator=(const KDSoapFaultException &other);

    /**
     * Destructor
     */
    ~KDSoapFaultException();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif // KDSOAPFAULTEXCEPTION_H
