#include "reuss/ZmqReceiver.h"
#include "reuss/project_defs.h"
#include <zmq.h>
#include <fmt/core.h>
#include <fmt/color.h>
namespace reuss {
ZmqReceiver::ZmqReceiver(const std::string &endpoint) : endpoint(endpoint) {}

void ZmqReceiver::receive_into(int n_frames, int64_t *frame_numbers, std::byte *data) {

    // zmq setup
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_SUB);
    zmq_connect(socket, endpoint.c_str());
    zmq_setsockopt(socket, ZMQ_RCVHWM, &zmq_hwm, sizeof(zmq_hwm));
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);

    int64_t previous_frame_number = -1;
    for (int i=0; i<n_frames; ++i) {
        // Header
        int64_t frame_number = 0;
        zmq_msg_t header_msg;
        zmq_msg_init(&header_msg);
        zmq_msg_recv(&header_msg, socket, 0);
        int size = zmq_msg_size(&header_msg);
        memcpy(&frame_number, zmq_msg_data(&header_msg), size);
        *frame_numbers++ = frame_number; //copy to output
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
            int nbytes = zmq_recv(socket, data, 1048576, 0);
            data += nbytes;
        }
        previous_frame_number = frame_number;
    }

    // zmq teardown
    zmq_close(socket);
    zmq_ctx_destroy(context);

}

void ZmqReceiver::set_zmq_hwm(uint64_t hwm){
    zmq_hwm = hwm;
}


} // namespace reuss
