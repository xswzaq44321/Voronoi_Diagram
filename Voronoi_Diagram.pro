QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
	dialog/newmap/newmapdialog.cpp \
	geometry/edge.cpp \
	geometry/point.cpp \
	geometry/polygon.cpp \
	geometry/rectangle.cpp \
	main.cpp \
	mainwindow.cpp \
	mywidget/clickgraphicsscene.cpp \
	mywidget/myqgraphicsellipseitem.cpp \
	voronoi/sweepline.cpp \
	voronoi/voronoi.cpp

HEADERS += \
	data_structure/selectivepriorityqueue.h \
	dialog/newmap/newmapdialog.h \
	geometry/edge.h \
	geometry/point.h \
	geometry/polygon.h \
	geometry/rectangle.h \
	mainwindow.h \
	mywidget/clickgraphicsscene.h \
	mywidget/myqgraphicsellipseitem.h \
	voronoi/sweepline.h \
	voronoi/voronoi.h

FORMS += \
	dialog/newmap/newmapdialog.ui \
	mainwindow.ui

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
