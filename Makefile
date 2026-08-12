clean:
	rm -rf build/

run:
	cmake --build build && ./build/imgui_test

san:
	cmake -B build -DENABLE_ASAN=ON && cmake --build build && ./build/imgui_test

