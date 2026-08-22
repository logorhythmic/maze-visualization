
run:
	cmake -B build && cmake --build build && ./build/Maze-Vis

san:
	cmake -B build -DENABLE_ASAN=ON && cmake --build build && ./build/Maze-Vis

release:
	cmake -B build -DCMAKE_BUILD_TYPE=Release --fresh && cmake --build build && ./build/Maze-Vis

clean:
	rm -rf build/

