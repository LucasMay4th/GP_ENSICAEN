# ENSICAEN
# 6 Boulevard Marechal Juin
# F-14050 Caen Cedex
#
# This file is owned by ENSICAEN.
# No portion of this document may be reproduced, copied
# or revised without written permission of the authors.
#
# @author Lucas Brunet <lucas.brunet251@ecole.ensicaen.fr>
# @author Axel Favreau-Perfetti <axel.favreau-perfetti@ecole.ensicaen.fr>
# @version 0.1.1- 2025/10/03
# 
#
# @file Makefile
# Configuration de la compilation en norme ANSI stricte pour le challenge de F1.

CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra -g -I./Include -I./structure_de_base/priority_queue
LDFLAGS = -lm

EXEC = bignyahoomobile67

SRC = BigNyahoumobile67.c \
      src/map_manager.c \
      src/physics.c \
      src/ai_decision.c \
      structure_de_base/priority_queue/priority_queue.c \
      ../follow_line/follow_line.c

OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

distclean: clean
	rm -f $(EXEC)