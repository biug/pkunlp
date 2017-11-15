import cn.edu.pku.icst.lcwm.grass.NERTagger;

import java.io.FileNotFoundException;
import java.util.List;

public class TestNER {
    public static void main(String[] args) throws FileNotFoundException {
        String inputString = "5个常任理事国: 中国、法国、俄罗斯、英国、美国。";
        NERTagger tagger = new NERTagger("nerfeat_pku");
        List<NERTagger.Pair> result = tagger.tagSentence(inputString);
        for(NERTagger.Pair pair: result) {
            System.out.println(pair);
        }
    }
}
