BUILD_DIR = build
SANITIZE_DIR = build-sanitize

EXTERNAL_TEST_DIR = example/external_test
EXTERNAL_INSTALL_DIR = /tmp/bitforge-external-install
EXTERNAL_BUILD_DIR = /tmp/bitforge-external-build

.PHONY: external-test
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

check: test sanitize cpp-check format-check

format:
	find src include tests example -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -i {} +

format-check:
	find src include tests example -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format --dry-run --Werror {} +

clean:
	rm -rf $(BUILD_DIR) $(SANITIZE_DIR) build-docs

docs:
	doxygen Doxyfile
	mkdir -p docs
	moxygen --groups --output docs/%s.md build-docs/xml
	rm -f docs/api.md

cpp-check:
	cppcheck --enable=warning,style,performance,portability --error-exitcode=1 --std=c17 --language=c --suppress=constVariable -I include src/ include/ tests/

external-test:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) --parallel
	cmake --install $(BUILD_DIR) --prefix $(EXTERNAL_INSTALL_DIR)
	cmake -S $(EXTERNAL_TEST_DIR) -B $(EXTERNAL_BUILD_DIR) -DCMAKE_PREFIX_PATH=$(EXTERNAL_INSTALL_DIR)
	cmake --build $(EXTERNAL_BUILD_DIR) --parallel
	$(EXTERNAL_BUILD_DIR)/external_test
	rm -rf $(EXTERNAL_INSTALL_DIR) $(EXTERNAL_BUILD_DIR)

.PHONY: docs
