## How to create an eclipse project
##### a) creating makefiles and for eclipse IDE

```
cd examples/step-2
make distclean
cmake -DCMAKE_ECLIPSE_VERSION=4.14 -DDEAL_II_DIR=/home/celia/MHPC/dealii/EXE/lib/cmake/deal.II/ -G"Eclipse CDT4 - Unix Makefiles" .

```

##### b) create the Makefile project with existing code

![create_project](create_project.png)

##### c) add path to deal II include directory 

![add_path](add_path.png)

##### d) configure eclipse to build the executable of the project

![configure_build](configure_build.png)

Then, inside the same Build Targets, click on `Build` to create the first executable, to be referenced in next step.

##### e) configure eclipse to run the executable of the project

![configure_run](configure_run.png)

