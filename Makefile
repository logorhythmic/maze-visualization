
run:
	cmake -B build && cmake --build build && ./build/Maze-Vis

clean:
	rm -rf build/


san:
	cmake -B build -DENABLE_ASAN=ON && cmake --build build && ./build/Maze-Vis

