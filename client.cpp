#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

const char* KEY_PATH = "key";
const size_t MAX_MESSAGE_SIZE = sizeof(long);

struct MessageBuffer {
    long mtype;
    long sender_id;
    char mtext[MAX_MESSAGE_SIZE];
};

// klient id_klienta
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Błąd: należy podać jedno id klienta\n";
        return EXIT_FAILURE;
    }

    FILE* f = fopen(KEY_PATH, "r");
    if (f == NULL) {
        std::cerr << "Błąd: plik klucza nie istnieje lub nie może zostać otwarty\n";
        return EXIT_FAILURE;
    }
    fclose(f);

    key_t key = ftok(KEY_PATH, 11);
    if (key == -1) {
        std::cerr << "Błąd: nie udało się wygenerować klucza\n";
        return EXIT_FAILURE;
    }

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        std::cerr << "Błąd: nie udało się połączyć z kolejką\n";
        return EXIT_FAILURE;
    }

    if (fork() == 0) {
        MessageBuffer msg;
        while (true) {
            sleep(1);
            int receive_result = msgrcv(msgid, &msg, sizeof(MessageBuffer) - sizeof(long), atoi(argv[1]), IPC_NOWAIT | MSG_NOERROR);
            if (receive_result != -1) {
                std::cout << msg.sender_id << ": " << msg.mtext << "\n";
            }
        }
        return EXIT_SUCCESS;
    }
    
    MessageBuffer msg;
    msg.mtype = __LONG_MAX__;
    msg.sender_id = atoi(argv[1]);
    while (true) {
        std::cin >> msg.mtext;
        int send_result = msgsnd(msgid, &msg, sizeof(MessageBuffer) - sizeof(long), 0);
        if (send_result == -1) {
            std::cout << "Ostrzeżenie: Nie udało się wysłać wiadomości.";
        }
    }

    return EXIT_SUCCESS;
}