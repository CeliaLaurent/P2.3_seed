



```bash

```



```bash
myserver$ docker pull dealii/dealii:latest

myserver$ cd WorkDir
myserver$ sudo docker run  -t -i -v `pwd`:/home/dealii dealii/dealii:latest
myserver$ export PATH=$(spack location -i mpich)/bin:$PATH  # gives access to mpirun command
 
myserver$ cd CompilingDir
myserver$ cmake .

myserver$ make release
myserver$ make
```

