package cn.edu.pku.icst.lcwm;

import cn.edu.pku.icst.lcwm.grass.Grass;
import cn.edu.pku.icst.lcwm.grass.POSTagger;
import cn.edu.pku.icst.lcwm.grass.Segmentor;

public class Main {

    public static void main(String[] args) {
        Segmentor s = new Segmentor("../feature/segment.feat",
                "../feature/segment.dic");
        String[] segResult = s.segString("包姌是五月天的铁杆粉丝，今年6月，得知偶像将在10月在上海开演唱会，她立刻在“牛魔王票务网”上订了张10月4日的演唱会看台票，票面价为555元，该网站售价780元。\n" +
                "“门票被加价225元，但因为是网站明码标价，所以也没有过多质疑”，包姌告诉记者，座位号不是自选的，而是票务方给分配的。后来，她收到牛魔王票务寄来的门票，座位号是“19台2层39排11座”，演出时间10月4日19点。\n" +
                "10月3号晚上7点，包姌为了看到第二天期待已久的演唱会，特意从江苏淮安坐车赶往上海，因为堵车，坐了13个小时，次日早晨才到。");
        System.out.println(segResult.length);
        for(String i : segResult) {
            System.out.print(i + " ");
        }

        POSTagger p = new POSTagger("../feature/postag.feat");
        Grass.WordWithTag[] posResult = p.tagSentence(segResult);
        for(Grass.WordWithTag i : posResult) {
            System.out.print(i + " ");
        }
    }
}
