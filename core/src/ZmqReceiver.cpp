#include "reuss/ZmqReceiver.h"
#include "reuss/project_defs.h"
#include <zmq.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <errno.h>
namespace reuss {
ZmqReceiver::ZmqReceiver(const std::string &endpoint) : endpoint(endpoint) {
    
}

ZmqReceiver::~ZmqReceiver() = default;

void ZmqReceiver::connect(){
    if (context || socket)
        throw std::runtime_error("Socket and context is not null");
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_SUB);
    zmq_connect(socket, endpoint.c_str());
    zmq_setsockopt(socket, ZMQ_RCVHWM, &zmq_hwm, sizeof(zmq_hwm));
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
}

void ZmqReceiver::disconnect(){
        // zmq teardown
    zmq_close(socket);
    zmq_ctx_destroy(context);
    socket = nullptr;
    context = nullptr;
}

int ZmqReceiver::receive_into(int n_frames, int64_t *frame_numbers, std::byte *data) {
    int caught_frames = 0;
    
    int64_t previous_frame_number = -1;
    for (int i=0; i<n_frames; ++i) {
        // Header
        int64_t frame_number = 0;
        int rc = zmq_recv(socket, &frame_number, sizeof(frame_number), 0);
        if (rc == -1){
            // fmt::print("No data\n");
            int errsv = errno;
            fmt::print("No data: {}\n", strerror(errsv));
            continue;
        }
        *frame_numbers++ = frame_number; //copy to output

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
            int nbytes = zmq_recv(socket, data, 1048576, 0);
            if (nbytes == -1)
                throw std::runtime_error("Got half of a multipart msg!!!");
            data += nbytes;
        }
        previous_frame_number = frame_number;
        ++caught_frames;
    }
    return caught_frames;

}

void ZmqReceiver::set_zmq_hwm(uint64_t hwm){
    zmq_hwm = hwm;
}

void ZmqReceiver::set_timeout(int ms){
    timeout = ms;
}



} // namespace reuss
