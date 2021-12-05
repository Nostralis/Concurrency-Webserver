#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include <pthread.h>
#include "pool.h"

char default_root[] = ".";
int main(int argc, char *argv[])
{
    int c;
    char *root_dir = default_root;
    int port = 10000;
    int nbThread=5;
    int tBuff =10;

    while ((c = getopt(argc, argv, "d:p:t:b:")) != -1)
        switch (c)
        {
        case 'd':
            root_dir = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 't':
            nbThread = atoi(optarg);
            break;
        case 'b':
            tBuff = atoi(optarg);
            break;
        default:
            fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffer]\n");
            exit(1);
        }
    

    tPool *pool = defPool(nbThread, tBuff);
    printf("Web serveur avec %d threads.\n", nbThread);
// run out of this directory
    chdir_or_die(root_dir);
    int *new_conn;
// now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1)
    {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
        new_conn =malloc(1);
        *new_conn = conn_fd;

        if(conn_fd!=-1)
        {
            add_req(pool,request_handle, (void *)new_conn);
            printf("Nouvelle requête: ");
        }

    }
    stop(pool);
    kille(pool);
    return 0;
}






