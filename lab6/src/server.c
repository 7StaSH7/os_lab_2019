#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <net/if.h>
#include "pthread.h"
#include "factorial.h"

int main(int argc, char **argv) {
  int tnum = -1;
  int port = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        port = atoi(optarg);
        
        if(port <= 0) {
            printf("Port is a positive number.\n");
            exit(1);
        }

        break;
      case 1:
        tnum = atoi(optarg);
        
        if(tnum <= 0) {
            printf("Threads num is a positive number.\n");
            exit(1);
        }
        

        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Unknown argument\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

    if (port == -1 || tnum == -1) {
        fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
        return 1;
    }
    int err;
    union {
        struct sockaddr_in6 sin6;
    } server; 
    union {
        struct sockaddr_in6 sin6;
    } client;

    socklen_t client_size = sizeof(client);
    socklen_t server_size = sizeof(struct sockaddr_in6);;
    int c;

    int server_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd  < 0) {     
        perror("socket failed");
        exit (1);
    }

    int opt_val = 1;
    err = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);
    /* initialize the server's sockaddr */
    memset(&server, 0, server_size);
   
    server.sin6.sin6_family = AF_INET6;
    server.sin6.sin6_addr = in6addr_any;
    server.sin6.sin6_port = htons((uint16_t)port);
    

    err = bind(server_fd ,(struct sockaddr *)&server, server_size);
    if (err < 0) {
        perror("bind failed");
        exit(1);
    }
    err = listen(server_fd, 128);
    if (err < 0) {
        fprintf(stderr, "Could not listen on socket\n");
        return 1;
    }
    printf("Server listening at %d\n", port);
  while (true) {

    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_size);

    if (client_fd < 0) {
      fprintf(stderr, "Could not establish new connection\n");
      continue;
    }

    while (true) {
      unsigned int buffer_size = sizeof(uint64_t) * 3;
      char from_client[buffer_size];
      int read = recv(client_fd, from_client, buffer_size, 0);

      if (!read)
        break;
      if (read < 0) {
        fprintf(stderr, "Client read failed\n");
        break;
      }
      if (read < buffer_size) {
        fprintf(stderr, "Client send wrong data format\n");
        break;
      }

      pthread_t threads[tnum];

      uint64_t begin = 0;
      uint64_t end = 0;
      uint64_t mod = 0;
      memcpy(&begin, from_client, sizeof(uint64_t));
      memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
      memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

      fprintf(stdout, "Receive: %lu %lu %lu\n", begin, end, mod);
      uint64_t step = (end-begin)/tnum + 1;

      struct FactorialArgs args[tnum];
      for (uint32_t i = 0; i < tnum; i++) {
        // TODO: parallel somehow
        args[i].begin = begin + step*i + ((i == 0) ? 0 : 1);
        args[i].end = ((begin + step * (i + 1)) < end) ? begin + step * (i + 1) : end;
        args[i].mod = mod;

        if (pthread_create(&threads[i], NULL, ThreadFactorial,
                           (void *)&args[i])) {
          printf("Error: pthread_create failed!\n");
          return 1;
        }
      }

      uint64_t total = 1;
      for (uint32_t i = 0; i < tnum; i++) {
        uint64_t result = 0;
        pthread_join(threads[i], (void **)&result);
        total = MultModulo(total, result, mod);
      }

      printf("Total: %lu\n", total);

      char buffer[sizeof(total)];
      memcpy(buffer, &total, sizeof(total));
      err = send(client_fd, buffer, sizeof(total), 0);
      if (err < 0) {
        fprintf(stderr, "Can't send data to client\n");
        break;
      }
    }

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
  }

  return 0;
}