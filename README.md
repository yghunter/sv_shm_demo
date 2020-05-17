# sv_shm_demo
system V share memory demo in linux

# compile
gcc svshm_xfr_writer.c binary_sems.c -o writer_shm
gcc sv_shm_writer.c binary_sems.c -o writer_shm

# test

# write the content of test.txt to share memory
./writer_shm < test.txt &

# read from share memory to out.txt on disk
./reader_shm  > out.txt

# compare the sourch file and output file
diff test.txt out.tx
