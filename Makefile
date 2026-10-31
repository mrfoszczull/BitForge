BUILD_DIR = build
SANITIZE_DIR = build-sanitize

.PHONY: build test sanitize check format format-check clean

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

sanitize:
	cmake -S . -B $(SANITIZE_DIR) -DBITFORGE_ENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(SANITIZE_DIR)
	ctest --test-dir $(SANITIZE_DIR) --output-on-failure

check: test sanitize

format:
	find src tests examples -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -i {} +

format-check:
	find src tests examples -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format --dry-run --Werror {} +

clean:
	$(RM) -r $(BUILD_DIR) $(SANITIZE_DIR)
