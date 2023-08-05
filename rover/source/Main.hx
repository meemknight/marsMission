package;

import haxe.macro.Expr.QuoteStatus;
import sys.FileSystem;
import sys.io.File;

import info.Instructions;

class Main {
    public static var id(default, null):Int = 0;
    public static var round(default, null):Int = 0;
    public static var map(default, null):WorldMap;

    public static function main():Void {
        AStar.init();

        trace("enter id: ");
        id = Std.parseInt(Sys.stdin().readLine());

        begin(id);
        execute();
        loop(id);

        /*
        while(true) {
            // Timer.delay(function() {
                var serverFileName:String = directory + "game/s" + id + "_" + round + ".txt";

                if(!FileSystem.exists(directory)) {
                    Log.warning("Directory" + directory + " does not exist.");
                    break;
                }

                if(FileSystem.exists(serverFileName)) {
                    var input = File.getContent(serverFileName);

                    var ourFileName:String = directory + "game/c" + id + "_" + round + ".txt";
                    File.saveContent(ourFileName, "m u\n");

                    round++;
                }else {
                    break;
                }
            // }, 5);
        }
        */
    }

    public static function loop(id:Int):Void {
        var directory:String = "../../../";

        while(true) {
            var event = directory + "game/s" + id + "_" + (round + 1) + ".txt";

            if(!FileSystem.exists(event)) {
                continue;
            }

            round++;
            execute();
        }
    }

    public static function execute():Void {
        map.doPathFinding();
    }

    public static function begin(id:Int):Void {
        var directory:String = "../../../";
        var serverFileName:String = directory + "game/s" + id + "_" + round + ".txt";

        while(true) {
            var temp = directory + "game/s" + id + "_" + (round + 1) + ".txt";

            if(!FileSystem.exists(temp)) {
                break;
            }

            round++;
            serverFileName = temp;
        }

        if(!FileSystem.exists(serverFileName)) {
            Log.warning("Directory" + serverFileName + " does not exist.");
        }

        map = new WorldMap(serverFileName);
    }
}