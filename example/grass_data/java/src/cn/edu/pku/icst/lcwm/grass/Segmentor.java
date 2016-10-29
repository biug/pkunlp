package cn.edu.pku.icst.lcwm.grass;

import cn.edu.pku.icst.lcwm.grass.Grass;

/**
 * Created by draplater on 16-10-23.
 */
public class Segmentor {
    private int segmentor;

    public Segmentor(String feature_file, String dict_file) {
        this.segmentor = Grass.INSTANCE.create_segmentor_ctx(feature_file, dict_file);
    }

    public String[] segString(String input) {
        return Grass.INSTANCE.seg_string_with_ctx(this.segmentor, input, Grass.UTF8).split(" ");
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        Grass.INSTANCE.delete_segmentor_ctx(this.segmentor);
    }

}
