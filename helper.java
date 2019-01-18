package ca.sfu.cmpt213.as1;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;


public class Helper {
    private ArrayList<Tokimon> tokens;
    private String title;
    private String[] options;
    private Scanner scanner = new Scanner(System.in);

    Helper(){
         this.tokens = new ArrayList<>();
         this.title = "default";
         this.options = new String[0];
    }

    Helper(String title, String[] options){
        this.tokens = new ArrayList<>();
        this.title = title;
        this.options = Arrays.copyOf(options,options.length);
    }

    private void display(){
        System.out.println("\n*************\n* Main Menu *\n*************");

        for (int i = 1; i <= options.length; i++){
            System.out.println( i + ". " + options[i - 1]);
        }

    }

    private void doOperation(int order){
        switch (order){
            case 1:
                this.display_token();
                break;
            case 2:
                this.add_token();
                break;
            case 3:
                this.remove_token();
                break;
            case 4:
                this.change_token_strength();
                break;
            case 5:
                this.dump();
                break;
            case 6:
                this.exit();
                break;
        }
    }

    private void display_token(){
        System.out.println("*********************\n* List of Tokimons: *\n*********************");
        for (int i =  1; i <= tokens.size(); i++){
            Tokimon cur = tokens.get(i - 1);
            System.out.printf("%d. %s, %fm, %s ability,%d strength\n", i, cur.getName(), cur.getSize(), cur.getType(), cur.getStrength());
        }
        return;
    }

    private void add_token(){
        System.out.print("Enter Tokimon's name: ");
        String name = scanner.next();
        System.out.print("Enter Tokimon's type: ");
        String type = scanner.next();
        System.out.print("Enter Tokimon's size: ");
        double size = scanner.nextDouble();
        //insert new Tokimon to the front;
        tokens.add(0,new Tokimon(name,type,size));
        return;
    }

    private void remove_token(){
        this.display_token();
        System.out.print("(Enter 0 to cancel)\n> ");
        int idx = scanner.nextInt();
        if(idx == 0){
            return;
        }
        tokens.remove(idx - 1);
        return;
    }

    private void change_token_strength(){
        this.display_token();
        System.out.print("(Enter 0 to cancel)\n> ");
        int idx = scanner.nextInt();
        if(idx == 0){
            return;
        }
        System.out.print("By how much?: ");
        int strength = scanner.nextInt();
        tokens.get(idx - 1).setStrength(tokens.get(idx - 1).getStrength() + strength);
        System.out.printf("%s now has strength %d!\n",tokens.get(idx - 1).getName(),tokens.get(idx - 1).getStrength());
    }

    private void dump(){
        System.out.println("All Tokimon objects:");
        for( int i = 1; i <= tokens.size(); i++){
            System.out.printf("%d. %s\n", i, tokens.get(i - 1).toString());
        }
    }

    private void exit(){
        this.scanner.close();
        System.exit(0);
    }

    public void start(){
        System.out.println(
                "*".repeat(4 + 19 + title.length()) + "\n" + "* Tokimon Tracker by " + title + " *\n" + "*".repeat(4 + 19 + title.length())
        );
        while (true){
            this.display();
            System.out.print("> ");
            try{
                int order = scanner.nextInt();
                doOperation(order);
            }
            finally {
                System.out.println("Enter A NUMBER!!!");
            }
        }

    }
}
