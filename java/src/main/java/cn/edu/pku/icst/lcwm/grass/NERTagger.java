package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.NativeLong;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;

public class NERTagger {
    public class Pair {
        private final String word;
        private final String tag;
        private final int start;
        private final int end;

        Pair(String word, String tag, int start, int end) {
            this.word = word;
            this.tag = tag;
            this.start = start;
            this.end = end;
        }

        Pair(String word, String tag) {
            this(word, tag, -1, -1);
        }

        public String getWord() {
            return word;
        }

        public String getTag() {
            return tag;
        }

        public int getStart() {
            return start;
        }

        public int getEnd() {
            return end;
        }

        @Override
        public String toString() {
            return String.format("%s/%s(%d,%d)", word, tag, start, end);
        }
    }

    private NativeLong context;

    public NERTagger(String featureFile, String dictFile) throws FileNotFoundException {
        if (!new File(featureFile).exists()) {
            throw new FileNotFoundException(featureFile);
        }
        if (dictFile != null && !new File(dictFile).exists()) {
            throw new FileNotFoundException(dictFile);
        }

        if (dictFile == null) {
            dictFile = "";
        }
        context = GrassNER.INSTANCE.create_ner_parser(featureFile, dictFile);
    }

    public NERTagger(String featureFile) throws FileNotFoundException {
        this(featureFile, null);
    }

    public List<Pair> tagSentence(String sentence) {
        final ArrayList<Pair> results = new ArrayList<>();
        GrassNER.callback_position_t callback = (start, end, word, tag) -> results.add(
                new Pair(word, tag, start - 1, end - 1));
        int code = GrassNER.INSTANCE.parse_string_with_ner_parser_withposition(context, sentence, callback);
        return results;
    }

    @Override
    protected void finalize() throws Throwable {
        int code = GrassNER.INSTANCE.delete_ner_parser(context);
        super.finalize();
    }
}
