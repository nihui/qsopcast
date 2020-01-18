configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(qsopcast_SRCS
    abstractchannel.cpp
    channel_argument.cpp
    channel_bookmark.cpp
    channel_history.cpp
    channel_pplive.cpp
    channel_ppstream.cpp
    channel_sopcast.cpp
    channellistmodel.cpp
    channellistview.cpp
    config.cpp
    filterbar.cpp
    main.cpp
    menubar.cpp
    mystatusbar.cpp
    pageplay.cpp
    pagerecord.cpp
    playfork.cpp
    qsopcast.cpp
    recorder.cpp
    searchedit.cpp
    tabwidget.cpp
    utils.cpp
)

qt4_automoc(${qsopcast_SRCS})

add_executable(qsopcast ${qsopcast_SRCS})

target_link_libraries(qsopcast ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY})

install(TARGETS qsopcast DESTINATION bin)
install(FILES qsopcast.desktop DESTINATION share/applications)

# install icons
install(FILES icons/mozart.png DESTINATION share/pixmaps)
install(FILES icons/mozart.xpm DESTINATION share/pixmaps)
install(FILES icons/sopcast.xpm DESTINATION share/pixmaps)

add_subdirectory(language)
