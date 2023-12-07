//
// Created by bram on 7/12/23.
//

#ifndef PROJECT_CONNMGR_H
#define PROJECT_CONNMGR_H

#include "lib/tcpsock.h"

/**
 * Implements a parallel test server (multiple connections at the same time)
 */

/**
 * Handles a client connection
 * \param arg a pointer to the tcpsock_t structure representing the client connection
 * \return NULL
 */
void *handle_client(void *arg);

/**
 * Main function of the connection manager
 * \param argc number of command-line arguments
 * \param argv array of command-line arguments
 * \return 0 on success, -1 on failure
 */
int main(int argc, char *argv[]);

#endif //PROJECT_CONNMGR_H
