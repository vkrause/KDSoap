/****************************************************************************
**
** This file is part of the KD Soap project.
**
** SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/
#ifndef KDSOAPJOB_H
#define KDSOAPJOB_H

#include "KDSoapGlobal.h"

#include <QtCore/QObject>

class KDSoapMessage;
class KDSoapHeaders;

/**
 * \brief KDSoapJob provides a job-based interface to handle asynchronous KD Soap calls.
 *
 * For each SOAP operation, kdwsdl2cpp generates a corresponding KDSoapJob subclass.
 * This makes it easier to manage multiple callers in the client application code calling the same functions, preventing
 * callers processing results for calls made from another caller instead of their own calls.
 *
 * For example, a call to getMothersDay (from the holidays example) using the conventional asynchronous API would look like this:
 * \code
 *  connect(client, &ServiceClass::getMothersDayDone, this, &MyClass::getMothersDayDone);
 *  connect(client, &ServiceClass::getMothersDayError, this, &MyClass::getMothersDayError);
 *  TNS__GetMothersDay params;
 *  params.setYear(2012);
 *  client->asyncGetMothersDay(params);
 * \endcode
 *
 * In this example, all responses to getMothersDay made \c client will be received by the slots connected above.
 * Using the job-based API, it becomes:
 *
 * \code
 *  GetMothersDayJob* job = new GetMothersDayJob(client);
 *  connect(job, &GetMothersDayJob::finished, this, &MyClass::getMothersDayDone);
 *  TNS__GetMothersDay params;
 *  params.setYear(2012);
 *  job->setParameters(params);
 *  job->start();
 * \endcode
 *
 * Now only the result of this specific getMothersDay call will be received by the slot.
 *
 * \since 1.2
 */
class KDSOAP_EXPORT KDSoapJob : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructs a KD Soap job.
     *
     * \param parent optional parent object
     */
    explicit KDSoapJob(QObject *parent = nullptr);

    /**
     * Destructor.
     */
    ~KDSoapJob();

    /**
     * Returns the reply headers received from the SOAP server once the request was completed.
     * Only valid once the request is completed and finished() was emitted.
     */
    KDSoapHeaders requestHeaders() const;

    /**
     * Sets request headers to be sent to the SOAP server. These are sent in addition
     * to the persistent headers set via the client interface.
     *
     * \since 1.8
     */
    void setRequestHeaders(const KDSoapHeaders &headers);

    /**
     * Returns whether the reply message (see reply()) represents a fault.
     */
    bool isFault() const;

    /**
     * A human-readable error string describing the fault if the reply message is a fault, an empty string otherwise.
     */
    QString faultAsString() const;

    /**
     * Returns the reply message received from the SOAP server once the request was completed.
     * Only valid once the request is completed and finished() was emitted.
     */
    KDSoapMessage reply() const;

    /**
     * Returns the reply headers received from the SOAP server once the request was completed.
     * Only valid once the request is completed and finished() was emitted.
     *
     * \since 1.8
     */
    KDSoapHeaders replyHeaders() const;

    /**
     * Starts the job. The job will emit finished() once done.
     */
    void start();

    /**
     * Defines whether the job should be automatically deleted or not.
     * \since 1.8
     */
    void setAutoDelete(bool enable);

Q_SIGNALS:
    /**
     * emitted when the job is completed, i.e. the reply for the job's request
     * was received. To read the result, call reply() in the connected slot.
     * Do not delete the job, the job will auto-delete itself. This behavior
     * can be changed with setAutoDelete().
     *
     * \param job The job instance that emitted the signal
     */
    void finished(KDSoapJob *job);

protected:
    /**
     * \internal
     * Reimplemented in kdwsdl2cpp-generated classes to start request asynchronously.
     */
    Q_INVOKABLE virtual void doStart() = 0;

    /**
     * \internal
     * Sets reply, emits finished() signal and manages deletion of job
     */
    void emitFinished(const KDSoapMessage &reply, const KDSoapHeaders &replyHeaders);

private:
    class Private;
    Private *const d;
};

#endif // KDSOAPJOB_H
