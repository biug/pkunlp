package cn.edu.pku.icst.lcwm.grass;

import com.github.arteam.simplejsonrpc.client.JsonRpcClient;
import com.github.arteam.simplejsonrpc.client.Transport;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

public class SyntaxParserV2 {
    private JsonRpcClient client;

    public SyntaxParserV2(String apiAddress) {
        client = new JsonRpcClient(new Transport() {
            @Override
            public String pass(String request) throws IOException {
                URL obj = new URL(apiAddress);
                HttpURLConnection con = (HttpURLConnection) obj.openConnection();
                con.setRequestMethod("POST");
                con.setDoOutput(true);
                con.setRequestProperty("Content-Type", "application/json");
                con.setRequestProperty("Accept", "application/json");
                BufferedWriter wr = new BufferedWriter(new OutputStreamWriter(
                        con.getOutputStream(), "UTF-8"));
                wr.write(request);
                wr.flush();
                wr.close();
                BufferedReader in = new BufferedReader(
                        new InputStreamReader(con.getInputStream(), "UTF-8"));
                String inputLine;
                StringBuilder response = new StringBuilder();

                while ((inputLine = in.readLine()) != null) {
                    response.append(inputLine);
                }
                in.close();

                return response.toString();
            }
        });
    }

    public List<String> parseList(List<List<POSTaggerV2.Pair>> sentences) {
        List<List<List<String>>> req = new ArrayList<>();
        for (List<POSTaggerV2.Pair> sentence : sentences) {
            List<List<String>> reqSent = new ArrayList<>();
            req.add(reqSent);
            for (POSTaggerV2.Pair wordAndTag : sentence) {
                List<String> reqToken = new ArrayList<>();
                reqToken.add(wordAndTag.getWord());
                reqToken.add(wordAndTag.getTag());
                reqSent.add(reqToken);
            }
        }

        return client.createRequest()
                .method("syntax_parser_raw")
                .id(0)
                .params(req)
                .returnAsList(String.class)
                .execute();
    }


    public String parse(List<POSTaggerV2.Pair> sentence) {
        List<List<POSTaggerV2.Pair>> sentences = new ArrayList<>();
        sentences.add(sentence);
        return parseList(sentences).get(0);
    }

}
