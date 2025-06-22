# === Configuración de directorios ===
SRC_DIR = src
INC_DIR = inc

BUILD_DIR = build
OBJ_DIR = ${BUILD_DIR}/obj
BIN_DIR = ${BUILD_DIR}/bin
DOC_DIR = ${BUILD_DIR}/doc

MODULES = 
BOARD = edu-ciaa-nxp
MUJU = ./muju


include $(MUJU)/module/base/makefile

$(DOC_DIR): | $(BUILD_DIR)
	@mkdir -p $@

doc: | $(DOC_DIR)
	@echo "Generating documentation..."
	@doxygen Doxyfile