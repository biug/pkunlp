package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.NativeLong;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOError;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class NERTagger {
    public class Pair {
        private final String word;
        private final String tag;

        Pair(String word, String tag) {
            this.word = word;
            this.tag = tag;
        }

        public String getWord() {
            return word;
        }

        public String getTag() {
            return tag;
        }

        @Override
        public String toString() {
            return String.format("%s/%s", word, tag);
        }
    }

    private NativeLong context;

    public NERTagger(String featureFile, String dictFile) throws FileNotFoundException {
        if(!new File(featureFile).exists()) {
            throw new FileNotFoundException(featureFile);
        }
        if(dictFile != null && !new File(dictFile).exists()) {
            throw new FileNotFoundException(dictFile);
        }

        if(dictFile == null) {
            dictFile = "";
        }
        context = GrassNER.INSTANCE.create_ner_parser(featureFile, dictFile);
    }

    public NERTagger(String featureFile) throws FileNotFoundException {
        this(featureFile, null);
    }

    public List<Pair> tagSentence(String sentence) {
        final ArrayList<Pair> results = new ArrayList<>();
        GrassNER.callback_t callback = (word, tag) -> results.add(new Pair(word, tag));
        int code = GrassNER.INSTANCE.parse_string_with_ner_parser(context, sentence, callback);
        return results;
    }

    @Override
    protected void finalize() throws Throwable {
        int code = GrassNER.INSTANCE.delete_ner_parser(context);
        super.finalize();
    }
}
