CFLAGS := $(CFLAGS) -Iapi -Iinc -Isrc -Itest -lm -Wall -Wextra -Wpedantic -fsanitize=address -g --std=c99

DIST_DIR := dist
DIST_INC_DIR := $(DIST_DIR)/inc
TEST_BUILD_DIR := build/test
DATASTRUCTURES := array array_list red_black_tree hash_table
TEST_BINS := $(addprefix $(TEST_BUILD_DIR)/test_,$(DATASTRUCTURES))
DIST_INC_FILES := $(addprefix $(DIST_INC_DIR)/,$(addsuffix .t.h,$(DATASTRUCTURES)) coln_result.h)

all: check copy_inc

.PHONY: all check copy_inc

check: $(TEST_BINS)
		@echo "Running tests..."
		@for test in $(TEST_BINS); do \
				echo "Running $$test..."; \
				$$test || exit 1; \
		done
		@echo "All tests passed."
$(TEST_BUILD_DIR)/test_%: test/test_%.c api/%.t.h
		@mkdir -p $(dir $@)
		$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

copy_inc: $(DIST_INC_FILES)
$(DIST_INC_DIR)/%: api/%
		@mkdir -p $(dir $@)
		@cp $< $@

clean: 
		@rm -rf build
		@rm -rf dist
