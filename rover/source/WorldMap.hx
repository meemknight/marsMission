package;

import cpp.NativeArray;
import sys.io.File;

class WorldMap {
    public var width(default, null):Int;
    public var height(default, null):Int;
    public var content(default, null):String;
    public var matrix(default, null):Array<Array<String>>;
    public var player(default, null):Player;
    public var length(default, null):Int = 0;

    public function new(serverFile:String) {
        content = File.getContent(serverFile);
        var lines = content.split("\n");
        length = lines.length;

        width = Std.parseInt(lines[0].split(" ")[0]);
        height = Std.parseInt(lines[0].split(" ")[1]);

        matrix = [];


        // Spawn Player

        player = new Player(this);
        player.x = Std.parseInt(lines[height + 1].split(" ")[0]);
        player.y = Std.parseInt(lines[height + 1].split(" ")[1]) + 1;

        for(i in 1...(height + 1)) {
            matrix[i] = lines[i].split(" ");
        }
    }

    public function refresh():Void {
        var directory:String = "../../../";
        var serverFileName:String = directory + "game/s" + Main.id + "_" + Main.round + ".txt";

        content = File.getContent(serverFileName);
        var lines = content.split("\n");

        player.x = Std.parseInt(lines[height + 1].split(" ")[0]);
        player.y = Std.parseInt(lines[height + 1].split(" ")[1]) + 1;
    }
}