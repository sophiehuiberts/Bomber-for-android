project(bomber)
include_directories(${CMAKE_SOURCE_DIR}/libkdegames/highscore)

if(KDE4_ENABLE_FPIE)
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_CXX_FPIE_FLAGS}")
endif(KDE4_ENABLE_FPIE)

set(bomber_SRCS
   bomber.cpp
   main.cpp
   bomberwidget.cpp   
   board.cpp
   plane.cpp
   bomb.cpp
   building.cpp 
   explodable.cpp
 )

kde4_add_kcfg_files(bomber_SRCS settings.kcfgc )
kde4_add_app_icon(bomber_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pictures/hi*-app-bomber.png"})
kde4_add_executable(bomber ${bomber_SRCS})

target_link_libraries(bomber kdegames ${KDE4_KDEUI_LIBS} ${KDE4_PHONON_LIBS})
if(KDE4_ENABLE_FPIE)
    target_link_libraries(bomber ${KDE4_PIE_LDFLAGS})    
endif(KDE4_ENABLE_FPIE)

install(TARGETS bomber ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS bomber.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES bomber.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES bomberui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bomber )

