// (c) 2016 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>.
// see LICENSE for license

#ifndef SIGNALBLOCKS_BLOCKRUNNER_H
#define SIGNALBLOCKS_BLOCKRUNNER_H

#include "IPort.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace sigblocks {
    class BlockRunner {
    public:
        enum BlockType {
            BLOCK_CHAR = 0,
            BLOCK_UCHAR,
            BLOCK_SHORT,
            BLOCK_USHORT,
            BLOCK_INT,
            BLOCK_UINT,
            BLOCK_LONG,
            BLOCK_ULONG,
            BLOCK_FLOAT,
            BLOCK_DOUBLE,
            BLOCK_LONG_DOUBLE
        };

    public:

        BlockRunner(const TimeTick& increment);
        
        bool Add(std::shared_ptr<IPort<char> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<unsigned char> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<short> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<unsigned short> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<int> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<unsigned int> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<long> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<unsigned long> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<float> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<double> > source, const std::string& name);
        bool Add(std::shared_ptr<IPort<long double> > source, const std::string& name);

        bool Remove(const std::string& name);

        // must be called repeatedly
        void Iterate();

    private:

        TimeTick mTimeTick;
        TimeTick mTimeTickIncrement;

        // Global database which archives all the source blocks and
        // their base data type. This is important to maintain uniqueness
        // across all the source blocks.
        std::unordered_map<std::string, BlockType> mBlocks;

        std::unordered_map<std::string, std::shared_ptr<IPort<char> > > mCharSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<unsigned char> > > mUcharSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<short> > > mShortSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<unsigned short> > > mUshortSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<int> > > mIntSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<unsigned int> > > mUintSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<long> > > mLongSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<unsigned long> > > mUlongSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<float> > > mFloatSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<double> > > mDoubleSources;
        std::unordered_map<std::string, std::shared_ptr<IPort<long double> > > mLongDoubleSources;
    };
}

#endif //SIGNALBLOCKS_BLOCKRUNNER_H