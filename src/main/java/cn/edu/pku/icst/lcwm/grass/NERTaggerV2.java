package cn.edu.pku.icst.lcwm.grass;

import java.util.*;

import cn.edu.pku.icst.lcwm.grass.*;

import java.io.*;
import java.util.regex.*;

public class NERTaggerV2 {
    static final String PERSON = "PER", LOCATION = "LOC", ORGANIZATION = "ORG", TIME = "TIM", CURRENCY = "CUR";
    private NERTagger ner_tagger;
    private DocumentSegmentor seg_tagger;
    private POSTaggerV2 pos_tagger;

    public class Pair {
        public int start, end;
        public String word;

        Pair(int s, int e, String w) {
            start = s;
            end = e;
            word = w;
        }

        public String toString() {
            return "[word=" + word + ",start=" + start + ",end=" + end + "]";
        }
    }

    public NERTaggerV2(String nerFeatureFile,
                       DocumentSegmentor segmentor,
                       POSTaggerV2 posTagger) throws FileNotFoundException {
        ner_tagger = new NERTagger(nerFeatureFile);
        seg_tagger = segmentor;
        pos_tagger = posTagger;
    }

    public String toDBC(String input) {
        if (null != input) {
            char c[] = input.toCharArray();
            for (int i = 0; i < c.length; i++) {
                if ('\u3000' == c[i]) {
                    c[i] = ' ';
                } else if (c[i] > '\uFF00' && c[i] < '\uFF5F') {
                    c[i] = (char) (c[i] - 65248);
                }
            }
            String dbc = new String(c);
            return dbc;
        }
        return null;
    }

    public void tagCurrency(Map<String, List<Pair>> ner_list, String inputString) {
        String r1 = "([?|?|?|?|?|?|?|?|?|0-9]+[?|?|?|?|?]+)+";
        String r2 = "([?|?|?|?|?|?|?|?|?|0-9]*[?|?|?|?|?]+)+";
        String r3 = "([?|?|?|?|?|?|?|?|?|0-9]+[?|?|?|?|?]*)+";
        String r4 = "[0-9]+\\.[0-9]+";
        String r5 = "([0-9]+,[0-9]+)+\\.[([0-9]+,[0-9]+)|[0-9]+]+";
        String rule = "([¥|$|?|£](" + r5 + "|" + r4 + "|" + r1 + "|" + r2 + "|" + r3 + "))|"
                + "((" + r5 + "|" + r4 + "|" + r1 + "|" + r2 + "|" + r3 + ").{0,3}[?|?|?|?|?])";
        Pattern p = Pattern.compile(rule);
        Matcher m = p.matcher(inputString);
        while (m.find()) {
            ner_list.get(NERTaggerV2.CURRENCY).add(new NERTaggerV2.Pair(m.start() + 1, m.end(), m.group()));
        }
    }

    public void tagTime(Map<String, List<NERTaggerV2.Pair>> ner_list, String inputString) {
        String[] segResult = seg_tagger.segString(inputString);
        List<POSTaggerV2.Pair> posResult = pos_tagger.tagSentence(segResult);
        int already = 0;
        List<NERTaggerV2.Pair> tmp = new ArrayList<NERTaggerV2.Pair>();
        for (POSTaggerV2.Pair g : posResult) {
            if (g.getTag().equals("NT")) {
                tmp.add(new Pair(already, already + g.getWord().length(), g.getWord()));
            }
            already += g.getWord().length();
        }
        int s = 0, e = 0;
        String w = "";
        for (NERTaggerV2.Pair p : tmp) {
            if (e == p.start) {
                e = p.end;
                w += p.word;
            } else {
                if (!w.equals("")) {
                    ner_list.get(NERTaggerV2.TIME).add(new Pair(s + 1, e, w));
                }
                s = p.start;
                e = p.end;
                w = p.word;
            }
        }
        if (!w.equals("")) {
            ner_list.get(NERTaggerV2.TIME).add(new Pair(s + 1, e, w));
        }
    }

    public Map<String, List<NERTaggerV2.Pair>> tagSentence(String inputString) {
        inputString = toDBC(inputString);
        String[] ner_type = {NERTaggerV2.PERSON, NERTaggerV2.LOCATION, NERTaggerV2.ORGANIZATION,
                NERTaggerV2.TIME, NERTaggerV2.CURRENCY};
        Map<String, List<NERTaggerV2.Pair>> ner_list = new TreeMap<String, List<NERTaggerV2.Pair>>();
        // 5种NER，全部初始化为空
        for (String str : ner_type) {
            ner_list.put(str, new ArrayList<NERTaggerV2.Pair>());
        }
        // 调用现有的NERTagger.tagSentence得到人名地名机构名
        List<NERTagger.Pair> result = ner_tagger.tagSentence(inputString);
        int already = 0;
        for (NERTagger.Pair pair : result) {
            int start = pair.getStart();
            int end = pair.getEnd();
            switch (pair.getTag()) {
                case NERTaggerV2.PERSON: {
                    ner_list.get(NERTaggerV2.PERSON).add(new NERTaggerV2.Pair(start, end, pair.getWord()));
                    break;
                }
                case NERTaggerV2.LOCATION: {
                    ner_list.get(NERTaggerV2.LOCATION).add(new NERTaggerV2.Pair(start, end, pair.getWord()));
                    break;
                }
                case NERTaggerV2.ORGANIZATION: {
                    ner_list.get(NERTaggerV2.ORGANIZATION).add(new NERTaggerV2.Pair(start, end, pair.getWord()));
                    break;
                }
            }
            already = end;
        }
        // 识别货币
        tagCurrency(ner_list, inputString);
        // 识别时间
        tagTime(ner_list, inputString);

        return ner_list;
    }
}