package ca.cmpt213.as2;

import com.google.gson.*;
import com.opencsv.CSVWriter;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Scanner;

/**
 * TokimonSupport class contains functions for
 * reading Tokimon teams from json files and 
 * generating results from Tokimon team information.
 */
public class TokimonSupport {
    public static void getTokimonFormFiles(File directory, ArrayList<TokimonTeam> teams){
        //will keep directory and .json files
        File[] files = directory.listFiles(new FileExtensionFilter(".json"));
        for(File file : files){
            if(file.isDirectory()){
                getTokimonFormFiles(file,teams);
            }
            else{
                //JSON FILE DETECTED
                //parsing json string to obj
                try{
                    Scanner scanner = new Scanner(file);
                    String json = scanner.useDelimiter("\\A").next();
                    JsonParser parser = new JsonParser();
                    JsonObject json_obj = parser.parse(json).getAsJsonObject();
                    ArrayList<Tokimon> toks = new ArrayList<>();
                    JsonArray array = json_obj.get("team").getAsJsonArray();
                    for(JsonElement ele : array){
                        JsonObject sub_obj = ele.getAsJsonObject();
                        String name = sub_obj.get("name").getAsString();
                        String id = sub_obj.get("id").getAsString().toLowerCase().trim();
                        double score = sub_obj.get("compatibility").getAsJsonObject().get("score").getAsDouble();
                        if(score < 0){
                            System.out.printf("In file %s, id: %s's score must greater than 0",file.getAbsolutePath(),id);
                            System.exit(1);
                        }
                        String comment = sub_obj.get("compatibility").getAsJsonObject().get("comment").getAsString();
                        //construct Tokimon obj
                        toks.add(new Tokimon(name,id,new Compatibility(score,comment)));
                    }
                    teams.add(new TokimonTeam(toks,json_obj.get("extra_comments").getAsString(),json_obj.get("team").getAsJsonArray().get(0).getAsJsonObject().get("id").getAsString()));
                }
                catch (FileNotFoundException nf){
                    System.out.printf("%s is not found/n",file.getName());
                }
                catch (JsonSyntaxException se){
                    System.out.println("Bad JSON file format or missing required fields");
                }
                catch(JsonParseException pe){
                    System.out.println("Bad JSON file format or missing required fields");
                }

            }
        }
    }
    public static void getTeams(ArrayList<TokimonTeam> toks_team,ArrayList<ArrayList<String>> teams){
        if(toks_team.size() != 0){
            teams.add(new ArrayList<>());
            for(Tokimon tok:toks_team.get(0).team){
                teams.get(0).add(tok.getId());
            }
        }
        for(TokimonTeam toks : toks_team){
            if(toks.team.size() == 0){
                continue;
            }
            //flag is used for checking whether needs to add info to new team
            //if false then add new team.
            boolean flag = false;
            for(int i = 0; i < teams.size(); i++){
                //json file already registered in team
                if(teams.get(i).contains(toks.team.get(0).getId())){

                    flag = true;
                    break;
                }
            }
            if(flag){
                continue;
            }
            else{
                ArrayList<String> new_team = new ArrayList<>();
                for(Tokimon tok : toks.team){
                    new_team.add(tok.getId());
                }
                teams.add(new_team);
            }

        }
        for(int i = 0; i < teams.size();i++){
            Collections.sort(teams.get(i));
        }
    }
    public static void writeResult(String path,ArrayList<ArrayList<String>> teams,ArrayList<TokimonTeam> toks_team){
        int len,sum;
        ArrayList<String[]> data= new ArrayList<>();
        data.add(new String[]{ "Team#", "From Toki", "To Toki", "Score", "Comment", "", "Extra"});
        int num = 1;
        for(ArrayList<String> team:teams){
            data.add(new String[]{"Team" + num,"","","","","",""});
            sum = 0;
            len = team.size();
            for(String id:team){
                for(TokimonTeam tok_team:toks_team){
                    if(id.toLowerCase().trim().compareTo(tok_team.getSelfid().toLowerCase().trim()) == 0){
                        ArrayList<String[]> temp = new ArrayList<>();
                        for(Tokimon tok : tok_team.team){
                            if(!team.contains(tok.getId())){
                                System.out.println("ID: " + tok.getId() + "is not the teammate with others");
                                System.exit(-1);
                            }
                            if(tok.getId() == tok_team.getSelfid()){
                                temp.add(new String[]{"",tok.getId(),"-",Double.toString(tok.getScore()),tok.getComment(),"",tok_team.getComment()});
                            }
                            else{
                                temp.add(new String[]{"",id,tok.getId(),Double.toString(tok.getScore()),tok.getComment(),"",""});
                            }
                        }
                        if(temp.size() != len){
                            System.out.println("Tokimon lacks other Tokimon(s) infomation");
                            System.exit(-1);
                        }
                        sum = sum + temp.size();
                        listSort(temp);
                        temp.forEach((n)->{data.add(n);});
                        break;
                    }
                }
            }
            if(sum != len*len){
                System.out.println("team fails to submit a JSON file");
                System.exit(-1);
            }
            num++;
        }

        try {
            File output = new File(path);
            FileWriter outputfile = new FileWriter(output);
            CSVWriter writer = new CSVWriter(outputfile, ',',
                    CSVWriter.DEFAULT_QUOTE_CHARACTER,
                    CSVWriter.DEFAULT_ESCAPE_CHARACTER);
            writer.writeAll(data);
            writer.close();
        }
        catch (IOException c){
            System.out.println(c.getCause());
        }
    }
    public static void listSort(ArrayList<String[]> array){
        for(int i = 0; i < array.size() - 1 ; i++){
            for(int j = 0; j <array.size() - 1 - i; j++){
                if(array.get(j)[2].compareTo(array.get(j + 1)[2]) > 0 || array.get(j)[2] == "-"){
                    Collections.swap(array,j,j+1);
                }
            }
        }

    }
}
