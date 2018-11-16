include_directories(
${ROOT_INCLUDE_DIRS}
${CMAKE_CURRENT_SOURCE_DIR}
)
STRING(REGEX REPLACE "Tree" "EG" EG_LIB ${ROOT_Tree_LIBRARY})
set(SRC
UParticle.cxx 
UEvent.cxx 
U2U.cxx 
U2A.cxx
UrQMDCall.cxx
UPdgConvert.cxx
UMerger.cxx
UItyp.cxx
UFile.cxx
UDecayChannel.cxx
UDecayParticle.cxx
)

MAKEHEADERS(HEADERS "${SRC}")

ROOT_GENERATE_DICTIONARY(G__UbootDict 
"${HEADERS}"
LINKDEF UbootLinkDef.h)

add_library(Uboot SHARED ${SRC} G__UbootDict.cxx)
target_link_libraries(Uboot ${ROOT_LIBRARIES} ${EG_LIB})

add_executable(U2boot Main.cpp)
target_link_libraries(U2boot Uboot)
install(TARGETS Uboot DESTINATION lib)
install(TARGETS U2boot DESTINATION bin)