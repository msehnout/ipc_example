#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

#define TAG_PARENT "[PARENT] "
#define TAG_CHILD "[CHILD] "
#define MY_SOCK_PATH "/tmp/MY_LOCAL_UNIX_SOCKET"
#define RECV_BUFFER_SIZE 2048

void set_ud_socket(struct sockaddr_un *sock) {
    memset(sock, 0, sizeof(struct sockaddr_un));
    sock->sun_family = AF_UNIX;
    strncpy(sock->sun_path, MY_SOCK_PATH, sizeof(MY_SOCK_PATH));
}

void run_server(int child_pid) {
    // Run server
    // Creates a socket, which is basically just a file descriptor
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1) {
        goto fail;
    }
    // Now create a structure, that describes the socket parameters
    struct sockaddr_un local;
    set_ud_socket(&local);
    // If there is already a socket, remove it:
    unlink(local.sun_path);
    // Finally bind our file descriptor "s" to this location (as described above)
    int ret = bind(s, (struct sockaddr *)&local, sizeof(struct sockaddr_un));
    if (ret == -1) {
        goto fail;
    }
    // Listen for incoming connection (only 1!)
    listen(s, 1);

    int client_fd;
    struct sockaddr_un peer_addr;
    socklen_t peer_addr_size;

    client_fd = accept(s, (struct sockaddr *) &peer_addr, &peer_addr_size);
    if (client_fd == -1) {
        goto fail;
    }

    printf(TAG_PARENT "Hell, yeah!\n");
    char buffer[RECV_BUFFER_SIZE];
    memset(buffer, 0, RECV_BUFFER_SIZE);
    while(recv(client_fd, buffer, RECV_BUFFER_SIZE, 0) > 0) {
        printf("Recieved: %s\n", buffer);
        memset(buffer, 0, RECV_BUFFER_SIZE);
    }

    // Wait for the child to finish
    int status;
    (void)waitpid(child_pid, &status, 0);

    // Remove the socket we used
    unlink(local.sun_path);

    return;

fail:
    printf(TAG_PARENT "THERE WAS AN ERROR! If you see this message, you should write more debug output.\n");
}

void run_client() {
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1) {
        goto fail;
    }

    struct sockaddr_un remote;
    set_ud_socket(&remote);
    int ret = connect(s, (struct sockaddr *)&remote, sizeof(struct sockaddr_un));
    if (ret == -1) {
        goto fail;
    }

    printf(TAG_CHILD "I'm connected, let's send 10,000 random numbers!\n");
    srand(time(NULL));
    int r = rand();      // returns a pseudo-random integer between 0 and RAND_MAX
    for (int i = 0; i<10; ++i) {
        if (send(s, "ahoj\n", 5, 0) == -1) {
            goto fail;
        }
    }

    close(s);
    return;

fail:
    printf(TAG_CHILD "THERE WAS AN ERROR! If you see this message, you should write more debug output.\n");

}

int main() {
    printf(TAG_PARENT "Example of process creation and IPC:\n");
    pid_t pid = fork();

    switch (pid) {
        case -1:
            perror("fork failed");
            exit(EXIT_FAILURE);

        case 0:
            printf(TAG_CHILD "Hello from the child process!\n");
            run_client();
            _exit(EXIT_SUCCESS);

        default:
            run_server(pid);
            break;
    }

    return EXIT_SUCCESS;

}