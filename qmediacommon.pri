INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/commonglobal.h \
    $$PWD/core/basepresenter.h \
    $$PWD/core/baserepository.h \
    $$PWD/core/baseservice.h \
    $$PWD/core/repositoryerror.h \
    $$PWD/logquery.h \
    $$PWD/orm/databaseconnection.h \
    $$PWD/orm/entitymanager.h \
    $$PWD/orm/ormtypes.h \
    $$PWD/orm/repositoryfactory.h \

SOURCES += \
    $$PWD/core/basepresenter.cpp \
    $$PWD/core/baserepository.cpp \
    $$PWD/core/baseservice.cpp \
    $$PWD/logquery.cpp \
    $$PWD/orm/databaseconnection.cpp \
    $$PWD/orm/entitymanager.cpp \
    $$PWD/orm/ormtypes.cpp \
    $$PWD/orm/repositoryfactory.cpp \
