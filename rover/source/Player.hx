package;

import sys.io.File;

class Player {
    public var x:Int;
    public var y:Int;

    @:noCompletion private var __directory:String = "../../../";

    public function new() {
        x = 0;
        y = 0;
    }

    public function command(cmd:String):Void {
        var client:String = __directory + "game/c" + Main.id + "_" + Main.round + ".txt";

        File.saveContent(client, cmd + "\n");
    }
}