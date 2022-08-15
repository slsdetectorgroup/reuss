#include "reuss/Writer.h"
#include "reuss/utils.h"
#include "File.h"
#include "DirectWriter.h"
#include "FwriteWriter.h"
#include "RawFile.h"
#include <string>
namespace reuss {

Writer::Writer(ImageFifo *fifo, size_t n_frames, std::string_view pathname)
    : fifo_(fifo), n_frames(n_frames), pathname(pathname) {}

void Writer::write(int cpu) {
    stopped_ = false;
    done_ = false;
    pin_this_thread(cpu);
    fmt::print("Writing to: {}\n", pathname);
    RawFile<FwriteWriter> f(pathname, G2_FRAMES_PER_FILE);
    while (!stopped_ && (total_frames_ < n_frames)) {
        ImageView img;
        if (fifo_->try_pop_image(img)) {
            // Write to file, take care with alignment if switching to 
            // DirectWriter
            const size_t nbytes = img.framesInPack*G2_PACKET_SIZE;
            f.write(img.framesInPack, img.data, nbytes);
            last_frame_ = img.frameNumber;
            total_frames_ += img.framesInPack;
            fifo_->push_free(img);
            
        }
        std::this_thread::sleep_for(DEFAULT_WAIT);
    }
    fmt::print("Writer done, total: {}\n", total_frames_);
    done_ = true;
}

void Writer::sink(int cpu){
    stopped_ = false;
    done_ = false;
    pin_this_thread(cpu);
    while (!stopped_ && (total_frames_ < n_frames)) {
        ImageView img;
        if (fifo_->try_pop_image(img)) {
            last_frame_ = img.frameNumber;
            total_frames_ += img.framesInPack;
            fifo_->push_free(img);
            
        }
        std::this_thread::sleep_for(DEFAULT_WAIT);
    }
    done_ = true;
}

void Writer::stop(){
    stopped_ = true;
}

int64_t Writer::last_frame() const{
    return last_frame_;
}

int64_t Writer::total_frames() const{
    return total_frames_;
}

bool Writer::done() const{
    return done_;
}

} // namespace reuss