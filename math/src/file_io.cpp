#include "reuss/file_io.h"
#include <fstream>
namespace reuss {

// NumpyFileHeader load_numpy_header(const std::string &fname) {

//     std::ifstream f(fname, "rb");
//     std::array<char, 6> tmp;
//     f.read(tmp.data(), sizeof(tmp));
//     return {};
// }


ImageData<uint16_t, 3> load_raw_bin(const std::string& fname){
    std::ifstream f(fname, std::ios::binary);
    if (!f)
        throw std::runtime_error("File not found");

    f.seekg(0, std::ios::end);
    auto eof = f.tellg();
    f.seekg(0, std::ios::beg);
    f.seekg(eof-16);
    int64_t n_frames, meta_size;
    f.read(reinterpret_cast<char*>(&n_frames), sizeof(int64_t));
    f.read(reinterpret_cast<char*>(&meta_size), sizeof(int64_t));
    f.seekg(0);
    fmt::print("Trying to read {} frames\n", n_frames);
    ImageData<uint16_t, 3> data{{n_frames, 512, 1024}, 0};
    // f.read(reinterpret_cast<char*>(data.buffer()), data.size() * sizeof(uint16_t));
    // std::vector<int64_t> meta(n_frames);
    // f.read(reinterpret_cast<char*>(meta.data()), meta.size() * sizeof(int64_t));
    auto expect_size = meta_size + data.size() * 2;
    if(eof != expect_size){
        throw std::runtime_error("File size does not match, expected: " + std::to_string(expect_size) + ", actual: " + std::to_string(eof) + ". Corrupt file?");
    }
    return data;

}

// def load_raw_bin(fname):
//     with open(fname, 'rb') as f:
//         eof = f.seek(0, 2)
//         f.seek(eof-16)
//         n_frames, meta_size = np.fromfile(f, count = 2, dtype = np.int64)
//         f.seek(0)
//         print(f'Trying to read {n_frames} frames')
//         # data = np.fromfile(f, count = 512*512*n_frames, dtype = np.uint16).reshape((n_frames, 512,512))
//         data = np.fromfile(f, count = 512*1024*n_frames, dtype = np.uint16).reshape((n_frames, 512,1024))
//         # at the moment we don't write any other meta info so we return only frame numbers not the full
//         # meta block
//         meta = np.fromfile(f, count = n_frames, dtype = np.int64)

//         #Since the file large we can affort to verify the size
//         expect_size = meta_size + data.size *2
//         assert eof == expect_size, f"File size does not match, expected: {expected}, actual: {eof}. Corrupt file?"

//         return data, meta


} // namespace reuss