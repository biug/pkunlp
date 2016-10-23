#!/usr/bin/python
## encoding: utf-8
from __future__ import unicode_literals, print_function
import ctypes

grass = ctypes.cdll.LoadLibrary("./lib/libgrass.so")
grass.seg_string_with_ctx.restype = ctypes.c_char_p
UTF8 = 65001


class Segmentor(object):
    def __init__(self, feature_file, dict_file):
        self.segmentor = grass.create_segmentor_ctx(feature_file, dict_file)

    def seg_string(self, input_str):
        if isinstance(input_str, unicode):
            input_str = input_str.encode(b"UTF-8")
        return grass.seg_string_with_ctx(self.segmentor, input_str, UTF8)

    def __del__(self):
        grass.delete_segmentor_ctx(self.segmentor)


if __name__ == "__main__":
    segmentor = Segmentor(b"../feature/segment.feat", b"../feature/segment.dic")
    print(segmentor.seg_string(
        """
包姌是五月天的铁杆粉丝，今年6月，得知偶像将在10月在上海开演唱会，她立刻在“牛魔王票务网”上订了张10月4日的演唱会看台票，票面价为555元，该网站售价780元。
“门票被加价225元，但因为是网站明码标价，所以也没有过多质疑”，包姌告诉记者，座位号不是自选的，而是票务方给分配的。后来，她收到牛魔王票务寄来的门票，座位号是“19台2层39排11座”，演出时间10月4日19点。
10月3号晚上7点，包姌为了看到第二天期待已久的演唱会，特意从江苏淮安坐车赶往上海，因为堵车，坐了13个小时，次日早晨才到。
        """
    ))
