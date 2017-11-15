package cn.edu.pku.icst.lcwm.grass;

import com.sun.jna.Native;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;

abstract class LibraryLoader {
    private static String getLibraryPath(String libraryBaseName) {
        String osName = System.getProperty("os.name").toLowerCase();
        String osArch = System.getProperty("os.arch").toLowerCase();
        if(osName.contains("win")) {
            osName = "win";
        } else if(osName.contains("linux")) {
            osName = "linux";
        } else {
            throw new IllegalStateException(
                    String.format("Your system %s-%s is unsupported.", osName, osArch));
        }

        if(osArch.contains("64")) {
            osArch = "64";
        } else if(osArch.contains("86") || osArch.contains("32")) {
            osArch = "32";
        } else {
            throw new IllegalStateException(
                    String.format("Your system %s-%s is unsupported.", osName, osArch));
        }

        return String.format("/libs/%s-%s%s.so", libraryBaseName, osName, osArch);
    }

    public static <T> T loadLibraryInJAR(Class<T> interfaceClass, String libraryBaseName) {
        try {
            File tmp = File.createTempFile("libgrass-library", ".tmp");
            InputStream libraryAsStream = LibraryLoader.class.getResourceAsStream(
                    getLibraryPath(libraryBaseName));
            if(libraryAsStream == null) {
                System.err.println("Can not find libgrass!");
                System.exit(1);
                return null;
            }
            Files.copy(libraryAsStream,
                    Paths.get(tmp.getAbsolutePath()),
                    StandardCopyOption.REPLACE_EXISTING);
            T ret = Native.loadLibrary(tmp.getAbsolutePath(), interfaceClass);
            tmp.deleteOnExit();
            return ret;
        } catch (IOException e) {
            System.err.println("Can not create temp file!");
            e.printStackTrace();
            System.exit(1);
            return null;
        }
    }
}
