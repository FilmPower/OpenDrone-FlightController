#include "MavLink.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#if (defined __QNX__) | (defined __QNXNTO__)
/* QNX specific headers */
#include <unix.h>
#else
/* Linux / MacOS POSIX timer headers */
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdbool.h> /* required for the definition of bool in C99 */
#endif

/* This assumes you have the mavlink headers on your include path
 or in the same folder as this source file */
#include <mavlink/common/mavlink.h>
#include <iostream>

#include "../Controller/PID.h"

#define BUFFER_LENGTH 2041 // minimum buffer size that can be used with qnx (I don't know why)

uint64_t microsSinceEpoch();


/* QNX timer version */
#if (defined __QNX__) | (defined __QNXNTO__)
uint64_t microsSinceEpoch()
{

    struct timespec time;

    uint64_t micros = 0;

    clock_gettime(CLOCK_REALTIME, &time);
    micros = (uint64_t)time.tv_sec * 1000000 + time.tv_nsec / 1000;

    return micros;
}
#else
uint64_t microsSinceEpoch()
{

    struct timeval tv;

    uint64_t micros = 0;

    gettimeofday(&tv, NULL);
    micros = ((uint64_t)tv.tv_sec) * 1000000 + tv.tv_usec;

    return micros;
}
#endif

void MavLink::start() {
    sleep(2);
    printf("\tStarted\n\n");
    char help[] = "--help";


    char target_ip[100];

    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in gcAddr;
    struct sockaddr_in locAddr;
    //struct sockaddr_in fromAddr;
    uint8_t buf[BUFFER_LENGTH];
    ssize_t recsize;
    socklen_t fromlen = sizeof(gcAddr);
    int bytes_sent;
    mavlink_message_t msg;
    uint16_t len;
    int i = 0;
    //int success = 0;
    unsigned int temp = 0;

    // Change the target ip if parameter was given
    strcpy(target_ip, "172.168.1.2");
    printf("IP OK...\n");


    memset(&locAddr, 0, sizeof(locAddr));
    locAddr.sin_family = AF_INET;
    locAddr.sin_addr.s_addr = INADDR_ANY;
    locAddr.sin_port = htons(14551);

    /* Bind the socket to port 14551 - necessary to receive packets from qgroundcontrol */
    if (-1 == bind(sock, (struct sockaddr*) & locAddr, sizeof(struct sockaddr)))
    {
        perror("error bind failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Bind OK...\n");

    /* Attempt to make it non blocking */
#if (defined __QNX__) | (defined __QNXNTO__)
    if (fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
#else
    if (fcntl(sock, F_SETFL, O_NONBLOCK | O_ASYNC) < 0)
#endif
    {
        fprintf(stderr, "error setting nonblocking: %s\n", strerror(errno));
        close(sock);
        exit(EXIT_FAILURE);
    }

    memset(&gcAddr, 0, sizeof(gcAddr));
    gcAddr.sin_family = AF_INET;
    gcAddr.sin_addr.s_addr = inet_addr(target_ip);
    gcAddr.sin_port = htons(14550);

    //Send Version
    mavlink_msg_protocol_version_encode(1, 200, &msg, new mavlink_protocol_version_t);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*) & gcAddr, sizeof(struct sockaddr_in));

    mavlink_sys_status_t* sysstat = new mavlink_sys_status_t();
    mavlink_msg_sys_status_encode(1, 200, &msg, sysstat);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*) & gcAddr, sizeof(struct sockaddr_in));

    for (;;)
    {
        //Send Version
        mavlink_msg_protocol_version_encode(1, 200, &msg, new mavlink_protocol_version_t);
        len = mavlink_msg_to_send_buffer(buf, &msg);
        bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*) & gcAddr, sizeof(struct sockaddr_in));

        /*Send Heartbeat */
        mavlink_msg_heartbeat_pack(1, 200, &msg, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_GENERIC_WAYPOINTS_ONLY, MAV_MODE_STABILIZE_DISARMED, 0, MAV_STATE_STANDBY);
        len = mavlink_msg_to_send_buffer(buf, &msg);
        bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*) & gcAddr, sizeof(struct sockaddr_in));

        memset(buf, 0, BUFFER_LENGTH);
        recsize = recvfrom(sock, (void*)buf, BUFFER_LENGTH, 0, (struct sockaddr*) & gcAddr, &fromlen);
        if (recsize > 0)
        {
            PID* pid = PID::getInstanceCreated();

            // Something received - print out all bytes and parse packet
            mavlink_message_t msg;
            mavlink_status_t status;
            for (i = 0; i < recsize; ++i)
            {
                //temp = buf[i];
                //printf("%02x ", (unsigned char)temp);
                if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status))
                {
                    switch (msg.msgid) {
                    case MAVLINK_MSG_ID_HEARTBEAT: // ID for MAVLINK_MSG_ID_HEARTBEAT
                    {
                        mavlink_heartbeat_t* hb = new mavlink_heartbeat_t();
                        mavlink_msg_heartbeat_decode(&msg, hb);
                        if (hb->type == MAV_TYPE_GCS) {
                            printf("GCS Alive and Connected...\n");
                        }
                        else {
                            printf("Connection lost");
                            //TODO when connection is lost
                        }
                        break;
                    }
                    case MAVLINK_MSG_ID_MANUAL_SETPOINT:
                    {
                        printf("PitchRollYaw\n");

                        mavlink_manual_setpoint_t* command = new mavlink_manual_setpoint_t();
                        mavlink_msg_manual_setpoint_decode(&msg, command);

                        uint32_t timestamp = command->time_boot_ms;
                        float pitch = command->pitch;
                        float roll = command->roll;
                        float yaw = command->yaw;

                        if (timestamp > lastRecieved) {
                            //TODO: Run setPitch/setRoll/setYaw
                        }
                        break;
                    }
                    case MAVLINK_MSG_ID_COMMAND_LONG:
                    {
                        mavlink_command_long_t* command = new mavlink_command_long_t();
                        mavlink_msg_command_long_decode(&msg, command);
                        switch (command->command) {
                        case MAV_CMD_COMPONENT_ARM_DISARM: {
                            if (command->param1 == 0) {
                                //TODO: Arm
                                printf("Arm\n");
                            }
                            else {
                                //TODO: Disarm
                                printf("Disarm\n");
                            }
                            break;
                        } case MAV_CMD_DO_FLIGHTTERMINATION: {
                            float status = command->param1;
                            if (status == 1) {
                                //TODO Shutdown drone -> Drone will crash
                            }
                            printf("Shutdown drone -> Drone will crash\n");
                            break;
                        } case MAV_CMD_NAV_LOITER_UNLIM: {
                            //TODO Method for setting the altitude-hold
                            printf("Method for setting the altitude-hold\n");
                            break;
                        } case MAV_CMD_DO_CHANGE_SPEED: {
                            //TODO Method for setting the throttle of the drone
                            printf("Method for setting the throttle of the drone\n");
                            float throttle = command->param3;
                            if (throttle != 0) {
                                //TODO: Call setThrottle method
                                std::cout << "Throttle: " << throttle << std::endl;
                            }
                            break;
                        } case MAV_CMD_NAV_TAKEOFF: {
                            //TODO Method for sending the take-off-command to the drone
                            printf("Method for sending the take-off-command to the drone\n");
                            break;
                        } case MAV_CMD_REQUEST_PROTOCOL_VERSION: {
                            //TODO MAV_CMD_REQUEST_PROTOCOL_VERSION
                            printf("MAV_CMD_REQUEST_PROTOCOL_VERSION\n");
                            break;
                        }
                        default:
                        {
                            printf("%d\n", msg.msgid);
                        }
                        break;
                        }
                    }
                    break;
                    default:
                        printf("%d\n", msg.msgid);
                    }
                }
            }
        }
        memset(buf, 0, BUFFER_LENGTH);

        usleep(50000);
    }
}
