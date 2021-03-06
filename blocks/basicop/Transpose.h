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

#ifndef SIGNALBLOCKS_TRANSPOSE_H
#define SIGNALBLOCKS_TRANSPOSE_H


#include "../../common/PortType.h"
#include "../../common/Port.h"

#include <algorithm>
#include <assert.h>
#include <memory>
#include <vector>

namespace signalblocks {

    /** Transpose of a matrix is a new matrix whose rows and column are interchanged.
     * This block takes transpose of the input only if it is a matrix, otherwise
     * the input is transmitted unmodified (that is scalar and vector are not touched).
     */
    template<class T>
    class Transpose
            : public Port<1, 1, T> {
    public:
        Transpose(std::string name)
                : Port<1, 1, T>(std::move(name), "A 2D matrix transpose block.") {
        }
    protected: // Port interface
        virtual void Process(int sourceIndex, const T& data, const TimeTick& startTime) {
            this->LeakData(sourceIndex, data, startTime);
        }

        virtual void Process(
                int sourceIndex, std::unique_ptr<T[]> data, int len, const TimeTick& startTime) {
            this->LeakData(sourceIndex, std::move(data), len, startTime);
        }

        virtual void ProcessMatrix(
                int sourceIndex, std::unique_ptr<T[]> data, const std::vector<int>& dims, const TimeTick& startTime) {
            assert(sourceIndex == 0);
            assert(!dims.empty());
            assert(dims.size() <= 2); // transpose not allowed beyond 2D matrix
            int len = 1;
            for (auto v : dims) {
                len *= v;
            }
            assert(len > 0);
            if (dims.size() == 1) {
                this->LeakMatrix(0, std::move(data), dims, startTime);
            } else {
                // dims.size() == 2
                assert(dims.size() == 2);
                // The simplest logic is using an auxiliary storage, but then doing in-place
                // will save memory allocation/de-allocation. This is not necessarily an issue
                // as long as the matrix consumes small amount of memory.
                std::unique_ptr<T[]> tmp_data(new T[len]);
                for (int i = 0; i < dims[0]; ++i) {
                    for (int j = 0; j < dims[1]; ++j) {
                        tmp_data[j * dims[0] + i] = data[i * dims[1] + j];
                    }
                }
                this->LeakMatrix(0, std::move(tmp_data), dims, startTime);
            }
        }
    };
}

#endif //SIGNALBLOCKS_TRANSPOSE_H
