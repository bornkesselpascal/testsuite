#include "uce.h"
#include <arpa/inet.h>
#include <cstring>
#include <linux/net_tstamp.h>
#include <netinet/ether.h>
#include <stdexcept>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <unistd.h>
#include <linux/sockios.h>


uce::server::server(sock_type type, std::string dst_address, int dst_port)
    : m_type(type)
    , m_dst_address(inet_addr(dst_address.c_str()))
    , m_dst_port(dst_port)
    , m_dst_interface(uce::support::get_interface_from_ip(dst_address))
{
    switch (m_type)
    {
    case sock_type::ST_UDP:
    {
        char decimal_port[16];
        snprintf(decimal_port, sizeof(decimal_port), "%d", dst_port);
        decimal_port[sizeof(decimal_port) / sizeof(decimal_port[0]) - 1] = '\0';

        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        int r = getaddrinfo(dst_address.c_str(), decimal_port, &hints, &m_addrinfo);
        if(r != 0 || m_addrinfo == NULL) {
            throw std::runtime_error("[uce_s - UDP] E01 -  Invalid address or port.");
        }

        m_socket = socket(m_addrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
        if(m_socket < 0)
        {
            freeaddrinfo(m_addrinfo);
            throw std::runtime_error("[comm_udp_s] E02 - Could not create socket for " + dst_address + ":" + std::to_string(dst_port) + ".");
        }

        if(bind(m_socket, m_addrinfo->ai_addr, m_addrinfo->ai_addrlen) < 0)
        {
            freeaddrinfo(m_addrinfo);
            close(m_socket);
            throw std::runtime_error("[uce_s - UDP] E03 - Could not bind socket with " + dst_address + ":" + std::to_string(dst_port) + ".");
        }

        break;
    }
    case sock_type::ST_RAW:
    case sock_type::ST_PACKET:
    {
        // Create the socket.
        if(m_type == sock_type::ST_PACKET)
        {
            m_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
        }
        else if(m_type == sock_type::ST_RAW)
        {
            m_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
        }
        if(m_socket < 0)
        {
            throw std::runtime_error("[uce_s] E02 - Could not create socket.");
        }

        // Bind the socket to an network interface.
        if(m_type == sock_type::ST_PACKET)
        {
            // Get index number of interface.
            struct ifreq ifr;
            strncpy(ifr.ifr_name, m_dst_interface.c_str(), sizeof(ifr.ifr_name));
            if(ioctl(m_socket, SIOCGIFINDEX, &ifr) != 0)
            {
                throw std::runtime_error("[uce_s] E03 - Could not get interface index number for " + m_dst_interface + ".");
            }

            // Bind interface to socket.
            struct sockaddr_ll sock_addr;
            memset(&sock_addr, 0, sizeof(struct sockaddr_ll));
            sock_addr.sll_family = AF_PACKET;           // link layer address family
            sock_addr.sll_ifindex = ifr.ifr_ifindex;    // interface index number
            if (0 != bind(m_socket, (struct sockaddr*) &sock_addr, sizeof(sock_addr)))
            {
                throw std::runtime_error("[uce_s] E03 - Could not bind socket.");
            }
        }
        else if(m_type == sock_type::ST_RAW)
        {
            struct sockaddr_in src_info;
            src_info.sin_family = AF_INET;
            src_info.sin_addr.s_addr = m_dst_address;
            src_info.sin_port = htons(m_dst_port);

            if (0 != bind(m_socket, (struct sockaddr*) &src_info, sizeof(src_info)))
            {
                throw std::runtime_error("[uce_s] E03 - Could not bind socket.");
            }
        }

        // Disable ICMP port unrechable messages.
        if (system("iptables -I OUTPUT -p icmp --icmp-type port-unreachable -j DROP") < 0)
        {
            throw std::runtime_error("[uce_s] E04 - Could not disable IMCP port unreachable messages.");
        }

        break;
    }
    }

    // Initialize receive datastructures.
    memset(&(m_receive_helper.iov), 0, sizeof(struct iovec));
    memset(&(m_receive_helper.msh), 0, sizeof(struct msghdr));

    m_receive_helper.msh.msg_iov = &(m_receive_helper.iov);
    m_receive_helper.msh.msg_iovlen = 1;
    m_receive_helper.msh.msg_namelen = sizeof(struct sockaddr_in);
    m_receive_helper.msh.msg_control = m_receive_helper.control_buffer;
    m_receive_helper.msh.msg_controllen = sizeof(m_receive_helper.control_buffer);
}

uce::server::~server()
{
    switch (m_type)
    {
    case sock_type::ST_UDP:
    {
        freeaddrinfo(m_addrinfo);
        close(m_socket);
        break;
    }
    case sock_type::ST_RAW:
    case sock_type::ST_PACKET:
    {
        close(m_socket);
        break;
    }
    }
}

bool uce::server::enable_timestamps(timestamp_mode mode)
{
    switch (mode) {
    case timestamp_mode::TSTMP_SW:
    {
        // Enable software timestamping.
        int flags = SOF_TIMESTAMPING_RX_SOFTWARE | SOF_TIMESTAMPING_SOFTWARE;
        if (setsockopt(m_socket, SOL_SOCKET, SO_TIMESTAMPING, &flags, sizeof(flags)) < 0)
        {
            return false;
        }

        server_timestamps.enabled = true;
        return true;
    }
    case timestamp_mode::TSTMP_ALL:
    {
        // Enable hardware timestamping on the interface.
        // NOTE: Intel X710 and Intel X540/X520 do not support timestamping of incoming packets.
        struct ifreq ifr;
        struct hwtstamp_config hwconfig;

        memset(&ifr, 0, sizeof(ifr));
        memset(&hwconfig, 0, sizeof(hwconfig));

        strncpy(ifr.ifr_name, m_dst_interface.c_str(), sizeof(ifr.ifr_name));
        hwconfig.rx_filter = HWTSTAMP_FILTER_ALL;
        ifr.ifr_data = reinterpret_cast<char*>(&hwconfig);

        if (ioctl(m_socket, SIOCSHWTSTAMP, &ifr) < 0)
        {
            return false;
        }

        // Enable hardware and software timestamping.
        int flags = SOF_TIMESTAMPING_RX_HARDWARE | SOF_TIMESTAMPING_RX_SOFTWARE | SOF_TIMESTAMPING_SOFTWARE | SOF_TIMESTAMPING_RAW_HARDWARE;
        if (setsockopt(m_socket, SOL_SOCKET, SO_TIMESTAMPING, &flags, sizeof(flags)) < 0)
        {
            return false;
        }

        server_timestamps.enabled = true;
        return true;
    }
    }

    return false;
}

int uce::server::receive(void *msg, size_t max_size)
{
    m_receive_helper.iov.iov_base = msg;
    m_receive_helper.iov.iov_len = max_size;

    switch (m_type)
    {
    case sock_type::ST_UDP:
    {
        int bytes = ::recvmsg(m_socket, &(m_receive_helper.msh), 0);
        if (server_timestamps.enabled) clock_gettime(CLOCK_REALTIME, &(server_timestamps.m_rec_program));

        // Process ancillary data to extract sw/hw timestamps.
        if(server_timestamps.enabled)
        {
            struct cmsghdr *cmsg;
            for (cmsg = CMSG_FIRSTHDR(&(m_receive_helper.msh)); cmsg != NULL; cmsg = CMSG_NXTHDR(&(m_receive_helper.msh), cmsg))
            {
                if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMPING)
                {
                    struct timespec *timestamps = (struct timespec *) CMSG_DATA(cmsg);
                    server_timestamps.m_rec_sw = timestamps[0]; // software timestamp
                    server_timestamps.m_rec_hw = timestamps[2]; // hardware timestamp
                }
            }
        }

        return bytes;
    }
    case sock_type::ST_RAW:
    case sock_type::ST_PACKET:
    {
        struct iphdr* hip_ptr;
        struct udphdr* hudp_ptr;
        if (m_type == sock_type::ST_PACKET)
        {
            hip_ptr  = (struct iphdr*) ((char*) msg + UCE_HEADER_ETH_SIZE);
            hudp_ptr = (struct udphdr*) ((char *) msg + UCE_HEADER_ETH_SIZE + UCE_HEADER_IP_SIZE);
        }
        else if (m_type == sock_type::ST_RAW)
        {
            hip_ptr  = (struct iphdr*) msg;
            hudp_ptr = (struct udphdr*) ((char *) msg + UCE_HEADER_IP_SIZE);
        }

        while(true)
        {
            int bytes = ::recvmsg(m_socket, &(m_receive_helper.msh), 0);
            if (-1 == bytes)
            {
                return bytes;
            }

            // Check if the recieved message is an UDP message.
            if (IPPROTO_UDP != hip_ptr->protocol)
            {
                continue;
            }

            // Get program timestamp.
            if (server_timestamps.enabled) clock_gettime(CLOCK_REALTIME, &(server_timestamps.m_rec_program));

            // Check if the destination port matches.
            if (m_dst_port != ntohs(hudp_ptr->dest))
            {
                continue;
            }

            // Process ancillary data to extract sw/hw timestamps.
            if(server_timestamps.enabled)
            {
                struct cmsghdr *cmsg;
                for (cmsg = CMSG_FIRSTHDR(&(m_receive_helper.msh)); cmsg != NULL; cmsg = CMSG_NXTHDR(&(m_receive_helper.msh), cmsg))
                {
                    if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMPING)
                    {
                        struct timespec *timestamps = (struct timespec *) CMSG_DATA(cmsg);
                        server_timestamps.m_rec_sw = timestamps[0]; // software timestamp
                        server_timestamps.m_rec_hw = timestamps[2]; // hardware timestamp
                    }
                }
            }

            return bytes;
        }
    }
    }

    return -1;
}
