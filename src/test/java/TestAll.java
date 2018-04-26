import cn.edu.pku.icst.lcwm.grass.*;

import java.io.FileNotFoundException;
import java.util.List;

public class TestAll {
    public static void main(String[] args) throws FileNotFoundException {
//        Segmentor s = new Segmentor("segment.feat",
//                "segment.dic");
        DocumentSegmentor s = new DocumentSegmentor(
                "feature/docsegfeature_ctb9.0_compname_feat11",
                "feature/as+cu+msr+pku+sogou+comp");
        String[] segResult = s.segString(
                "在第31届夏季奥林匹克运动会上，中国女子排球队永不放弃、拼搏到底，" +
                        "时隔12年重返世界之巅，中国女排姑娘们展现出了一辈辈传承至" +
                        "今的女排精神，再一次感动了国人、振奋了国人。");
        System.out.println(segResult.length);
        for (String i : segResult) {
            System.out.print(i + " ");
        }

        POSTaggerV2 p = new POSTaggerV2("feature/postag.feat");
        List<POSTaggerV2.Pair> posResult = p.tagSentence(segResult);
        for (POSTaggerV2.Pair i : posResult) {
            System.out.print(i + " ");
        }
        System.out.print("\n");


        SyntaxParserV2 parser = new SyntaxParserV2("http://www.icst.pku.edu.cn/lcwm/pkunlp/api");
        System.out.println(parser.parse(posResult));

        String inputString = "5个常任理事国: 中国、法国、俄罗斯、英国、美国。";
        NERTagger tagger = new NERTagger("feature/nerfeat_pku");
        List<NERTagger.Pair> result = tagger.tagSentence(inputString);
        for (NERTagger.Pair pair : result) {
            System.out.println(pair);
        }
    }
}


