HASH = \
	md5_words \
	md5_numbers \
	djb2_words \
	djb2_numbers

CC = g++
CFLAGS = -Wall -Wextra -O3

all: $(HASH)

md5_words: md5_words.cpp
	$(CC) $(CFLAGS) $^ -o $@ -lcrypto -lpthread

md5_numbers: md5_numbers.cpp
	$(CC) $(CFLAGS) $^ -o $@ -lcrypto -lpthread

djb2_words: djb2_words.cpp
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

djb2_numbers: djb2_numbers.cpp
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

.PHONY: clean

clean:
	rm -f $(HASH)

