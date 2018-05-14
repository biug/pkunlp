import cn.edu.pku.icst.lcwm.grass.*;

import java.io.FileNotFoundException;
import java.util.List;
import java.util.Map;

public class TestAll {
    public static void main(String[] args) throws FileNotFoundException {
        DocumentSegmentor s = new DocumentSegmentor(
                "feature/docsegfeature_ctb9.0_compname_feat11",
                "feature/as+cu+msr+pku+sogou+comp");
        String[] segResult;
        segResult = s.segString("五个常任理事国：中国、法国、俄罗斯、英国、美国。");
        System.out.println(segResult.length);
        for (String i : segResult) {
            System.out.print(i + " ");
        }

        POSTaggerV2 p = new POSTaggerV2("feature/postag_withpuw_new.feat");
        List<POSTaggerV2.Pair> posResult = p.tagSentence(segResult);
        for (POSTaggerV2.Pair i : posResult) {
            System.out.print(i + " ");
        }
        System.out.print("\n");


        SyntaxParserV2 parser = new SyntaxParserV2("http://www.icst.pku.edu.cn/lcwm/pkunlp/api");
//        SyntaxParserV2 parser = new SyntaxParserV2("http://127.0.0.1:22233/api");
        System.out.println(parser.parse(posResult));

        String inputString = "2016年9月1日,曹俊杰在中国上北京大学，学费$23，980,684.21。今年,曹俊杰在中国上清华大学，学费5300.00日元。";
        String inputString2 = "佛吉亚在中国的客户包括比亚迪、山东苹果公司、河北蓬莱公司等。";
        NERTaggerV2 tagger = new NERTaggerV2("feature/nerfeat_pku",
                "feature/nerfeat_org",s, p);
        Map<String, List<NERTaggerV2.Pair>> result = tagger.tagSentence(inputString2);
        for(Map.Entry<String, List<NERTaggerV2.Pair>> e : result.entrySet()) {
        	System.out.println(e.getKey() + ":");
        	for(NERTaggerV2.Pair pair : e.getValue()) {
        		System.out.println("  "+ pair.word + "  " + pair.start + "  " + pair.end);
        	}
        }
    }
}


