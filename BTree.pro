TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

HEADERS += \
    btree_node.h \
    btree.h \
    btree_functions.h \
    pair.h \
    map.h \
    dictmap.h \
    mpair.h \
    multimap.h

DISTFILES += \
    work_report.txt \
    output.txt \
    btree \
    README.markdown
