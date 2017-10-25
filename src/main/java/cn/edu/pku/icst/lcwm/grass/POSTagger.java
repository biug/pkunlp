package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.StringArray;

/**
 * Created by draplater on 16-10-29.
 */
public class POSTagger {

    private int tagger;

    public POSTagger(String featureFile) {
        tagger = Grass.INSTANCE.create_postagger_ctx(featureFile);
    }

    public Grass.WordWithTag[] tagSentence(String[] words) {
        Grass.TaggingResult.ByValue result = Grass.INSTANCE.tag_sentence_with_ctx(
                tagger, new StringArray(words), words.length, Grass.UTF8);
        return (Grass.WordWithTag[]) result.words.toArray(result.length);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        Grass.INSTANCE.delete_segmentor_ctx(tagger);
    }
}
