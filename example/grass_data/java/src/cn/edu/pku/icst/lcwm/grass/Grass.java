package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.*;

import java.util.Arrays;
import java.util.List;

/**
 * Created by draplater on 16-10-23.
 */
public interface Grass extends Library {
    Grass INSTANCE = (Grass) Native.loadLibrary(GrassLibraryNameGetter.libraryName,
            Grass.class);
    int UTF8 = 65001;

    int create_segmentor_ctx(String featureFile, String dictFile);
    void delete_segmentor_ctx(int idx);
    void train_segmentor_ctx(String train_file, String feature_file, String dict_file, int times, int encoding);
    void seg_file_with_ctx(int idx, String input_file, String output_file, int encoding);
    Pointer seg_string_with_ctx(int idx, String input, int encoding);

    int create_postagger_ctx(String featureFile);
    TaggingResult.ByValue tag_sentence_with_ctx(int idx, StringArray array, int length, int encoding);
    int delete_postagger_ctx(int idx);

    class WordWithTag extends Structure {
        public static class ByReference extends WordWithTag implements Structure.ByReference {}
        public String word;
        public String tag;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("word", "tag");
        }

        @Override
        public String toString() {
            return word + "/" + tag;
        }
    }

    class TaggingResult extends Structure {
        public static class ByValue extends TaggingResult implements Structure.ByValue {}
        public WordWithTag.ByReference words;
        public int length;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("words", "length");
        }
    }
}

class GrassLibraryNameGetter {
    static String libraryName;
    static {
        String osName = System.getProperty("os.name").toLowerCase();
        String osArch = System.getProperty("os.arch").toLowerCase();
        if(osName.contains("win")) {
            osName = "win";
        } else if(osName.contains("linux")) {
            osName = "linux";
        } else {
            throw new IllegalStateException(
                    String.format("Your system %s-%s is unsupported.", osName, osArch));
        }

        if(osArch.contains("64")) {
            osArch = "64";
        } else if(osArch.contains("86") || osArch.contains("32")) {
            osArch = "32";
        } else {
            throw new IllegalStateException(
                    String.format("Your system %s-%s is unsupported.", osName, osArch));
        }

        libraryName = "grass-" + osName + osArch;
    }
}