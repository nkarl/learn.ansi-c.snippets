##################################################################
## Project Variables
##################################################################
HDR			:= ${CWD}/src/hdr
SRC			:= ${CWD}/src
BUILD			:= ${CWD}/build

## BUILD MAIN APP ################################################
SRC_MAIN		:= ${SRC}/main
BUILD_MAIN		:= ${BUILD}/main
APP			:= ${BUILD_MAIN}/app

## BUILD SNIPPETS ################################################
SRC_SNIPPETS 		:= ${SRC}/snippets
BUILD_SNIPPETS		:= ${BUILD}/snippets

SNIPPETS := ring_buffer.o
SNIPPETS := ${addprefix ${BUILD_SNIPPETS}/, ${SNIPPETS}}

##################################################################
## Compiler Flags
##################################################################
INCLUDE 		:= -I${HDR} -I${SRC_SNIPPETS}
CXX			:= gcc-14.1.0 # run make CXX=clang
COMPILER_FLAGS		:= -g \
			   -Wall -Wextra -Wconversion -Wpedantic \
			   ${INCLUDE}
LINKER_FLAGS		:=

##################################################################
## Build Recipes
##################################################################
.PHONY: app	## compile and build main app
app: ${APP}

${APP}: ${SNIPPETS} ${APP}.o
	${CXX} ${LINKER_FLAGS} -o $@ $^

${BUILD_MAIN}/%.o: ${SRC_MAIN}/%.c | setup
	${CXX} ${COMPILER_FLAGS} -o $@ -c $^ 

${BUILD_SNIPPETS}/%.o: ${SRC_SNIPPETS}/%.c | setup
	${CXX} ${COMPILER_FLAGS} -o $@ -c $^

.PHONY: setup
setup: 		## set up the build directories
	$(info $(shell mkdir -p ${BUILD_MAIN} ${BUILD_SNIPPETS}))

##################################################################
## CLEAN UP
##################################################################
.PHONY: clean
clean:
	rm -r ${CWD}/build/

