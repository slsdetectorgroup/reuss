#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/project_defs.h"

#include "reuss/utils.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <string>

#include <stdlib.h> //posix_memalign

namespace reuss {

template <typename T> class RawFile {
    static constexpr auto writer_color = fmt::color::green;
    T writerImpl;

    std::string basename_;
    size_t target_frames_per_file_{1000};
    size_t n_written_{};
    size_t file_nr_{};

  public:
    RawFile(const std::string &basename, size_t frames_per_file)
        : basename_(basename), target_frames_per_file_(frames_per_file){
        writerImpl.open(currentFname());
    }
    ~RawFile() {
        close();
    }

    void write(size_t n_frames, void * data, size_t data_size) {
        if (n_written_ >= target_frames_per_file_) {
            close();
            open(fmt::format("{}_{}.bin", basename_, ++file_nr_));
        }
        n_written_ += n_frames;
        writerImpl.write(data, data_size);
    }
    
    std::string currentFname() {
        return fmt::format("{}_{}.bin", basename_, file_nr_);
    }

  private:
    void open(const std::string &fname) {
        fmt::print(fg(writer_color), "Opening: {} for writing\n",
                   currentFname());
        writerImpl.open(fname);
        n_written_ = 0;
    }

    void close() {
        fmt::print(fg(writer_color), "Closing: {}\n", currentFname());
        writerImpl.close();
    }

};

} // namespace reuss