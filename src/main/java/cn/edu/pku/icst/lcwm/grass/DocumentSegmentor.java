package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.NativeLong;

import java.io.File;
import java.io.FileNotFoundException;
import java.nio.charset.Charset;
import java.util.ArrayList;

/**
 * Created by draplater on 16-10-23.
 */
public class DocumentSegmentor {
    private NativeLong segmentor;

    public DocumentSegmentor(String featureFile, String dictFile) throws FileNotFoundException {
        if (!new File(featureFile).exists()) {
            throw new FileNotFoundException(featureFile);
        }
        if (!new File(dictFile).exists()) {
            throw new FileNotFoundException(dictFile);
        }
        this.segmentor = GrassNER.INSTANCE.create_docseg_parser(featureFile, dictFile);
    }

    public String[] segString(String input) {
        ArrayList<String> ret = new ArrayList<>();
        GrassNER.INSTANCE.parse_string_with_docseg_parser(this.segmentor, input,
                ret::add);
        return ret.toArray(new String[0]);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        GrassNER.INSTANCE.delete_docseg_parser(this.segmentor);
    }

}
