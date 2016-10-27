#!/usr/bin/python
## encoding: utf-8
from __future__ import unicode_literals, print_function
import ctypes
import sys
import platform


def detect_lib_name():
    """
    Select corresponding libgrass
    :return: library file name
    """
    if sys.platform in ['win32', 'cygwin']:
        system = "win"
        ext = 'dll'
    elif sys.platform.startswith("linux"):
        system = "linux"
        ext = 'so'
    else:
        raise SystemError("Your system ({}) is not supported.".format(sys.platform))
    bit = platform.architecture()[0][:2] # 32 or 64
    return "libgrass-{}{}.{}".format(system, bit, ext)


grass = ctypes.cdll.LoadLibrary("./lib/" + detect_lib_name())
grass.seg_string_with_ctx.restype = ctypes.c_char_p
UTF8 = 65001


class Segmentor(object):
    def __init__(self, feature_file, dict_file):
        self.segmentor = grass.create_segmentor_ctx(feature_file, dict_file)

    def seg_string(self, input_str, encoding=b"UTF-8"):
        if isinstance(input_str, unicode):
            input_str = input_str.encode(encoding)
        return grass.seg_string_with_ctx(self.segmentor, input_str, UTF8).decode(encoding)

    def __del__(self):
        grass.delete_segmentor_ctx(self.segmentor)



