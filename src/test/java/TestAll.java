import cn.edu.pku.icst.lcwm.grass.*;

import java.io.FileNotFoundException;
import java.util.List;

public class TestAll {
        public static void main(String[] args) throws FileNotFoundException {
//        Segmentor s = new Segmentor("segment.feat",
//                "segment.dic");
//        String[] segResult = s.segString(
//                "在第31届夏季奥林匹克运动会上，中国女子排球队永不放弃、拼搏到底，" +
//                        "时隔12年重返世界之巅，中国女排姑娘们展现出了一辈辈传承至" +
//                        "今的女排精神，再一次感动了国人、振奋了国人。");
//        System.out.println(segResult.length);
//        for(String i : segResult) {
//            System.out.print(i + " ");
//        }
//
//        POSTagger p = new POSTagger("postag.feat");
//        Grass.WordWithTag[] posResult = p.tagSentence(segResult);
//        for(Grass.WordWithTag i : posResult) {
//            System.out.print(i + " ");
//        }
//        System.out.print("\n");
//
//        SyntaxParser syn = new SyntaxParser("syntax.feat");
//        String syntaxTree = syn.parse(posResult);
//        System.out.println(syntaxTree);
//
//        SemanticParser sem = new SemanticParser(
//                "semantic.feat",
//                "syntax.feat");
//        String semGraph = sem.parse(posResult);
//        System.out.println(semGraph);

        String inputString = "5个常任理事国: 中国、法国、俄罗斯、英国、美国。";
        NERTagger tagger = new NERTagger("nerfeat_pku");
        List<NERTagger.Pair> result = tagger.tagSentence(inputString);
        for(NERTagger.Pair pair: result) {
            System.out.println(pair);
        }
    }
}


