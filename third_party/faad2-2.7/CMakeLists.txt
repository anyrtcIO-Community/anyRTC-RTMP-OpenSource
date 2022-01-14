cmake_minimum_required(VERSION 3.10.2)

add_library(
        faad STATIC
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/bits.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/cfft.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/decoder.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/drc.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/drm_dec.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/error.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/filtbank.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/ic_predict.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/is.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/lt_predict.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/mdct.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/mp4.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/ms.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/output.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/pns.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/ps_dec.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/ps_syntax.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/pulse.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/specrec.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/syntax.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/tns.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/hcr.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/huffman.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/rvlc.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/ssr.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/ssr_fb.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/ssr_ipqf.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/common.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_dct.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_e_nf.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_fbt.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_hfadj.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_hfgen.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_huff.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_qmf.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_syntax.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_tf_grid.c
        ${CMAKE_HOME_DIRECTORY}/third_party/faad2-2.7/libfaad/sbr_dec.c

)
target_include_directories(faad PUBLIC ${CMAKE_SOURCE_DIR}/third_party/faad2-2.7/include/)
target_include_directories(faad PUBLIC ${CMAKE_SOURCE_DIR}/third_party/faad2-2.7/libfaad/)
add_definitions(-DFAAD_HAVE_CONFIG_H)