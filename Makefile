COMPILER = g++                                                                            # compiler

SOURCE = main.cpp prompt.cpp token.cpp command.cpp special.cpp seperate.cpp               # files to be compiled
OBJECT = $(SOURCE: .cpp = .o)                                                             # create object files with name = source.o

FINAL = myShell                                                                           # name of final executable file

$(FINAL) : $(OBJECT)                                                                      # build final executable
	$(COMPILER) -o $@ $(OBJECT)

%.o : %.cpp	                                                                              # compile each .cpp file into .o file
	$(COMPILER) -c $< -o $@

#clean: rm -f $(FINAL) $(OBJECT)                                                          # removes all the object and executable files 