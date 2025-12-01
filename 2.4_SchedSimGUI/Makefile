CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude
# Flags para GTK (Necesitas tener GTK3 instalado)
GTK_LIBS = $(shell pkg-config --cflags --libs gtk+-3.0)

TARGET = scheduler_sim
SRCS = src/scheduler.c src/algorithms.c src/metrics.c src/gui_gtk.c src/report.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(GTK_LIBS)

# Reglas de compilación de objetos
src/scheduler.o: src/scheduler.c include/scheduler.h
src/algorithms.o: src/algorithms.c include/algorithms.h
src/metrics.o: src/metrics.c include/metrics.h
src/gui_gtk.o: src/gui_gtk.c include/scheduler.h
src/report.o: src/report.c include/metrics.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f docs/ANALYSIS.pdf
	@echo "Cleaned."
