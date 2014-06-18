SOURCES += cameraBlobber.cpp
HEADERS += cameraBlobber.h \
            ../../base/processor.h


# OPENCV
INCLUDEPATH += /usr/include/opencv2/

LIBS += -L \
    /usr/lib/ope
    -lhighguincv/ \
    -lcv \

# CVBLOBSLIB
SOURCES += cvblobslib/BlobResult.cpp \
    cvblobslib/BlobExtraction.cpp \
    cvblobslib/Blob.cpp

HEADERS  += cvblobslib/BlobResult.h \
    cvblobslib/BlobLibraryConfiguration.h \
    cvblobslib/BlobExtraction.h \
    cvblobslib/Blob.h
