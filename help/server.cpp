#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define on_error(...)                 \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
        fflush(stderr);               \
        exit(1);                      \
    }

char *generate_long_string(int n)
{
    char *buffer = (char *)malloc(n + 1);
    for (int i = 0; i < n; i++)
    {
        buffer[i] = 'a' + i % 26;
    }
    buffer[n] = 0;
    return buffer;
}

int main(int argc, char *argv[])
{
    srand((unsigned)time(0));
    if (argc < 2)
        on_error("Usage: %s [port]\n", argv[0]);

    int port = atoi(argv[1]);

    int server_fd, client_fd, err;
    struct sockaddr_in server, client;
    char buf[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        on_error("Could not create socket\n");

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

    err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    if (err < 0)
        on_error("Could not bind socket\n");

    err = listen(server_fd, 128);
    if (err < 0)
        on_error("Could not listen on socket\n");

    printf("Server is listening on %d\n", port);

    while (1)
    {
        socklen_t client_len = sizeof(client);
        client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

        if (client_fd < 0)
            on_error("Could not establish new connection\n");

        while (1)
        {
            bzero(buf, BUFFER_SIZE);
            int read = recv(client_fd, buf, BUFFER_SIZE, 0);
            printf("Recv: %s\n", buf);
            if (!read)
                break; // done reading
            if (read < 0)
                on_error("Client read failed\n");
            
            char* msg = generate_long_string(rand()%9000+9000);

            err = send(client_fd, buf, read, 0);
            printf("Sent %d bytes\n", (int)strlen(msg));
            if (err < 0)
                on_error("Client write failed\n");
        }
    }

    return 0;
}