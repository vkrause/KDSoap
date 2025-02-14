/****************************************************************************
**
** This file is part of the KD Soap project.
**
** SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/
#ifndef KDSOAPPENDINGCALL_H
#define KDSOAPPENDINGCALL_H

#include "KDSoapMessage.h"
#include <QtCore/QExplicitlySharedDataPointer>
QT_BEGIN_NAMESPACE
class QNetworkReply;
class QBuffer;
QT_END_NAMESPACE
class KDSoapPendingCallWatcher;

/**
 * The KDSoapPendingCall class refers to one pending asynchronous call
 *
 * A KDSoapPendingCall object is a reference to a SOAP method call that was sent over
 * without waiting for a reply.
 * KDSoapPendingCall is an opaque type, meant to be used as a handle for a pending reply.
 *
 * The KDSoapPendingCallWatcher class allows one to connect to a signal that will
 * indicate when the reply has arrived or if an error occurred.
 *
 * \note If you create a copy of a KDSoapPendingCall object, all information will
 * be shared among the many copies. Therefore, KDSoapPendingCall is an
 * explicitly-shared object and does not provide a method of detaching the copies
 * (since they refer to the same pending call)
 *
 * \see KDSoapClientInterface::asyncCall()
 */
class KDSOAP_EXPORT KDSoapPendingCall
{
public:
    /**
     * Creates a copy of the \p other pending asynchronous call.
     * Note that both objects will refer to the same pending call.
     */
    KDSoapPendingCall(const KDSoapPendingCall &other);

    /**
     * Destroys this copy of the KDSoapPendingCall object.
     * \warning If this copy is also the last copy of a pending asynchronous call,
     * the call will be canceled and no further notifications will be received.
     */
    ~KDSoapPendingCall();

    /**
     * Creates a copy of the \p other pending asynchronous call and drops
     * the reference to the previously-referenced call. Note that both objects
     * will refer to the same pending call after this function.
     *
     * \warning If this object contained the last reference of a pending asynchronous
     * call, the call will be canceled and no further notifications will be
     * received.
     */
    KDSoapPendingCall &operator=(const KDSoapPendingCall &other);

    /**
     * Returns the response message sent by the server.
     * Could either be a fault (see KDSoapMessage::isFault) or the actual response arguments.
     */
    KDSoapMessage returnMessage() const;

    /**
     * Helper method for the simple case where a single argument is returned:
     * Returns the value of that single argument.
     */
    QVariant returnValue() const;

    /**
     * Returns the response headers sent by the server.
     */
    KDSoapHeaders returnHeaders() const;

    /**
     * Returns \c true if the pending call has finished processing and the reply has been received.
     *
     * \note This function only changes state if an external event happens,
     * which in general only happens if you return to the event loop execution.
     *
     * You generally do not need to use this: use KDSoapPendingCallWatcher to be notified
     * of the call completion.
     */
    bool isFinished() const;

private:
    friend class KDSoapClientInterface;
    friend class KDSoapThreadTask;
    KDSoapPendingCall(QNetworkReply *reply, QBuffer *buffer);

    friend class KDSoapPendingCallWatcher; // for connecting to d->reply

    class Private;
    QExplicitlySharedDataPointer<Private> d;
};

#endif // KDSOAPPENDINGCALL_H
