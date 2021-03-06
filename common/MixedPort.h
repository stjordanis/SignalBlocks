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

#ifndef SIGNALBLOCKS_MIXEDPORT_H
#define SIGNALBLOCKS_MIXEDPORT_H

#include "BlockTypes.h"
#include "BlockManager.h"
#include "IPort.h"
#include "TimeTick.h"
#include "Port.h"

namespace signalblocks {

    /// Note that we are using Port<1, M, T>, but actually there is no
    /// source connected to TransparentProxy
    template<int M, class T>
    class TransparentProxy
            : public Port<1, M, T> {

    public:
        TransparentProxy(std::string name)
                : Port<1, M, T>(std::move(name), "A Transparent Proxy") {
        }

    protected: // override Port interface to act as a transparent proxy
        virtual void Process(int sourceIndex, const T& data, const TimeTick& startTime) {
            for (int i = 0; i < M; ++i) {
                this->LeakData(i, data, startTime);
            }
        }

        virtual void Process(
                int sourceIndex, std::unique_ptr<T[]> data, int len, const TimeTick& startTime) {
            for (int i = 0; i < M; ++i) {
                this->LeakData(i, std::move(data), len, startTime);
            }
        }

        virtual void ProcessMatrix(
                int sourceIndex, std::unique_ptr<T[]> data, const std::vector<int>& dims, const TimeTick& startTime) {
            for (int i = 0; i < M; ++i) {
                this->LeakMatrix(i, std::move(data), dims, startTime);
            }
        }
    };

    // FIXME specialze a case when N=1, M=1 and optimize it
    template<int N, int M, class TN, class TM>
    class MixedPort
            : public IPort<TN> {
    public:
        MixedPort(std::string name, std::string description)
                : mBlockInfo(std::move(name), std::move(description), TypeToBt<TN>::ToType(), N, M),
                  mpSource(),
                  mInternalSource(new TransparentProxy<M, TM>(mBlockInfo.mName)) {
            BlockManager::BlockInfoFuncType cb = std::bind(&MixedPort<N, M, TN, TM>::GetBlockInfo,
                                                           this);
            mBlockInfo.mId = BlockManager::Getinstance()->Add(std::move(cb));

            for (int i = 0; i < N; ++i) {
                mpSource[i] = nullptr;
            }
        }

        virtual ~MixedPort() {
        }

        std::shared_ptr<IPort<TM> >& GetAsSinkType() {
            return mInternalSource;
        }

    public: // IPort interface
        void SetSource(IPort<TN>* peer, int index) {
            assert(index < N); // XXX replace assert with better error handling
            mpSource[index] = peer;
        }

        virtual bool IsSourceConnected(int index) const {
            return mpSource[index] != nullptr;
        }

        virtual bool IsSinkConnected(int index) const {
            return false;  // direct sink is not valid, look for GetAsSinkType()->IsSinkConnected()
        }

        void DisconnectSource(IPort<TN>* peer) {
            for (int i = 0; i < N; ++i) {
                if (mpSource[i] == peer) {
                    mpSource[i] = 0;
                }
            }
        }

        /// use this interface when T is a primitive data type, ex. int, float, etc
        void ConsumeScalar(
                const IPort<TN>* pSender, const TN& data, const TimeTick& startTime) {
            for (int i = 0; i < N; ++i) {
                if (mpSource[i] == pSender) {
                    Process(i, data, startTime);
                    break;
                }
            }
        }

        void ConsumeVector(
                const IPort<TN>* pSender, std::unique_ptr<TN[]> data, int len, const TimeTick& startTime) {
            for (int i = 0; i < N; ++i) {
                if (mpSource[i] == pSender) {
                    Process(i, std::move(data), len, startTime);
                    break;
                }
            }
        }

        void ConsumeMatrix(const IPort<TN>* pSender,
                           std::unique_ptr<TN[]> data,
                           const std::vector<int>& dims,
                           const TimeTick& startTime) {
            for (int i = 0; i < N; ++i) {
                if (mpSource[i] == pSender) {
                    ProcessMatrix(i, std::move(data), dims, startTime);
                    break;
                }
            }
        }

        void ClockCycle(const TimeTick& timeTick) {
            // default implementation is to pass clock cycle
            mInternalSource->ClockCycle(timeTick);
        }

        const BlockManager::BlockInfo& GetBlockInfo() {
            return mBlockInfo;
        }

    private: // IPort interface (which should not be used because sink are of type TM)
        // NOT TO BE USED
        void SetSink(std::shared_ptr<IPort<TN> >& peer, int index) {
        }

    protected: // override these methods in derived class to do something useful
        /// use this interface when T is a primitive data type, ex. int, float, etc
        virtual void Process(int sourceIndex, const TN& data, const TimeTick& startTime) {
            // silently drop here, while derived class should do something
            // useful
        }

        virtual void Process(
                int sourceIndex, std::unique_ptr<TN[]> data, int len, const TimeTick& startTime) {
            // silently drop here, while derived class should do something
            // useful
        }

        virtual void ProcessMatrix(int sourceIndex,
                                   std::unique_ptr<TN[]> data,
                                   const std::vector<int>& dims,
                                   const TimeTick& startTime) {
            // silently drop here, while derived class should do something
            // useful
        }

    private:
        BlockManager::BlockInfo mBlockInfo;
        IPort<TN>* mpSource[N];
        std::shared_ptr<IPort<TM> > mInternalSource;
    };
}

#endif // SIGNALBLOCKS_MIXEDPORT_H
