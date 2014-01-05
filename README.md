hash-algorithms-performances
============================

Masurarea performantelor algoritmilor de hashing

1. Run example

# create a RAM cache and mount it in the 'cache' directory
mkdir cache
sudo mount -o size=100M -t tmpfs none cache
# copy the input to the RAM cache for faster I/O access
cp words.txt cache

# run the script
./generate_hash.sh cache/words.txt cache/words_md5.txt md5sum

# check the results
cat cache/words_md5.tx

