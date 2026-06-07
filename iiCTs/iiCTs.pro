QT       += core gui sql
QT       += charts
QT       += concurrent
QT       += opengl

# 平台特定模块
unix:!macx {
    QT += x11extras
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += exceptions

# 包含路径设置
INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/include/mainui
INCLUDEPATH += $$PWD/include/panel
INCLUDEPATH += $$PWD/include/widget
INCLUDEPATH += $$PWD/include/tgwl

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += SCALE_RATE=0.6
DEFINES += CTISS_USERS_DB_FILENAME=\\\"iiCTs_users.db\\\"


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    source/mainui/chatpanel.cpp \
    source/mainui/paramfilemanager.cpp \
    source/mainui/progressmonitor.cpp \
    source/mainui/changepasswd.cpp \
    source/mainui/commandwriterthread.cpp \
    source/mainui/loginwindow.cpp \
    source/mainui/main.cpp \
    source/mainui/mainwindow.cpp \
    source/mainui/registerwindow.cpp \
    source/mainui/userdb.cpp\
    source/mainui/roundeditemdelegate.cpp \
    source/widget/imagecontainer.cpp \
    source/widget/imagecontainerwidget.cpp

SOURCES += \
    source/panel/modelctreconstruction.cpp \
    source/panel/modelctscansimulation.cpp \
    source/panel/modeldetectormodel.cpp \
    source/panel/modelhome.cpp \
    source/panel/modelspectrumsimulation.cpp \
    source/panel/modeltubesimulation.cpp \
    source/panel/modeluserguide.cpp \
    source/panel/modelversioninformation.cpp \
    source/panel/paractreconstruction.cpp \
    source/panel/paractscansimulation.cpp \
    source/panel/paradetectormodel.cpp \
    source/panel/parahome.cpp \
    source/panel/paraspectrumsimulation.cpp \
    source/panel/paratubesimulation.cpp \
    source/panel/parauserguide.cpp \
    source/panel/paraversioninformation.cpp \
    source/panel/resultctreconstruction.cpp \
    source/panel/resultctscansimulation.cpp \
    source/panel/resultdetectormodel.cpp \
    source/panel/resulthome.cpp \
    source/panel/resulttubesimulation.cpp \
    source/panel/resultspectrumsimulation.cpp \
    source/panel/resultuserguide.cpp \
    source/panel/resultversioninformation.cpp

SOURCES += \
    source/widget/chatagent.cpp \
    source/widget/terminalwidget.cpp \
    source/widget/embed.cpp

SOURCES += \
    source/tgwl/transspectrum.cpp \
    source/tgwl/transcbct.cpp \
    source/tgwl/transhelicalct.cpp \
    source/tgwl/translinearct.cpp \
    source/tgwl/transpolygonintervalct.cpp \
    source/tgwl/transpolygonsidect.cpp

HEADERS += \
    include/mainui/filedialog.h \
    include/mainui/commandwriterthread.h \
    include/mainui/userdb.h \
    include/mainui/changepasswd.h \
    include/mainui/chatpanel.h \
    include/mainui/loginwindow.h \
    include/mainui/mainwindow.h \
    include/mainui/progressmonitor.h \
    include/mainui/paramfilemanager.h \
    include/mainui/registerwindow.h \
    include/mainui/roundeditemdelegate.h \
    include/mainui/projectinfo.h


HEADERS += \
    include/panel/modelctreconstruction.h \
    include/panel/modelctscansimulation.h \
    include/panel/modeldetectormodel.h \
    include/panel/modelhome.h \
    include/panel/modelspectrumsimulation.h \
    include/panel/modeltubesimulation.h \
    include/panel/modeluserguide.h \
    include/panel/modelversioninformation.h \
    include/panel/paractreconstruction.h \
    include/panel/paractscansimulation.h \
    include/panel/paradetectormodel.h \
    include/panel/parahome.h \
    include/panel/paraspectrumsimulation.h \
    include/panel/paratubesimulation.h \
    include/panel/parauserguide.h \
    include/panel/paraversioninformation.h \
    include/panel/resultctreconstruction.h \
    include/panel/resultctscansimulation.h \
    include/panel/resultdetectormodel.h \
    include/panel/resulthome.h \
    include/panel/resulttubesimulation.h \
    include/panel/resultspectrumsimulation.h \
    include/panel/resultuserguide.h \
    include/panel/resultversioninformation.h

HEADERS += \
    include/tgwl/transspectrum.h \
    include/tgwl/transcbct.h \
    include/tgwl/transhelicalct.h \
    include/tgwl/translinearct.h \
    include/tgwl/transpolygonintervalct.h \
    include/tgwl/transpolygonsidect.h

HEADERS += \
    include/widget/terminalwidget.h \
    include/widget/chatagent.h \
    include/widget/imagecontainer.h \
    include/widget/imagecontainerwidget.h \
    include/widget/verticalbutton.h \
    include/widget/verticallabel.h \
    include/widget/embed.h

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    guide/guide.qrc \
    images/images.qrc \
    version/version.qrc

CONFIG(release, debug|release) {
    DEFINES -= QT_NO_DEBUG_OUTPUT
    #DEFINES -= QT_NO_INFO_OUTPUT
    #DEFINES -= QT_NO_WARNING_OUTPUT
    #DEFINES -= QT_NO_FATAL_OUTPUT
}

# 自定义清理步骤，确保删除所有MOC生成的文件
CLEAN_FILES += moc_*.cpp
QMAKE_POST_LINK += $$QMAKE_CLEAN


