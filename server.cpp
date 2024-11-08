#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>

const char* KEY_PATH = "key";

// serwer id_klienta1 id_klienta2 ...
int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cerr << "Błąd: nie podano żadnego id klienta\n";
        return EXIT_FAILURE;
    }

    FILE* f = fopen(KEY_PATH, "r");
    if (f == NULL) {
        std::cerr << "Błąd: plik nie istnieje lub nie może zostać otwarty\n";
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
        std::cerr << "Błąd: nie udało się wygenerować kolejki\n";
        return EXIT_FAILURE;
    }

    std::cout << "Utworzono kolejkę wiadomości " << msgid << "\n";

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        std::cerr << "Błąd: nie udało się usunąć kolejki\n";
        return EXIT_FAILURE;
    }

    std::cout << "Usunięto kolejkę wiadomości " << msgid << "\n";
    return EXIT_SUCCESS;
}