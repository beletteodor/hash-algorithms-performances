# create a RAM cache and mount it in the 'cache' directory
# mkdir cache
# sudo mount -o size=500M -t tmpfs none cache

# copy the files to the RAM cache for faster I/O access
# cp words.txt cache

# compile
# make

# run
export NUM_CPU=$(cat /proc/cpuinfo | grep -c processor)
./md5_words cache/words.txt cache/md5_words.txt cache/md5_words.dat $NUM_CPU
./md5_numbers cache/md5_numbers.txt cache/md5_numbers.dat $NUM_CPU
./djb2_words cache/words.txt cache/djb2_words.txt cache/djb2_words.dat $NUM_CPU 2> /dev/null
./djb2_numbers cache/djb2_numbers.txt cache/djb2_numbers.dat $NUM_CPU 2> /dev/null

# plot
gnuplot -e "input_file='cache/md5_numbers.dat'; output_file='cache/md5_numbers.png'" plot.gnu
gnuplot -e "input_file='cache/md5_words.dat'; output_file='cache/md5_words.png'" plot.gnu
gnuplot -e "input_file='cache/djb2_words.dat'; output_file='cache/djb2_words.png'" plot.gnu
gnuplot -e "input_file='cache/djb2_numbers.dat'; output_file='cache/djb2_numbers.png'" plot.gnu

# cleanup
# make clean
# sudo umount cache
# rmdir cache

