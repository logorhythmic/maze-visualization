
PROJ_NAME := Maze-Vis
.PHONY: run san web web-run

run:
	cmake -B build && cmake --build build && ./build/$(PROJ_NAME)

san:
	cmake -B build -DENABLE_ASAN=ON && cmake --build build && ./build/$(PROJ_NAME)

release:
	cmake -B build -DCMAKE_BUILD_TYPE=Release --fresh && cmake --build build && ./build/$(PROJ_NAME)

web:
	emcmake cmake -B build-web/ && cmake --build build-web

web-run:
	source /etc/profile.d/emscripten.sh && emrun build-web/Maze-Vis.html

clean:
	rm -rf build/

