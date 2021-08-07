TARGET=ItoA
all: default

default:
	gcc src/*.c -lfreeimage -o $(TARGET)


debug:
	gcc -D DEBUG src/*.c -lfreeimage -o $(TARGET)