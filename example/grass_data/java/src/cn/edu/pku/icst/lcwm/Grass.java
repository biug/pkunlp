package cn.edu.pku.icst.lcwm;

import com.sun.jna.Library;
import com.sun.jna.Native;

/**
 * Created by draplater on 16-10-23.
 */
public interface Grass extends Library {
    Grass INSTANCE = (Grass) Native.loadLibrary("grass", Grass.class);
    int UTF8 = 65001;
    int create_segmentor_ctx(String featureFile, String dictFile);
    void delete_segmentor_ctx(int idx);
    void train_segmentor_ctx(String train_file, String feature_file, String dict_file, int times, int encoding);
    void seg_file_with_ctx(int idx, String input_file, String output_file, int encoding);
    String seg_string_with_ctx(int idx, String input, int encoding);
    }