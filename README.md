# Przykład użycia
``` bash
make

# terminal 0
./target/serwer 1 2 3 # aż do N

# terminal N gdzie N jest dodatnią całkowitą mniejszą niż __LONG_MAX__
./target/klient N

# na koniec aby usunąć binaries z ./target/
make clean
```