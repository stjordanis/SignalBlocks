// (c) 2016 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
// see LICENSE for license

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "../common/Port.h"

using namespace sigblocks;

TEST_CASE("A very basic test cases for ports", "[basic]") {
    std::shared_ptr<IPort<float> > port1(new Port<2, 2, float>());
    std::shared_ptr<IPort<float> > port2(new Port<2, 2, float>());
    port1->SetSink(port2, 0);
    port2->SetSource(port1.get(), 1);
    REQUIRE(port1);
    REQUIRE(port2);
}