CC = gcc
DEPS = helper.h

VPATH = src
OBJDIR = obj

.phony all:
all: diskinfo disklist diskget diskput

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

diskinfo: $(OBJDIR)/diskinfo.o $(OBJDIR)/helper.o
	$(CC) -Wall -pedantic -o $@ $^

disklist: $(OBJDIR)/disklist.o $(OBJDIR)/helper.o
	$(CC) -Wall -pedantic -o $@ $^

diskget: $(OBJDIR)/diskget.o $(OBJDIR)/helper.o
	$(CC) -Wall -pedantic -o $@ $^

diskput: $(OBJDIR)/diskput.o $(OBJDIR)/helper.o
	$(CC) -Wall -pedantic -o $@ $^

.PHONY: clean
clean:
	rm -r diskinfo disklist diskget diskput $(OBJDIR)

$(shell mkdir -p $(OBJDIR))