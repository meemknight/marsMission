package;

import haxe.macro.Expr.Catch;
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
        #if (!windows && (mac || linux))
        seeMyGame(); // Unix based OS only
        #end

        trace("enter id: ");
        id = Std.parseInt(Sys.stdin().readLine());

        Timer.delay(function() {
            begin(id);
            //execute();
            //loop(id);
        }, 5);
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
        var createdMap = false;

        while(true) {
            serverFileName = directory + "game/s" + id + "_" + round + ".txt";

            if(!FileSystem.exists(directory)) {
                Log.warning("Directory" + directory + " does not exist.");
                break;
            }

            if(FileSystem.exists(serverFileName)) {
                var content = File.getContent(serverFileName);
                var lines = content.split("\n");

                if(!createdMap) {
                    map = new WorldMap(serverFileName);
                    createdMap = true;
                }

                if(lines.length < map.length) {
                    continue;
                }

                execute();
                map.refresh();

                round++;
            }
        }
    }

    public static function seeMyGame():Void {
        var cmd = "";
        var args = [];

        #if linux
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