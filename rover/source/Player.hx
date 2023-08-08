package;

import algorithm.AI;
import algorithm.Node;
import info.Instructions;
import spinehaxe.MathUtils;
import sys.io.File;

using StringTools;


class Player {
    public var x:Int;
    public var y:Int;

    public var world(default, null):WorldMap;
    public var facing(default, null):String;

    @:noCompletion private var __directory:String = "../../../";
    @:noCompletion private var __directions:Array<String> = [UP, DOWN, LEFT, RIGHT];
    @:noCompletion private var __rockQueue:Array<Node>;
    @:noCompletion private var __currentRock:Node;
    @:noCompletion private var __playerNode:Node;
    @:noCompletion private var __index:Int = 0;

    public static var battery:Bool = false;
    public static var level:Int = 1;
    public static var sight:Int = 2;

    public function new(world:WorldMap) {
        x = 0;
        y = 0;
        level = 1;
        sight = 2;

        this.world = world;

        __rockQueue = [];
        __playerNode = new Node(x, y);
    }

    public function pathMovement():Void {
        scanArea();

        if(__rockQueue.length == 0) {
            mapOut();
        } else {
            goToOre();
        }
    }

    private function goToOre():Void {
        if(__currentRock == null) {
            AI.clear();
            __currentRock = __rockQueue.pop();
        }

        update();
        var n = AI.findPath(level, __playerNode, __currentRock, world);
        __playerNode = n;
        command(n.direction + " m " + n.direction);

        if((__playerNode.x == __currentRock.x && __playerNode.y == __currentRock.y)) {
            __currentRock = null;
        }
    }

    private function mapOut():Void {
        var j = Std.int(MathUtils.clamp(this.y - level, 1, world.height));
        var jl = Std.int(MathUtils.clamp(this.y + level, 1, world.height));

        var direction = "";
        var mine = "";

        while (j <= jl) {
            var i = Std.int(MathUtils.clamp(this.x - level, 0, world.width));
            var il = Std.int(MathUtils.clamp(this.x + level, 0, world.width));

            while (i <= il) {
                if((i != x && j != y) || (i == x && j == y) || world.matrix[j][i] == "B") {
                    i++;
                    __index++;
                    continue;
                }

                if(Math.floor(Math.random() * 3) == 0) {
                    __index++;
                }

                direction = __directions[__index % 4];

                if(world.matrix[j][i] == 'X' || world.matrix[j][i] == 'C' || world.matrix[j][i] == 'D') {
                    switch(MathUtils.clamp(i - x, -1, 1)) {
                        case -1:
                            mine = "m l";
                        case 1:
                            mine = "m r";
                    }

                    switch(MathUtils.clamp(j - y, -1, 1)) {
                        case -1:
                            mine = "m u";
                        case 1:
                            mine = "m d";
                    }

                    command(mine);
                    return;
                }

                i++;
            }

            j++;
        }

        if(direction != "") {
            command(direction);
        }
    }

    private function scanArea():Void {
        var j = Std.int(MathUtils.clamp(this.y - sight, 1, world.height));
        var jl = Std.int(MathUtils.clamp(this.y + sight, 1, world.height));

        while(j <= jl) {
            var i = Std.int(MathUtils.clamp(this.x - sight, 0, world.width));
            var il = Std.int(MathUtils.clamp(this.x + sight, 0, world.width));

            while(i <= il) {
                if(world.matrix[j][i] == 'C'
                || world.matrix[j][i] == 'D') {
                    var node:Node = new Node(i, j);
                    node.char = world.matrix[j][i];
                    node.h = AI.heuristic(__playerNode, node);
                    __rockQueue.push(node);
                }

                i++;
            }

            j++;
        }
    }

    public function command(cmd:String):Void {
        var client:String = __directory + "game/c" + Main.id + "_" + Main.round + ".txt";

        File.saveContent(client, cmd + "\n");
    }

    public function update():Void {
        __playerNode.x = x;
        __playerNode.y = y;
    }
}