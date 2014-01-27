set terminal png nocrop enhanced size 480,480
set output output_file
unset key
unset tics
plot input_file with dots

