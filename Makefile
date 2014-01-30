HASH = \
	md5_words \
	md5_numbers \
	djb2_words \
	djb2_numbers \
	sdbm_words \
	sdbm_numbers \
	lose_words \
	lose_numbers \
	murmur2_words \
	murmur2_numbers \
	fnv1_words \
	fnv1_numbers

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
	
sdbm_words: sdbm_words.cpp
		$(CC) $(CFLAGS) $^ -o $@ -lcrypto -lpthread

sdbm_numbers: sdbm_numbers.cpp
		$(CC) $(CFLAGS) $^ -o $@ -lcrypto -lpthread

lose_words: lose_words.cpp
		$(CC) $(CFLAGS) $^ -o $@ -lpthread

lose_numbers: lose_numbers.cpp
		$(CC) $(CFLAGS) $^ -o $@ -lpthread
		
fnv1_words: fnv1_words.cpp
	$(CC) $(CFLAGS) $^ -o $@ -lcrypto -lpthread

fnv1_numbers: fnv1_numbers.cpp
	$(CC) $(CFLAGS) $^ -o $@ -lcrypto -lpthread
	
murmur2_words: murmur2_words.cpp
		$(CC) $(CFLAGS) $^ -o $@ -lcrypto -lpthread

murmur2_numbers: murmur2_numbers.cpp
		$(CC) $(CFLAGS) $^ -o $@ -lcrypto -lpthread

.PHONY: clean

clean:
	rm -f $(HASH)

