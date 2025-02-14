/****************************************************************************
**
** This file is part of the KD Soap project.
**
** SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/
#include "KDSoapThreadPool.h"
#include "KDSoapServerThread_p.h"
#include <QDebug>

class KDSoapThreadPool::Private
{
public:
    Private()
        : m_maxThreadCount(QThread::idealThreadCount())
    {
    }

    KDSoapServerThread *chooseNextThread();

    int m_maxThreadCount;
    typedef QList<KDSoapServerThread *> ThreadCollection;
    ThreadCollection m_threads;
};

KDSoapThreadPool::KDSoapThreadPool(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

KDSoapThreadPool::~KDSoapThreadPool()
{
    // ask all threads to finish, then delete them all
    for (KDSoapServerThread *thread : qAsConst(d->m_threads)) {
        thread->quitThread();
    }
    for (KDSoapServerThread *thread : qAsConst(d->m_threads)) {
        thread->wait();
        delete thread;
    }

    delete d;
}

void KDSoapThreadPool::setMaxThreadCount(int maxThreadCount)
{
    d->m_maxThreadCount = maxThreadCount;
}

int KDSoapThreadPool::maxThreadCount() const
{
    return d->m_maxThreadCount;
}

KDSoapServerThread *KDSoapThreadPool::Private::chooseNextThread()
{
    KDSoapServerThread *chosenThread = nullptr;
    // Try to pick an existing thread
    int minSocketCount = 0;
    KDSoapServerThread *bestThread = nullptr;
    for (KDSoapServerThread *thr : qAsConst(m_threads)) {
        // We look at the amount of sockets connected to each thread, and pick the less busy one.
        // Note that this isn't fully accurate, due to Keep-Alive: it's possible for long-term
        // idling clients to be all on one thread, and active clients on another one, and this
        // wouldn't use CPU fully. But this is probably still better than no keep-alive (much more
        // connection overhead). Maybe we have to look at sockets who made a request in the last
        // N seconds... but the past is no indication of the future.
        const int sc = thr->socketCount();
        if (sc == 0) { // Perfect, an idling thread
            // qDebug() << "Picked" << thr << "since it was idling";
            chosenThread = thr;
            break;
        }
        if (!bestThread || sc < minSocketCount) {
            minSocketCount = sc;
            bestThread = thr;
        }
    }

    // Use an existing non-idling thread, if we reached maxThreads
    if (!chosenThread && bestThread && m_threads.count() == m_maxThreadCount) {
        chosenThread = bestThread;
    }

    // Create new thread
    if (!chosenThread) {
        chosenThread = new KDSoapServerThread(nullptr);
        // qDebug() << "Creating KDSoapServerThread" << chosenThread;
        m_threads.append(chosenThread);
        chosenThread->startThread();
    }
    return chosenThread;
}

void KDSoapThreadPool::handleIncomingConnection(int socketDescriptor, KDSoapServer *server)
{
    // First, pick or create a thread.
    KDSoapServerThread *chosenThread = d->chooseNextThread();

    // Then create the socket, and register it in the corresponding socket-pool, and move it to the thread.
    chosenThread->handleIncomingConnection(socketDescriptor, server);
}

int KDSoapThreadPool::numConnectedSockets(const KDSoapServer *server) const
{
    int sc = 0;
    for (KDSoapServerThread *thread : qAsConst(d->m_threads)) {
        sc += thread->socketCountForServer(server);
    }
    return sc;
}

void KDSoapThreadPool::disconnectSockets(KDSoapServer *server)
{
    QSemaphore readyThreads;
    for (KDSoapServerThread *thread : qAsConst(d->m_threads)) {
        thread->disconnectSocketsForServer(server, readyThreads);
    }
    // Wait for all threads to have disconnected their sockets
    readyThreads.acquire(d->m_threads.count());
}

int KDSoapThreadPool::totalConnectionCount(const KDSoapServer *server) const
{
    int sc = 0;
    for (KDSoapServerThread *thread : qAsConst(d->m_threads)) {
        sc += thread->totalConnectionCountForServer(server);
    }
    return sc;
}

void KDSoapThreadPool::resetTotalConnectionCount(const KDSoapServer *server)
{
    for (KDSoapServerThread *thread : qAsConst(d->m_threads)) {
        thread->resetTotalConnectionCountForServer(server);
    }
}

#include "moc_KDSoapThreadPool.cpp"
