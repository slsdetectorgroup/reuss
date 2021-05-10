#pragma once
/*
Project wide definitions and configuration
*/


#include <chrono>
#include <cstddef>
#include <cstdint>
// copy of slsDetectorDefs::sls_detector_header; be careful with versions!
struct PacketHeader {
    uint64_t frameNumber;
    uint32_t expLength; /* Eiger: numsubframes, exptime x 100ns others) */
    uint32_t packetNumber;
    uint64_t bunchId;
    uint64_t timestamp;
    uint16_t modId;
    uint16_t row;
    uint16_t column;
    uint16_t reserved;
    uint32_t debug;
    uint16_t roundRNumber;
    uint8_t detType;
    uint8_t version;
};
constexpr size_t QUEUE_SIZE = 100;
constexpr size_t NROW = 256;
constexpr size_t NCOL = 1024;
// constexpr size_t PACKETS_PER_FRAME = 128;
constexpr size_t PACKETS_PER_FRAME = 64;
constexpr int PAYLOAD_SIZE = 8192;
constexpr int PACKET_SIZE = sizeof(PacketHeader) + PAYLOAD_SIZE;
constexpr size_t FRAME_SIZE = NROW * NCOL * sizeof(uint16_t);
constexpr size_t DEFAULT_UDP_BUFFER_SIZE = 1024 * 1024 * 50;
constexpr auto DEFAULT_ENDPOINT = "tcp://*:4545";
constexpr auto DEFAULT_RECEIVE = "tcp://localhost:4545";
constexpr auto DEFAULT_WAIT = std::chrono::microseconds(100);
constexpr size_t IO_ALIGNMENT = 4096;
constexpr int64_t PRINT_MOD = 1000;
constexpr size_t BUFFER_SIZE = 1048576;

