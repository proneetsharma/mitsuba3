#include <mitsuba/core/stream.h>
#include <mitsuba/core/astream.h>
#include <mitsuba/core/dstream.h>
#include <mitsuba/core/fstream.h>

#include <mitsuba/core/filesystem.h>
#include "python.h"

// TODO: do we really need to do that for templated functions?
#define READ_WRITE_TYPE(Type)                               \
    def("readValue", [](Stream& s, Type &value) {           \
        s.readValue(value);                                 \
    }, DM(Stream, readValue))                               \
    .def("writeValue", [](Stream& s, const Type &value) {   \
        s.writeValue(value);                                \
    }, DM(Stream, writeValue))                              \

MTS_PY_EXPORT(Stream) {
    MTS_PY_CLASS(Stream, Object)
        .mdef(Stream, canWrite)
        .mdef(Stream, canRead)
        // TODO: handle py <=> c++ casts explicitly?
        .READ_WRITE_TYPE(int8_t)
        .READ_WRITE_TYPE(std::string)
        // TODO: lots and lots of other types
        .def("__repr__", &Stream::toString);
}

MTS_PY_EXPORT(DummyStream) {
    MTS_PY_CLASS(DummyStream, Stream)
        .def(py::init<>(), DM(DummyStream, DummyStream))
        .mdef(DummyStream, seek)
        .mdef(DummyStream, truncate)
        .mdef(DummyStream, getPos)
        .mdef(DummyStream, getSize)
        .mdef(DummyStream, flush)
        .mdef(DummyStream, canWrite)
        .mdef(DummyStream, canRead)
        .mdef(DummyStream, canRead)
        .def("__repr__", &DummyStream::toString);

}

MTS_PY_EXPORT(FileStream) {
    MTS_PY_CLASS(FileStream, Stream)
        .def(py::init<const mitsuba::filesystem::path &, bool>(), DM(FileStream, FileStream))
        .mdef(FileStream, seek)
        .mdef(FileStream, truncate)
        .mdef(FileStream, getPos)
        .mdef(FileStream, getSize)
        .mdef(FileStream, flush)
//        .def("canWrite", &FileStream::canWrite, DM(Stream, canWrite))
//        .def("canRead", &FileStream::canRead, DM(Stream, canRead));
        .def("__repr__", &FileStream::toString);
}

MTS_PY_EXPORT(AnnotatedStream) {
    MTS_PY_CLASS(AnnotatedStream, Object);
    // TODO
}
