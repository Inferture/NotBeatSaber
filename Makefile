#Enter the correct parameters for your installation of SDL

SDL_INCLUDE = C:\Program Files (x86)\CodeBlocks\SDL2-2.0.14\i686-w64-mingw32\include
SDL_LIB = C:\Program Files (x86)\CodeBlocks\SDL2-2.0.14\i686-w64-mingw32\lib


APP_TITLE := NotBeatSaber
APP_DESCRIPTION := NotBeatSaber
APP_AUTHOR := Inferture
APP_VERSION := 0.1.0


INCS      = -I"$(SDL_INCLUDE)"  -Iinclude/
LINK      = mingw32-g++
CPP       = mingw32-g++

RM        = rm -f

LIBS      = -L"$(SDL_LIB)" -lmingw32 -lSDL2main -lSDL2 -lSDL2.dll -luser32 -lgdi32 -lwinmm -ldxguid -lSDL2_gfx -lSDL2_mixer -lSDL2_ttf 
LDFLAGS   = -static-libgcc -static-libstdc++ -mwindows 

OBJ_FOLDER = ./obj/PC
OBJ       = $(OBJ_FOLDER)/main.o $(OBJ_FOLDER)/Global.o $(OBJ_FOLDER)/Transform.o $(OBJ_FOLDER)/GameObject.o $(OBJ_FOLDER)/ColorUtility.o $(OBJ_FOLDER)/DrawingUtility.o $(OBJ_FOLDER)/Line.o $(OBJ_FOLDER)/Block.o $(OBJ_FOLDER)/Pattern.o
LINKOBJ   = $(OBJ_FOLDER)/main.o $(OBJ_FOLDER)/Global.o $(OBJ_FOLDER)/Transform.o $(OBJ_FOLDER)/GameObject.o $(OBJ_FOLDER)/ColorUtility.o $(OBJ_FOLDER)/DrawingUtility.o $(OBJ_FOLDER)/Line.o $(OBJ_FOLDER)/Block.o $(OBJ_FOLDER)/Pattern.o

CXXFLAGS = $(INCS) -std=c++11

BIN_FOLDER = ./bin/PC
BIN       = $(BIN_FOLDER)/NotBeatSaber.exe

OUTPUT    = NotBeatSaber

.PHONY: all clean all-before all-after cleanobj


all: all-before $(BIN) all-after

all-before:
	echo "$(SDL_INCLUDE)"
	mkdir -p $(OBJ_FOLDER)
	mkdir -p $(BIN_FOLDER)
	cp -rf *.dll $(BIN_FOLDER)
	cp -r -rf ./NotBeatSaber_data $(BIN_FOLDER)

all-after:

cleanobj: 
	$(RM) $(OBJ)

clean:
	$(RM) $(OBJ) $(BIN)


$(BIN): $(LINKOBJ)
	$(LINK) $(LINKOBJ) -o $(BIN) $(LIBS) $(LDFLAGS)


$(OBJ_FOLDER)/Block.o: src/Block.cpp $(OBJ_FOLDER)/Global.o
	$(CPP) -c src/Block.cpp -o $(OBJ_FOLDER)/Block.o $(CXXFLAGS)


$(OBJ_FOLDER)/ColorUtility.o: src/ColorUtility.cpp  $(OBJ_FOLDER)/Global.o
	$(CPP) -c src/ColorUtility.cpp -o $(OBJ_FOLDER)/ColorUtility.o $(CXXFLAGS)


$(OBJ_FOLDER)/DrawingUtility.o: src/DrawingUtility.cpp  $(OBJ_FOLDER)/Global.o
	$(CPP) -c src/DrawingUtility.cpp -o $(OBJ_FOLDER)/DrawingUtility.o $(CXXFLAGS)


$(OBJ_FOLDER)/GameObject.o: src/GameObject.cpp  $(OBJ_FOLDER)/Global.o
	$(CPP) -c src/GameObject.cpp -o $(OBJ_FOLDER)/GameObject.o $(CXXFLAGS)


$(OBJ_FOLDER)/Line.o: src/Line.cpp  $(OBJ_FOLDER)/Global.o
	$(CPP) -c src/Line.cpp -o $(OBJ_FOLDER)/Line.o $(CXXFLAGS)


$(OBJ_FOLDER)/Pattern.o: src/Pattern.cpp  $(OBJ_FOLDER)/Global.o
	$(CPP) -c src/Pattern.cpp -o $(OBJ_FOLDER)/Pattern.o $(CXXFLAGS)


$(OBJ_FOLDER)/Transform.o: src/Transform.cpp $(OBJ_FOLDER)/Global.o
	$(CPP) -c src/Transform.cpp -o $(OBJ_FOLDER)/Transform.o $(CXXFLAGS)

$(OBJ_FOLDER)/main.o: src/main.cpp src/Global.cpp $(OBJ_FOLDER)/Global.o
	$(CPP) -c src/main.cpp  -o $(OBJ_FOLDER)/main.o $(CXXFLAGS)

$(OBJ_FOLDER)/Global.o:
	$(CPP) -c src/Global.cpp -o $(OBJ_FOLDER)/Global.o $(CXXFLAGS)

