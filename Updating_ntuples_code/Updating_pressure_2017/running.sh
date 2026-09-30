g++ -I $ROOTSYS/include main.C pressurecsc_2017.cpp  `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable
