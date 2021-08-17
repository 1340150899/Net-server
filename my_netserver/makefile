DIR_BIN = .
DIR_OBJ = ./bin
DIR_SRC = ./cpp

SRC = $(wildcard $(DIR_SRC)/*.cpp)
OBJ = $(patsubst %.cpp,$(DIR_OBJ)/%.o,$(notdir $(SRC)))


CXX_FLAG = -g -Wall -std=c++11 -pthread -O3
CC = g++

TARGET = net_server

$(DIR_BIN)/$(TARGET) : $(OBJ)
	$(CC) $(CXX_FLAG) -o $@ $^

$(DIR_OBJ)/%.o : $(DIR_SRC)/%.cpp
	if [ ! -d $(DIR_OBJ) ];	then mkdir -p $(DIR_OBJ); fi;
	$(CC) $(CXX_FLAG) -c $< -o $@

clean : 
	-rm -rf $(DIR_OBJ)

.PHONY : clean
