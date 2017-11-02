#!/usr/bin/python
## encoding: utf-8
from __future__ import unicode_literals, print_function
import ctypes
import os

import six
import sys
import platform
from collections import defaultdict, namedtuple


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
    return b"libgrass-{}{}.{}".format(system, bit, ext)


lib_file = os.path.join(os.path.dirname(__file__), b"lib", detect_lib_name())
grass = ctypes.cdll.LoadLibrary(lib_file)
grass.seg_string_with_ctx.restype = ctypes.c_char_p
grass.syntax_parse_string_with_ctx.restype = ctypes.c_char_p
grass.semantic_parse_string_with_ctx.restype = ctypes.c_char_p
UTF8 = 65001


class Segmentor(object):
    def __init__(self, feature_file, dict_file):
        self.segmentor = grass.create_segmentor_ctx(feature_file, dict_file)

    def seg_string(self, input_str, encoding="UTF-8"):
        if isinstance(input_str, six.text_type):
            input_str = input_str.encode(encoding)
        return [i.decode(encoding) for i in grass.seg_string_with_ctx(self.segmentor, input_str, UTF8).split(b"\x09")]

    def __del__(self):
        grass.delete_segmentor_ctx(self.segmentor)


class WordWithTag(namedtuple("WordWithTag", ["word", "tag"])):
    def __repr__(self):
        return "{}/{}".format(self.word, self.tag)

    def __unicode__(self):
        return self.__repr__()


class CWordWithTag(ctypes.Structure):
    _fields_ = [("word", ctypes.c_char_p), ("tag", ctypes.c_char_p)]


class TaggingResult(ctypes.Structure):
    _fields_ = [("words", ctypes.POINTER(CWordWithTag)),
                ("length", ctypes.c_int)]


class POSTagger(object):
    def __init__(self, feature_file):
        self.tagger = grass.create_postagger_ctx(feature_file)

    def tag_sentence(self, sentences, encoding="UTF-8"):
        if isinstance(sentences, (six.string_types, six.binary_type)):
            sentences = [sentences]

        length = len(sentences)
        array = (ctypes.c_char_p * length)()
        for i in range(length):
            array[i] = sentences[i].encode(encoding)

        result = grass.tag_sentence_with_ctx(self.tagger, array, length, UTF8)
        return [WordWithTag(word=result.words[i].word.decode(encoding), tag=result.words[i].tag.decode(encoding))
                for i in range(result.length)]

    def __del__(self):
        grass.delete_postagger_ctx(self.tagger)


class SyntaxParser(object):
    def __init__(self, feature_file):
        self.parser = grass.create_syntax_parser_ctx(feature_file)

    def parse_string(self, sentence, encoding="UTF-8"):
        if isinstance(sentence, six.text_type):
            sentence = sentence.encode(encoding)
        return grass.syntax_parse_string_with_ctx(self.parser, sentence, UTF8).decode(encoding)


class SemanticParser(object):
    def __init__(self, semantic_feature_file, tree_feature_file):
        self.parser = grass.create_semantic_parser_ctx(semantic_feature_file, tree_feature_file)

    def parse_string(self, sentence, encoding="UTF-8"):
        if isinstance(sentence, six.text_type):
            sentence = sentence.encode(encoding)
        return grass.semantic_parse_string_with_ctx(self.parser, sentence, UTF8).decode(encoding)


grass.tag_sentence_with_ctx.restype = TaggingResult
