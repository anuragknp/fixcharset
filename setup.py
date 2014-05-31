from distutils.core import setup, Extension

setup(
    ext_modules=[Extension("_fixcharset", ["_fixcharset.cc", "fixcharset.cc", "ystring.cc"])]
)
