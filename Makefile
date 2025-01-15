CXX = g++ 
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -Iincludes -Werror -Wno-sign-compare 
LDFLAGS = -static-libstdc++ -static-libgcc -lpci
SOURCES = telemetry.cpp includes/board.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = telemetry.exe
#//-D_GLIBCXX_USE_CXX11_ABI=0
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
