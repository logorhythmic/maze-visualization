clean:
	rm -rf build/

run:
	cmake --build build && ./build/Maze-Vis

san:
	cmake -B build -DENABLE_ASAN=ON && cmake --build build && ./build/Maze-Vis

