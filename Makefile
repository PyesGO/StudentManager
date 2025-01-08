CC = gcc
W32CC = x86_64-w64-mingw32-gcc

CFLAGS += -Wall
MAKEFLAGS += -rR

PHONY += __all
__all: a.out

a.out: *.c
	$(CC) $(CFLAGS) -o $@ $^

PHONY += windows
windows: StudentManager.exe StudentManager-GBK.exe

StudentManager.exe: *.c
	$(W32CC) $(CFLAGS) -o $@ $^

StudentManager-GBK.exe: *.c
	$(W32CC) $(CFLAGS) -fexec-charset=GBK -o $@ $^

PHONY += clean
clean:
	rm -f *.out

PHONY += clean_windows
clean_windows:
	rm -f *.exe

.PHONY: $(PHONY)
