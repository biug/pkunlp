package cn.edu.pku.icst.lcwm.grass;

import cn.edu.pku.icst.lcwm.grass.Grass;

import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by draplater on 16-10-23.
 */
public class Segmentor {
    private int segmentor;

    public Segmentor(String feature_file, String dict_file) {
        this.segmentor = Grass.INSTANCE.create_segmentor_ctx(feature_file, dict_file);
    }

    public String[] segString(String input) {
        byte[] result = Grass.INSTANCE.seg_string_with_ctx(this.segmentor, input, Grass.UTF8).getString(0).getBytes();
        ArrayList<String> ret = new ArrayList<>();
        int start = 0;
        for(int i=start; i<result.length; i++) {
            if(start != i && result[i] == 9) {
                ret.add(new String(result, start, i - start, Charset.forName("UTF-8")));
                start = i + 1;
            }
        }

        if(start != result.length) {
            ret.add(new String(result, start, result.length - start, Charset.forName("UTF-8")));
        }

        return ret.toArray(new String[0]);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        Grass.INSTANCE.delete_segmentor_ctx(this.segmentor);
    }

}
