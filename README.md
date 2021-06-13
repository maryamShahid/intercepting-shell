# Intercepting Shell Program

### Execution:
Find the directory project locates

 `$ make`
 `$ ./isp <N> <mode 1: normal, 2: tapped>`
 
 - N is the number of bytes to read/write in one system call 
 - mode is the mode of the communication to use from normal and tapped

### Running the producer-consumer model:

  - `isp$ ./producer <M> | ./consumer <M>`
