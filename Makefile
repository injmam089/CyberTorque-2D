CXX = g++
CXXFLAGS = -O3 -std=c++14 -Wall
LDFLAGS = -lgdi32 -lmsimg32 -lwinmm -lgdiplus -mwindows

SRCS = AudioSynth.cpp ParticleSystem.cpp Road.cpp Car.cpp Traffic.cpp Renderer.cpp Game.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = CyberTorque.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q $(OBJS) $(TARGET)
