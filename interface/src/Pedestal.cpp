#include "reuss/Pedestal.h"
#include "reuss/ZmqReceiver.h"
#include "reuss/project_defs.h"
#include "sls/Detector.h"

#include "fmt/core.h"

namespace reuss{



    void Pedestal::collect(){

        ZmqReceiver r{DEFAULT_RECEIVE};
        sls::Detector d;
        fmt::print("Gain 0\n");
        
        



        
    }
}