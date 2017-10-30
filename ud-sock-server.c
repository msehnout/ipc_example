#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MY_SOCK_PATH "/tmp/MY_LOCAL_UNIX_SOCKET"
#define RECV_BUFFER_SIZE 2048

int main(void) {
    printf("Running Unix domain server\n");
    // Create a socket file
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    // Check for errors
    if (s == -1) { goto fail; }
    // Now create a structure, that describes the socket parameters
    struct sockaddr_un local;
    // Clean up the memory for this variable
    memset(&local, 0, sizeof(struct sockaddr_un));
    // Set socket family to the Unix domain
    local.sun_family = AF_UNIX;
    // Copy defined path to the structure
    strncpy(local.sun_path, MY_SOCK_PATH, sizeof(MY_SOCK_PATH));
    // If there is already a socket, remove it
    unlink(local.sun_path);
    // Finally bind our file descriptor "s" to this location (as described above)
    int ret = bind(s, (struct sockaddr *)&local, sizeof(struct sockaddr_un));
    // Again check for any errors
    if (ret == -1) { goto fail; }
    // Listen for incoming connection (only 1!)
    listen(s, 1);
    // Declare a client file descriptor, it will be used for reading data from client
    int client_fd;
    // Peer address and its size
    struct sockaddr_un peer_addr;
    socklen_t peer_addr_size;
    // Wait for a client connection and accept it
    client_fd = accept(s, (struct sockaddr *) &peer_addr, &peer_addr_size);
    // ...
    if (client_fd == -1) { goto fail; }
    // Create a receive buffer
    char buffer[RECV_BUFFER_SIZE];
    // Clear the buffer
    memset(buffer, 0, RECV_BUFFER_SIZE);
    // Read data from client as long as there are some
    while(recv(client_fd, buffer, RECV_BUFFER_SIZE, 0) > 0) {
        // Print them
        printf("Received: %s", buffer);
        // Clear them
        memset(buffer, 0, RECV_BUFFER_SIZE);
    }
    // Remove the socket
    unlink(local.sun_path);
    return 0;

// In case of failure, jump here
fail:
    fprintf(stderr, "ERROR!\n");
    return -1;
}