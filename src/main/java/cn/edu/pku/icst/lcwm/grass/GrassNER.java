package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.*;

/**
 * Created by draplater on 16-10-23.
 */
public interface GrassNER extends Library {
    interface callback_t extends Callback {
        void invoke(String word, String tag);
    }

    interface docseg_callback_t extends Callback {
        void invoke(String[] words, int length);
    }

    interface callback_position_t extends Callback {
        void invoke(int start, int end, String word, String tag);
    }

    GrassNER INSTANCE = (GrassNER) LibraryLoader.loadLibraryInJAR(GrassNER.class, "libgrass2");
    NativeLong create_ner_parser(String featureInput, String dictPath);
    int parse_string_with_ner_parser(NativeLong context, String inputString, callback_t fn);
    int parse_string_with_ner_parser_withposition(NativeLong context, String inputString, callback_position_t fn);
    int delete_ner_parser(NativeLong context);


    NativeLong libgrass_create_postagger(String featureInput);
    int libgrass_run_tagger(NativeLong context, String inputString, callback_t fn);
    int libgrass_delete_postagger(NativeLong context);

    NativeLong create_docseg_parser(String featureInput, String dictPath);
    int parse_string_with_docseg_parser(NativeLong context, String inputString, docseg_callback_t fn);
    int delete_docseg_parser(NativeLong context);
}
