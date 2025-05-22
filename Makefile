CXX = g++
TARGET = elevator
OBJS = main.o peoplelist.o eventlist.o levelqueue.o func.o animation.o
CXXFLAGS = -std=gnu++11 -Wall -O2 -g

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)
	
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(TARGET)
