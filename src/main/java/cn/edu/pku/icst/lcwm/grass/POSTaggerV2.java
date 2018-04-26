package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.NativeLong;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

public class POSTaggerV2 {
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

    public POSTaggerV2(String featureFile) throws FileNotFoundException {
        if (!new File(featureFile).exists()) {
            throw new FileNotFoundException(featureFile);
        }
        context = GrassNER.INSTANCE.libgrass_create_postagger(featureFile);
    }

    public List<Pair> tagSentence(String[] words) {
        final ArrayList<Pair> results = new ArrayList<>();
        GrassNER.callback_t callback = (word, tag) -> results.add(
                new Pair(word, tag));
        String inputString = Arrays.stream(words).collect(Collectors.joining("\3"));
        int code = GrassNER.INSTANCE.libgrass_run_tagger(context, inputString, callback);
        return results;
    }

    @Override
    protected void finalize() throws Throwable {
        int code = GrassNER.INSTANCE.libgrass_delete_postagger(context);
        super.finalize();
    }
}
