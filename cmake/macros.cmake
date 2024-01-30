add_custom_target(LLFileGeneration ALL)

function(generate_ll_file)
  set(options TBAA MEM2REG SROA DEBUG O1 O2 O3)
  set(onearg FILE DEPENDENT)
  set(multiarg INCLUDE)
  cmake_parse_arguments(GEN_LL "${options}" "${onearg}" "${multiarg}" ${ARGN})

  set(DEPENDENT LLFileGeneration)
  if(GEN_LL_DEPENDENT)
    set(DEPENDENT ${GEN_LL_DEPENDENT})
  endif()

  # get file extension
  get_filename_component(test_code_file_ext ${GEN_LL_FILE} EXT)
  string(REPLACE "." "_" ll_file_suffix ${test_code_file_ext})

  # define compilation flags
  set(GEN_CXX_FLAGS -std=c++17 -fno-discard-value-names -emit-llvm -S -w)
  set(GEN_C_FLAGS -fno-discard-value-names -emit-llvm -S -w)
  set(GEN_CMD_COMMENT "[LL]")

  foreach(include_dir ${GEN_LL_INCLUDE})
    list(APPEND GEN_CXX_FLAGS -I ${include_dir})
    list(APPEND GEN_C_FLAGS -I ${include_dir})
  endforeach()

  if(GEN_LL_MEM2REG)
    list(APPEND GEN_CXX_FLAGS -Xclang -disable-O0-optnone)
    list(APPEND GEN_C_FLAGS -Xclang -disable-O0-optnone)
    set(GEN_CMD_COMMENT "${GEN_CMD_COMMENT}[M2R]")
    set(OPT_PASSES ${OPT_PASSES} --mem2reg)
    set(ll_file_suffix "${ll_file_suffix}_m2r")
  endif()

  if(GEN_LL_SROA)
    list(APPEND GEN_CXX_FLAGS -Xclang -disable-O0-optnone)
    list(APPEND GEN_C_FLAGS -Xclang -disable-O0-optnone)
    set(GEN_CMD_COMMENT "${GEN_CMD_COMMENT}[SROA]")
    set(OPT_PASSES ${OPT_PASSES} --sroa)
    set(ll_file_suffix "${ll_file_suffix}_sroa")
  endif()

  if(GEN_LL_TBAA)
    list(APPEND GEN_CXX_FLAGS -O1 -Xclang -disable-llvm-optzns -g0)
    list(APPEND GEN_C_FLAGS -O1 -Xclang -disable-llvm-optzns -g0)
    set(GEN_CMD_COMMENT "${GEN_CMD_COMMENT}[TBAA]")
    set(ll_file_suffix "${ll_file_suffix}_tbaa")
  endif()

  if(GEN_LL_DEBUG)
    list(APPEND GEN_CXX_FLAGS -g)
    list(APPEND GEN_C_FLAGS -g)
    set(GEN_CMD_COMMENT "${GEN_CMD_COMMENT}[DBG]")
    set(ll_file_suffix "${ll_file_suffix}_dbg")
  endif()

  if(GEN_LL_O1)
    list(APPEND GEN_CXX_FLAGS -O1)
    list(APPEND GEN_C_FLAGS -O1)
    set(GEN_CMD_COMMENT "${GEN_CMD_COMMENT}[O1]")
    set(ll_file_suffix "${ll_file_suffix}_o1")
  endif()

  if(GEN_LL_O2)
    list(APPEND GEN_CXX_FLAGS -O2)
    list(APPEND GEN_C_FLAGS -O2)
    set(GEN_CMD_COMMENT "${GEN_CMD_COMMENT}[O2]")
    set(ll_file_suffix "${ll_file_suffix}_o2")
  endif()

  if(GEN_LL_03)
    list(APPEND GEN_CXX_FLAGS -O3)
    list(APPEND GEN_C_FLAGS -O3)
    set(GEN_CMD_COMMENT "${GEN_CMD_COMMENT}[O3]")
    set(ll_file_suffix "${ll_file_suffix}_o3")
  endif()

  set(GEN_CMD_COMMENT "${GEN_CMD_COMMENT} ${GEN_LL_FILE}")

  # set(ll_file_suffix "${ll_file_suffix}.ll")
  string(REPLACE ${test_code_file_ext}
    "${ll_file_suffix}.ll" test_code_ll_file
    ${GEN_LL_FILE}
  )

  # get file path
  set(test_code_file_path "${CMAKE_CURRENT_SOURCE_DIR}/${GEN_LL_FILE}")

  # define custom target name
  # target name = parentdir + test code file name + mem2reg + debug
  get_filename_component(parent_dir ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  get_filename_component(test_code_file_name ${GEN_LL_FILE} NAME_WE)
  set(test_code_file_target "${parent_dir}_${test_code_file_name}${ll_file_suffix}")

  # define .ll file generation command
  if(${test_code_file_ext} STREQUAL ".cpp")
    set(GEN_CMD ${CMAKE_CXX_COMPILER_LAUNCHER} ${CMAKE_CXX_COMPILER})
    list(APPEND GEN_CMD ${GEN_CXX_FLAGS})
  else()
    set(GEN_CMD ${CMAKE_C_COMPILER_LAUNCHER} ${CMAKE_C_COMPILER})
    list(APPEND GEN_CMD ${GEN_C_FLAGS})
  endif()

  message("Setup IR Generation for: ${test_code_ll_file}")

  if(OPT_PASSES)
    add_custom_command(
      OUTPUT ${test_code_ll_file}
      COMMAND ${GEN_CMD} ${test_code_file_path} -o ${test_code_ll_file}
      COMMAND ${CMAKE_CXX_COMPILER_LAUNCHER} opt ${OPT_PASSES} -S ${test_code_ll_file} -o ${test_code_ll_file}
      COMMENT ${GEN_CMD_COMMENT}
      DEPENDS ${GEN_LL_FILE}
      VERBATIM
    )
  else()
    add_custom_command(
      OUTPUT ${test_code_ll_file}
      COMMAND ${GEN_CMD} ${test_code_file_path} -o ${test_code_ll_file}
      COMMENT ${GEN_CMD_COMMENT}
      DEPENDS ${GEN_LL_FILE}
      VERBATIM
    )
  endif()

  add_custom_target(${test_code_file_target}
    DEPENDS ${test_code_ll_file}
  )
  add_dependencies(${DEPENDENT} ${test_code_file_target})
endfunction()

macro(process_all_files)
  file(GLOB SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c *.cpp)

  foreach(file ${SRC_FILES})
      generate_ll_file(FILE ${file} TBAA)
      get_filename_component(test ${file} NAME_WE)
      add_executable(${test} ${file})
  endforeach()
endmacro()
