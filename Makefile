BUILD_DIR = build
SANITIZE_DIR = build-sanitize

test:
	cmake --build $(BUILD_DIR)
	ctest --test-dir $(BUILD_DIR) --output-on-failure

sanitize:
	cmake -S . -B $(SANITIZE_DIR) -DBITFORGE_ENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(SANITIZE_DIR)
	ctest --test-dir $(SANITIZE_DIR) --output-on-failure
