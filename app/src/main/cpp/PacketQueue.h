//
// Created by Di on 2019-06-20.
//

#ifndef FFMPEGDEMO_PACKETQUEUE_H
#define FFMPEGDEMO_PACKETQUEUE_H


class PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
};


#endif //FFMPEGDEMO_PACKETQUEUE_H

