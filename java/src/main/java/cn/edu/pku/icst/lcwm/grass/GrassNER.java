package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.*;

/**
 * Created by draplater on 16-10-23.
 */
public interface GrassNER extends Library {
    interface callback_t extends Callback {
        void invoke(String word, String tag);
    }

    interface callback_position_t extends Callback {
        void invoke(int start, int end, String word, String tag);
    }

    GrassNER INSTANCE = (GrassNER) LibraryLoader.loadLibraryInJAR(GrassNER.class, "libgrass2");
    NativeLong create_ner_parser(String featureInput, String dictPath);
    int parse_string_with_ner_parser(NativeLong context, String inputString, callback_t fn);
    int parse_string_with_ner_parser_withposition(NativeLong context, String inputString, callback_position_t fn);
    int delete_ner_parser(NativeLong context);
}
