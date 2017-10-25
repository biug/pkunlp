package cn.edu.pku.icst.lcwm;

import cn.edu.pku.icst.lcwm.grass.*;

public class Main {

    public static void main(String[] args) {
        Segmentor s = new Segmentor("../feature/segment.feat",
                "../feature/segment.dic");
        String[] segResult = s.segString(
                "在第31届夏季奥林匹克运动会上，中国女子排球队永不放弃、拼搏到底，" +
                        "时隔12年重返世界之巅，中国女排姑娘们展现出了一辈辈传承至" +
                        "今的女排精神，再一次感动了国人、振奋了国人。");
        System.out.println(segResult.length);
        for(String i : segResult) {
            System.out.print(i + " ");
        }

        POSTagger p = new POSTagger("../feature/postag.feat");
        Grass.WordWithTag[] posResult = p.tagSentence(segResult);
        for(Grass.WordWithTag i : posResult) {
            System.out.print(i + " ");
        }
        System.out.print("\n");

        SyntaxParser syn = new SyntaxParser("../feature/syntax.feat");
        String syntaxTree = syn.parse(posResult);
        System.out.println(syntaxTree);

        SemanticParser sem = new SemanticParser(
                "../feature/semantic.feat",
                "../feature/syntax.feat");
        String semGraph = sem.parse(posResult);
        System.out.println(semGraph);
    }
}
