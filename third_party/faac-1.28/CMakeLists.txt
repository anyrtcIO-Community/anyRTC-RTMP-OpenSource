cmake_minimum_required(VERSION 3.10.2)
set(FAAC_DIR ./../../../../..)

add_library(
        faac STATIC
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/aacquant.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/bitstream.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/fft.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/frame.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/midside.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/psychkni.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/util.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/backpred.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/channels.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/filtbank.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/huffman.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/ltp.c
        ${FAAC_DIR}/third_party/faac-1.28/libfaac/tns.c
)
target_include_directories(faac PUBLIC ${FAAC_DIR}/third_party/faac-1.28/include/)