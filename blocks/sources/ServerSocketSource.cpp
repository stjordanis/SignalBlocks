// (c) 2016 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>.
// This file is part of SignalBlocks.
//
// SignalBlocks is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SignalBlocks is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SignalBlocks.  If not, see <http://www.gnu.org/licenses/>.
//

#include "ServerSocketSource.h"

#include "../../socket/IServerSocket.h"

#include <netinet/in.h>
#include <iostream>
#include <assert.h>
#include <string.h>

using namespace signalblocks;
using namespace std;

#define DESCRIPTION "A source block which gets data from a socket."

namespace {
    const int MAX_BUFFER_SIZE = 4096; // power of 2 (2^12 = 4096)

    // TODO: Have them configurable
    const int TIMEOUT_SEC = 1;
    const int TIMEOUT_USEC = 0;
}

ServerSocketSource::ServerSocketSource(
        std::string name, int blockSize)
        : Port<0, 1, unsigned char>(std::move(name), DESCRIPTION),
          mLastTick(),
          mpSocket(nullptr),
          mBlockSize(blockSize),
          mpBuffer(new uint8_t[MAX_BUFFER_SIZE]),
          mBytesRead(0),
          mBufferSize(0) {
    assert(mBlockSize > 0);
}

ServerSocketSource::ServerSocketSource(
        std::string name,
        std::unique_ptr<SocketProgramming::IServerSocket> pIns,
        int blockSize)
        : Port<0, 1, unsigned char>(std::move(name), DESCRIPTION),
          mLastTick(),
          mpSocket(std::move(pIns)),
          mBlockSize(blockSize),
          mpBuffer(new uint8_t[MAX_BUFFER_SIZE]),
          mBytesRead(0),
          mBufferSize(0) {
    assert(mBlockSize > 0);
}

void
ServerSocketSource::SetStreamSource(
        std::unique_ptr<SocketProgramming::IServerSocket> pIns) {
    mpSocket.swap(pIns);
}

void
ServerSocketSource::ClockCycle(const TimeTick& timeTick) {
    if (!mpSocket->IsValid()) {
        cerr << this << " ServerSocketSource is done processing!" << endl;
        return; // dont do anything
    }
    if (mLastTick == timeTick) {
        return;  // already processed the event
    }
    mLastTick = timeTick;

    struct sockaddr_in from;
    int fromlen = sizeof(struct sockaddr_in);

    if (!mpSocket->IsConnected()) {
        const int nfds = 1;
        fd_set readfd;
        FD_ZERO(&readfd);
        FD_SET(mpSocket->GetFd(), &readfd);
        struct timeval timeout = {TIMEOUT_SEC, TIMEOUT_USEC};
        int numfd_set = select(nfds, &readfd, 0, 0, &timeout);
        if (numfd_set != 1) {
            return; // dont do anything
        }
        // else accept new connection
        mpSocket->Accept((struct sockaddr*) &from, (socklen_t*) &fromlen);
        // and now start reading as done below.
    }

    if ((mBlockSize - (mBufferSize - mBytesRead)) > 0) {
        // get max possible bytes
        int bytes_read =
                mpSocket->Receive(mpBuffer.get() + mBytesRead,
                                  (MAX_BUFFER_SIZE - (mBufferSize - mBytesRead)) * sizeof(uint8_t),
                                  (sockaddr*) &from,
                                  (socklen_t*) &fromlen);
        if (bytes_read > 0) {
            mBufferSize += bytes_read;
            assert(MAX_BUFFER_SIZE >= mBufferSize);
        }
        else if (bytes_read <= 0) {
            return; // could not read, so lets just return
        }
    }
    // This logic will fail to send less than mBlockSize in case of
    // connection oriented socket (eg. TCP) when the connected
    // was lost.
    // TODO: Do we care?
    if ((mBufferSize - mBytesRead) >= mBlockSize) {
        std::unique_ptr<uint8_t[]> data(new uint8_t[mBlockSize]);
        memcpy(data.get(), mpBuffer.get() + mBytesRead, mBlockSize);
        LeakData(0, std::move(data), mBlockSize, timeTick);
        mBytesRead += mBlockSize;
        if (mBytesRead == mBufferSize) {
            mBytesRead = mBufferSize = 0;
        }
    }
}
