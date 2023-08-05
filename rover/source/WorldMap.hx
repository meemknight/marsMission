package;

import sys.io.File;

class WorldMap {
    public var width(default, null):Int;
    public var height(default, null):Int;
    public var content(default, null):String;
    public var player(default, null):Player;

    public function new(serverFile:String) {
        content = File.getContent(serverFile);
        var lines = content.split("\n");

        width = Std.parseInt(lines[0].split(" ")[0]);
        height = Std.parseInt(lines[0].split(" ")[1]);


        // Spawn Player

        player = new Player();
        player.x = Std.parseInt(lines[height + 1].split(" ")[0]);
        player.y = Std.parseInt(lines[height + 1].split(" ")[1]);
    }

    public function doPathFinding():Void {
        AStar.execute(player.x, player.y, width, height);
    }
}