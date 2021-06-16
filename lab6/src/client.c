#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "factorial.h"

struct Server {
  char ip[255];
  int port;
};

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        // TODO: 
        
        if(k <= 0) {
            printf("Argument is a positive number.\n");
            exit(1);
        }

        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        // TODO:
        
        if(mod <= 0) printf("Module is a positive number.\n");

        break;
      case 2:
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

    // FILE* servers_storage = fopen(servers, "r");

    // if(servers_storage == NULL) printf("Can't open %s.\n", servers);

  unsigned int servers_num = 1;

//     while(!feof(servers_storage)) {
//         if(fgetc(servers_storage) == ';')
//         servers_num++;
//     }
//     fseek(servers_storage, 0, SEEK_SET);
//   struct Server *to = (struct Server*)malloc(sizeof(struct Server) * servers_num);
  
//    for(int i = 0; i < servers_num; i++) fscanf(servers_storage,"%s %d;", to[i].ip, &to[i].port);
  
//   fclose(servers_storage);
  int step = k / servers_num + 1;
  uint64_t answer = 1;
//   for (int i = 0; i < servers_num; i++) 
    // struct hostent *hostname = getaddrinfo(to[i].ip);
    // if (hostname == NULL) {
    //   fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
    //   exit(1);
    // }

    struct sockaddr_in6 server;
    server.sin6_family = AF_INET6;
	inet_pton(AF_INET6, "::", &server.sin6_addr);
	server.sin6_port = htons(45493);

    int sck = socket(AF_INET6, SOCK_STREAM, 0);
    if (sck < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }
    uint64_t begin = 1 ;
    uint64_t end = ((1+1)*step < k) ? (1+1)*step : k;
    

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
      fprintf(stderr, "Send failed\n");
      exit(1);
    }

    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
      fprintf(stderr, "Recieve failed\n");
      exit(1);
    }

    uint64_t from = 0;
    memcpy(&from, response, sizeof(uint64_t));
    answer = MultModulo(answer, from, mod);


    close(sck);
  
  printf("Answer: %lu\n", answer);

//   free(to);

  return 0;
}