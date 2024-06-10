#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MSG_LENGTH 64
#define LOG_FILE "logfile.txt"

void write_to_log(const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("Could not open log file");
        return;
    }
    fprintf(log_file, "%s\n", message);
    printf("write  %s\n", message);
    fclose(log_file);
}

int count_log_entries() {
    FILE *log_file = fopen(LOG_FILE, "r");
    if (log_file == NULL) {
        perror("Could not open log file");
        return 0;
    }

    char buffer[MSG_LENGTH];
    int count = 0;

    while (fgets(buffer, MSG_LENGTH, log_file) != NULL) {
        count++;
    }
    fclose(log_file);

    return count;
}

void read_last_n_messages(int n, char messages[][MSG_LENGTH]) {
    FILE *log_file = fopen(LOG_FILE, "r");
    if (log_file == NULL) {
        perror("Could not open log file");
        return;
    }

    char buffer[MSG_LENGTH];
    int total_entries = count_log_entries();
    int start_index = total_entries - n;

    int count = 0;
    int index = 0;

    while (fgets(buffer, MSG_LENGTH, log_file) != NULL) {
        if (count >= start_index) {
            strncpy(messages[index], buffer, MSG_LENGTH);
            index++;
        }
        count++;
    }
    fclose(log_file);
}
int handle_client(int client_socket) {
    char buffer[MSG_LENGTH] = {0};
    int n;
    read(client_socket, buffer, MSG_LENGTH);
    printf("buffer  %s\n", buffer);
    if (strncmp(buffer, "exit", 3) == 0){
       return -1;
    }
    if (strncmp(buffer, "GET", 3) == 0) {
        printf("buffer  %s\n", buffer);
     //   snprintf();
        int num_logs = atoi(buffer + 4);
        if (num_logs > 0) {
            int total_entries = count_log_entries();
            num_logs = (num_logs < total_entries) ? num_logs : total_entries;

            char messages[num_logs][MSG_LENGTH];
            read_last_n_messages(num_logs, messages);
            for (int i = 0; i < num_logs; i++) {
                send(client_socket, messages[i], MSG_LENGTH, 0);
            }
        }
    } else {
        write_to_log(buffer);
    }
    close(client_socket);
    return 0;
}

int main() {
    int server_fd, client_socket,status;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("listen\n");
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
       status= handle_client(client_socket);
       if (status==-1)
       {
        break;
       }
    }


    return 0;
}
