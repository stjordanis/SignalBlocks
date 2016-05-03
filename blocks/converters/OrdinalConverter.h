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

#ifndef SIGBLOCKS_ORDINALCONVERTER_H
#define SIGBLOCKS_ORDINALCONVERTER_H

#include "../../common/MixedPort.h"

#include <tuple>
#include <unordered_map>

namespace sigblocks {
    class OrdinalConverter
            : public MixedPort<1, 1, std::string, unsigned long> {

    public:
        OrdinalConverter(std::string name);

        OrdinalConverter(std::string name,
                         std::unordered_map<std::string, unsigned long> initDict,
                         unsigned long nextOrdinalValue);

        std::tuple<unsigned long, bool> GetOrdinalValue(const std::string& key) const;
        unsigned long GetNextOrdinalValue() const;

        std::unordered_map<std::string, unsigned long> ExtractDictionary();

    protected: // Override MixedPort interfaces for Sink
        virtual void Process(int sourceIndex, const std::string& data, const TimeTick& startTime);

        virtual void Process(
                int sourceIndex, std::unique_ptr<std::string[]> data, int len, const TimeTick& startTime);

        virtual void ProcessMatrix(int sourceIndex,
                                   std::unique_ptr<std::string[]> data,
                                   const std::vector<int>& dims,
                                   const TimeTick& startTime);

    private:
        std::unordered_map<std::string, unsigned long> mDictionary;
        unsigned long mNextOrdinalValue;
    };
}


#endif //SIGBLOCKS_ORDINALCONVERTER_H
