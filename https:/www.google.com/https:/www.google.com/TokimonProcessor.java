package ca.cmpt213.as2;

import com.google.gson.*;

import java.io.*;
import java.util.ArrayList;
import java.util.Scanner;

/*
   Author: Xin Wei
   TokimonProcessor class contains the main fuction 
   of the project, getTokimonFormFiles and getTeams 
   functions. It supports analyzing Tokimon team 
   information from a JSON file and outputting a csv
   file which contains data analyzed.
   Input: tokimon data path and out data path
*/
public class TokimonProcessor {

    public static void main(String[] args) {
	    if(args.length != 2){
	        System.out.println("Expect two and only two arguments");
	        System.exit(1);
        }

	    //initialize
        File directory = new File(args[0]);
	    ArrayList<TokimonTeam> toks_team = new ArrayList<>();
	    getTokimonFormFiles(directory,toks_team);

	    //construct team member
		ArrayList<ArrayList<String>> teams = new ArrayList<>();
		getTeams(toks_team,teams);
		System.out.println(teams.toString());

    }


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
						String id = sub_obj.get("id").getAsString();
						double score = sub_obj.get("compatibility").getAsJsonObject().get("score").getAsDouble();
						String comment = sub_obj.get("compatibility").getAsJsonObject().get("comment").getAsString();
						//construct Tokimon obj
						toks.add(new Tokimon(name,id,new Compatibility(score,comment)));
					}
					teams.add(new TokimonTeam(toks,json_obj.get("extra_comments").getAsString()));
				}
				catch (FileNotFoundException nf){
					System.out.printf("%s is not found/n",file.getName());
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
	}
}
