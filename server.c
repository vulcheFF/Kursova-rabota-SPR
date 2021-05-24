#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8080

typedef enum vid {
    TOK,
    VODA,
    INTERNET,
    TELEVIZIQ,
    HRANA,
    KOLA,
    DANUCI,
    PARNO,
    DECA,
    NAEM
} vid;

typedef struct Razhod {
    vid vid;
    float suma;
    int d, m, y;

} Razhod;

int main()
{

    int serv_fd, n_socket, v_read;
    struct sockaddr_in address;

    int opt = 1;
    int addr_length = sizeof(address);

    if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failure!!!");
        exit(0);
    }

    if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt error!!!");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(serv_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Binding failed!!!");
        exit(2);
    }

    if (listen(serv_fd, 3) < 0) {
        perror("Listen error!!!");
        exit(3);
    }

    if ((n_socket = accept(serv_fd, (struct sockaddr*)&address, (socklen_t*)&addr_length)) < 0) {
        perror("Accept error");
        exit(4);
    }

    Razhod razhod;

    FILE* fp;

    float sum = 0, max = 0, fd = 0;
    int year, month, n = 0, countS = 0;

    while (1) {

        int choice = -1;

        if ((v_read = read(n_socket, &choice, sizeof(int))) < 0) {
            perror("Read error!\n");
            exit(5);
        }

        switch (choice) {

        case 1:

            if ((v_read = read(n_socket, &razhod, sizeof(Razhod))) < 0) {
                perror("Read error!\n");
                exit(6);
            }

            fd = open("t.txt", O_WRONLY | O_CREAT | O_APPEND, 0777);
            if (fd < 0) {
                perror("Error opening file \n");
                exit(7);
            }

            if (write(fd, &razhod, sizeof(Razhod)) < 0) {
                perror("Writing failed!\n");
                exit(8);
            }

            close(fd);

            break;

        case 2:
            Razhod r;

            fd = open("t.txt", O_RDONLY, 0777);
            if (fd < 0) {
                perror("Error opening file for R\n");
                exit(9);
            }

            if ((v_read = read(n_socket, &month, sizeof(int))) < 0) {
                perror("Read error!\n");
                exit(10);
            }

            if ((v_read = read(n_socket, &year, sizeof(int))) < 0) {
                perror("Error reading");
                exit(11);
            }

            while ((n = read(fd, &r, sizeof(Razhod))) > 0) {
                if (month == r.m && year == r.y) {

                    sum += r.suma;
                }
            }

            //send sum
            if ((send(n_socket, &sum, sizeof(float), 0)) < 0) {
                perror("Error sending data!\n");
                exit(12);
            }
            else {
                printf("Monthly expense for month: %d and year: %d is:%.2f lv\n", month, year, sum);
            }

            sum = 0;

            close(fd);
            break;
        case 3:

            fd = open("t.txt", O_RDONLY, 0777);
            if (fd < 0) {
                perror("Error opening file for reading\n");
                exit(13);
            }

            Razhod x;
            while ((n = read(fd, &x, sizeof(Razhod))) > 0) {
                if (x.suma > max) {
                    max = x.suma;
                }
            }

            lseek(fd, 0, SEEK_SET);
            while ((n = read(fd, &x, sizeof(Razhod))) > 0) {
                if (max == x.suma) {
                    printf("Biggest expense ever made is type:%u amount:%.2flv on date:%d/%d/%d\n", x.vid, x.suma, x.d, x.m, x.y);
                    if ((send(n_socket, &x, sizeof(Razhod), 0)) < 0) {
                        perror("Error sending!\n");
                        exit(14);
                    }
                }
            }

            close(fd);
            break;
        case 4:
            fd = open("t.txt", O_RDONLY, 0777);
            if (fd < 0) {
                perror("Error opening file for reading\n");
                exit(15);
            }

            Razhod sen;

            while ((n = read(fd, &sen, sizeof(Razhod))) > 0) {
                countS++;
            }

            if ((send(n_socket, &countS, sizeof(int), 0)) < 0) {
                break;
                perror("Error sending data\n");
                exit(16);
            }

            lseek(fd, 0, SEEK_SET);
            while ((n = read(fd, &sen, sizeof(Razhod))) > 0) {
                if ((send(n_socket, &sen, sizeof(Razhod), 0)) < 0) {

                    perror("Error sending data\n");
                    exit(17);
                }
                printf("Montly expense type:%u Amount:%.2flv. Date:%d/%d/%d\n", sen.vid, sen.suma, sen.d, sen.m, sen.y);
            }

            printf("Read structs from file:%d\n", countS);

            countS = 0;
            close(fd);
            break;

        case 5:

            printf("Close! \n");
            close(serv_fd);
            break;
        }
        if (choice == 5) {
            break;
        }
    }

    return 0;
}
