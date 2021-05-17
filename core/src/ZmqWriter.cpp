

#include "reuss/ZmqWriter.h"
#include "DirectWriter.h"
#include "File.h"

namespace reuss {

ZmqWriter::ZmqWriter(const std::string &endpoint) : endpoint(endpoint) {}
void ZmqWriter::set_fname(const std::string &fname) { filename = fname; }
void ZmqWriter::start() {
    if (running) {
        fmt::print("Writer already running\n");
        throw 1;
    }
    writer_thread = std::thread(&ZmqWriter::write, this);
}
void ZmqWriter::stop() {
    if (!running) {
        fmt::print("Writer is not running and cannot be stopped\n");
        throw 1;
    }
    request_stop = true;
    writer_thread.join();
}

bool ZmqWriter::is_running() { return running; }
bool ZmqWriter::stop_requested() { return request_stop; }

int64_t ZmqWriter::frames_written() const noexcept { return frames_written_; }

void ZmqWriter::write() {
    // thread is launched so in case of stop we should let it do the
    // teardown

    running = true;

    // Setup the file
    File<DirectWriter> f(filename, 5000);

    // zmq setup
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_SUB);
    zmq_connect(socket, endpoint.c_str());
    uint64_t hwm = 10000;
    zmq_setsockopt(socket, ZMQ_RCVHWM, &hwm, sizeof(hwm));
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);

    auto buffer =
        static_cast<std::byte *>(std::aligned_alloc(IO_ALIGNMENT, BUFFER_SIZE));

    int64_t previous_frame_number = -1;
    while (!request_stop) {
        // Header
        int64_t frame_number = 0;
        zmq_msg_t header_msg;
        zmq_msg_init(&header_msg);
        zmq_msg_recv(&header_msg, socket, 0);
        int size = zmq_msg_size(&header_msg);
        memcpy(&frame_number, zmq_msg_data(&header_msg), size);
        zmq_msg_close(&header_msg);

        // Notify if we drop frames
        int64_t diff = frame_number - previous_frame_number;
        if (previous_frame_number > 0 && diff != 1)
            fmt::print(fg(fmt::color::red),
                       "current: {} last: {} Lost {} frames\n", frame_number,
                       previous_frame_number, diff);

        // Data
        int more;
        size_t more_size = sizeof(more);
        zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
        if (more) {
            int nbytes = zmq_recv(socket, buffer, 1048576, 0);
            // fmt::print("Writer got: {} bytes\n", nbytes);
            f.write(frame_number, buffer, nbytes);
        }
        previous_frame_number = frame_number;
    }

    // zmq teardown
    zmq_close(socket);
    zmq_ctx_destroy(context);

    running = false;
    request_stop = false;
    free(buffer);
}

} // namespace reuss