#include "reuss/CollectingStreamer.h"
#include "reuss/ZmqSocket.h"
#include "reuss/utils.h"
#include "reuss/ImageData.h"
#include <string>
namespace reuss {

CollectingStreamer::CollectingStreamer(const std::string &endpoint,
                                       std::vector<ImageFifo *> fifos)
    : fifos_(fifos), socket_(endpoint, 10) {}

void CollectingStreamer::stream(int cpu) {
    pin_this_thread(cpu);

    //TODO! find a good place for this
    ImageData<float> image_with_gap{{514, 1030}, 0.0};
    while (!stop_requested_) {
        ImageView img;
        for (auto &fifo : fifos_) {
            if (fifo->try_pop_image(img)) {
                

                //TODO! remove this hardcoded bit. Maybe move to a separate zmq process?
                //copy to the other image with gap pixels
                DataSpan<float, 2> image(reinterpret_cast<float*>(img.data), std::array<ssize_t,2>{512,1024});
                auto ptr = image.data();
                // size_t inc=0;
                for(size_t row=0; row<514; row++){
                    for(size_t col=0; col<1030; col++){
                        if(col == 256 || col == 257 || col == 514 || col == 515 || col == 772 || col == 773 || row == 256 || row == 257) continue;
                        image_with_gap(row, col) = *ptr++;
                        // inc++;
                    }
                }
                //Copy the gap pixels 
                size_t row = 255;
                for (size_t col=0; col<1030; col++){
                    image_with_gap(row, col) /= 2; //pixels are twice as big
                    image_with_gap(row+1, col) = image_with_gap(row, col);
                }
                row = 258;
                for (size_t col=0; col<1030; col++){
                    image_with_gap(row, col) /= 2; //pixels are twice as big
                    image_with_gap(row-1, col) = image_with_gap(row, col);
                }

                // size_t col = 255;
                for (size_t col = 255; col<1000; col+=258){
                    for (size_t row = 0; row<514; row++){
                        //255
                        image_with_gap(row, col) /= 2; //pixels are twice as big
                        image_with_gap(row, col+1) = image_with_gap(row, col);

                        //258
                        image_with_gap(row, col+3) /= 2; //pixels are twice as big
                        image_with_gap(row, col+2) = image_with_gap(row, col);

                    }
                }
                




                // socket_.send(img, fifo->image_size());
                ImageView image_with_gap_view{img.frameNumber, reinterpret_cast<char*>(image_with_gap.data())};
                socket_.send(image_with_gap_view, 514*1030*4);

                fifo->push_free(img);
                last_frame_ = img.frameNumber;
                total_frames_++;
            } else {
                std::this_thread::sleep_for(DEFAULT_WAIT);
            }
        }
    }
    stopped_ = true;
    fmt::print(fg(fmt::color::hot_pink), "CollectingStreamer::stream done!\n");
}
} // namespace reuss