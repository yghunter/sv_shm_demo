# sv_shm_demo
system V shared memory demo in linux

# compile  
gcc svshm_reader.c binary_sems.c -o reader_shm  

gcc sv_shm_writer.c binary_sems.c -o writer_shm  


# test

## write the content of test.txt to shared memory background
./writer_shm < test.txt &

## read from shared memory to out.txt on disk
./reader_shm  > out.txt

## check, compare the sourch file with the output file
diff test.txt out.txt
