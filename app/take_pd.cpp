

#include "reuss/pedestal.h"
#include "reuss/JungfrauDetector.h"
#include "reuss/ImageData.h"
#include "reuss/DataSpan.h"


int main(){

    using namespace reuss;
    reuss::JungfrauDetector d;

    take_pedestal<float>(&d, 500);
    // take_pedestal(DetectorInterface *det,int n_frames) 
    // void take_pedestal(DataSpan<T, 3> pedestal, DetectorInterface *det,
    //                int n_frames);

    // ImageData<float, 3> pd({3, 512,512}, 0);
    // auto span = make_span(pd);
    // take_pedestal<float>(span, d, 500);
    // auto pd = reuss::take_pedestal<float>(&d, 500);

}