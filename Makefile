#############################################################################
# File name:  Makefile
# Author:     chadd williams
# Date:       Sept 1, 2021
# Class:      CS360
# Assignment: MathPacket
# Purpose:    
#############################################################################

# https://www.gnu.org/software/make/manual/html_node/Special-Targets.html
# https://www.gnu.org/software/make/manual/html_node/Chained-Rules.html#Chained-Rules
.SECONDARY:

all: bin bin/mathPacket_client bin/mathPacket_server bin/mathPacket_errors

bin:
	mkdir -p bin

# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
# https://riptutorial.com/makefile/example/21469/automatic-variables

bin/%: bin/%.o
	gcc -o $@ -g -Wall $^

bin/%.o: bin src/%.c
	gcc -c -o $@ -g -Wall $^

valgrind_client: bin/mathPacket_client
	valgrind -v --leak-check=yes --track-origins=yes --leak-check=full --show-leak-kinds=all bin/mathPacket_client 127.0.0.1 8080 1 + 2

valgrind_server: bin/mathPacket_server
	valgrind -v --leak-check=yes --track-origins=yes --leak-check=full --show-leak-kinds=all bin/mathPacket_server 8080

clean:
	rm -f bin/*

printClient: bin
	enscript -C -T 2 -p - -M Letter -Ec --color -fCourier8 src/mathPacket_client.c  | ps2pdf - bin/mathPacket_client.pdf

printServer: bin
	enscript -C -T 2 -p - -M Letter -Ec --color -fCourier8 src/mathPacket_server.c  | ps2pdf - bin/mathPacket_server.pdf

printErrors: bin
	enscript -C -T 2 -p - -M Letter -Ec --color -fCourier8 src/mathPacket_errors.c  | ps2pdf - bin/mathPacket_errors.pdf
