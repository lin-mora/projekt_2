#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

const char* KEY_PATH = "key";
const size_t MAX_MESSAGE_SIZE = sizeof(long); // min is sizeof(long) cause of padding

struct MessageBuffer {
    long mtype;
    long sender_id;
    char mtext[MAX_MESSAGE_SIZE];
};

// serwer id_klienta1 id_klienta2 ...
int main(int argc, char* argv[]) {

    if (argc == 1) {
        std::cerr << "Błąd: nie podano żadnego id klienta\n";
        return EXIT_FAILURE;
    }

    // The fopen() function opens the file whose name is the string pointed to by pathname and associates a stream with it.
    FILE* f = fopen(KEY_PATH, "r");
    if (f == NULL) {
        std::cerr << "Błąd: plik klucza nie istnieje lub nie może zostać otwarty\n";
        return EXIT_FAILURE;
    }
    // The fclose() function flushes the stream pointed to by stream (writing any buffered output data using fflush(3)) and closes the underlying file descriptor.
    fclose(f);

    // The  ftok()  function  uses  the identity of the file named by the given pathname (which must refer to an existing, accessible file)
    // and the least significant 8 bits of proj_id (which must be nonzero) to generate a key_t type System V IPC key, 
    // suitable for use with msgget(2), semget(2), or shmget(2).
    key_t key = ftok(KEY_PATH, 11);
    if (key == -1) {
        std::cerr << "Błąd: nie udało się wygenerować klucza\n";
        return EXIT_FAILURE;
    }   

    // The  msgget()  system call returns the System V message queue identifier associated with the value of the key argument.
    // It may be used either to obtain the identifier of a previously created message queue
    // (when msgflg is zero and key does not have the value IPC_PRIVATE), or to create a new set.
    // OCT 0666 == BIN 110 110 110 == rw-|rw-|rw- // owner|group|others
    // #define IPC_CREAT 01000
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        std::cerr << "Błąd: nie udało się wygenerować kolejki\n";
        return EXIT_FAILURE;
    }

    std::cout << "Utworzono kolejkę wiadomości " << msgid << "\n";

    MessageBuffer msg;
    // the server will run in perpetuity but that's acceptable under the conditions of the assignment
    while (true) {
        // MSG_NOERROR - To truncate the message text if longer than msgsz bytes.
        if (msgrcv(msgid, &msg, sizeof(MessageBuffer) - sizeof(long), __LONG_MAX__, MSG_NOERROR) != -1) {
            std::cout << msg.sender_id << ": " << msg.mtext << "\n";
            for (int i = 1; i < argc; i++) {
                msg.mtype = atoi(argv[i]);
                if (msg.mtype == msg.sender_id) {
                    continue;
                }
                int send_result = msgsnd(msgid, &msg, sizeof(MessageBuffer) - sizeof(long), 0);
                if (send_result == -1) {
                    std::cerr << "Ostrzeżenie: nie udało się wysłać wiadomości od \"" << msg.sender_id << "\" do \"" << msg.mtype << "\n";
                }
            }
        }
    }
    

    // int msgctl(int msqid, int cmd, struct msqid_ds *buf);
    // msgctl() performs the control operation specified by cmd on the System V message queue with identifier msqid.
    // IPC_RMID - Immediately remove the message queue, awakening all waiting reader and writer processes
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        std::cerr << "Błąd: nie udało się usunąć kolejki\n";
        return EXIT_FAILURE;
    }

    std::cout << "Usunięto kolejkę wiadomości " << msgid << "\n";
    return EXIT_SUCCESS;
}
