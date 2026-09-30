g++ -I $ROOTSYS/include main.C tree_class.C `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable
