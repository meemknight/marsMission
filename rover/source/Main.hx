package;

import haxe.macro.Expr.Catch;
import haxe.Timer;
import haxe.macro.Expr.QuoteStatus;
import sys.FileSystem;
import sys.io.File;

import info.Instructions;

@:buildXml('<include name="../../Include.xml.tpl" />')
@:cppInclude("rover.h")
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

        begin(id);
    }

    public static function execute():Void {
        map.player.pathMovement();
    }

    public static function begin(id:Int):Void {
        var directory:String = "../../../";

        var serverFileName:String = directory + "game/s" + id + "_" + round + ".txt";
        var createdMap = false;

        while(true) {
            untyped __cpp__('std::this_thread::sleep_for(std::chrono::milliseconds(5));');

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

                if(lines.length < map.length) { // Safety guard just incase the sleep doesn't work.
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