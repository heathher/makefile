include config.mk

#
#
#
#
#
#

OBJS=  dependency_tree.o\
		my_exceptions.o\
		file_stuff.o\
		string_stuff.o\
		template_builder.o\
		make.o\
		argv_parser.o\
		syntax_analyzer.o\
		lex.o

all: m_make
	echo here

m_make: $(OBJS)
	$(CXX) -o $@ $^

%.o : ../%.cpp
	$(CXX) $(CXX_FLAGS) -c $^

clean:
	rm -f $(OBJS) m_make

.PHONY: m_make
	echo 42525
