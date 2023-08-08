package;

import haxe.Timer;
import haxe.macro.Expr.QuoteStatus;
import sys.FileSystem;
import sys.io.File;

import info.Instructions;

class Main {
    public static var id(default, null):Int = 0;
    public static var round(default, null):Int = 0;
    public static var map(default, null):WorldMap;

    public static function main():Void {
        seeMyGame();

        trace("enter id: ");
        id = Std.parseInt(Sys.stdin().readLine());

        Timer.delay(function() {
            begin(id);
            execute();
            loop(id);
        }, 5);

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

            var content = File.getContent(event);
            var lines = content.split("\n");

            if(lines.length < map.length) {
                continue;
            }

            round++;
            execute();
            map.refresh();
        }
    }

    public static function execute():Void {
        map.player.pathMovement();
    }

    public static function begin(id:Int):Void {
        var directory:String = "../../../";
        var serverFileName:String = directory + "game/s" + id + "_" + round + ".txt";

        if(!FileSystem.exists(directory + "game/")) {
            Log.warning(directory + "game/");
        }

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

    public static function seeMyGame():Void {
        var cmd = "";
        var args = [];

        #if windows
        cmd = "tasklist | findstr";
        args = ["/I", '/C: "mygame"', '/C:"feeshygame"'];
        #elseif linux
        cmd = "pgrep";
        args = ["mygame", "feeshygame"];
        #else
        cmd = "pgrep";
        args = ["mygame", "feeshygame"];
        #end

        var process = new sys.io.Process(cmd, args);
        var output = process.stdout.readAll().toString();

        if(output.length == 0) {
            Log.warning("Could not find application! Please make sure the executable is named 'mygame' or 'feeshygame'! Or that it is running!");
        }

        while(output.length == 0) {
            process = new sys.io.Process(cmd, args);
            output = process.stdout.readAll().toString();
        }

        trace("Successfully found application!");
    }
}