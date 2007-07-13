TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib #dll # build shared library
CONFIG += release warn_off create_prl
TARGET = vecmat
DESTDIR = ../../lib

macx {
 # compile release + universal binary
 CONFIG += x86
 #CONFIG += lib_bundle
 #QMAKE_BUNDLE_EXTENSION = .framework
}

HEADERS += \
	Matrix3x3.hh \
	Matrix4x4.hh \
	Quaternion.hh \
	Vector.hh \
	Vector2d.hh \
	Vector3d.hh \
	Vector4d.h

SOURCES += \
	Matrix3x3.cc \
	Matrix4x4.cc \
	Vector.cc \
	Vector2d.cc \
	Vector3d.cc \
	Vector4d.cc
