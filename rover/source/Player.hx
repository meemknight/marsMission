package;

import sys.io.File;
import info.Instructions;
import spinehaxe.MathUtils;
import algorithm.PriorityQueue;
import algorithm.Node;

using StringTools;

class Player {
    public static var priorities(default, null):Map<String, Int> = [
        "C" => 1,
        "D" => 2
    ];

    public var x:Int;
    public var y:Int;
    public var sight:Int;
    public var level:Int;

    public var world(default, null):WorldMap;
    public var facing(default, null):String;

    @:noCompletion private var __directory:String = "../../../";
    @:noCompletion private var __directions:Array<String> = [UP, DOWN, LEFT, RIGHT];
    @:noCompletion private var __rockQueue:PriorityQueue<Node>;

    public function new(world:WorldMap) {
        x = 0;
        y = 0;
        level = 1;
        sight = 2;

        this.world = world;

        __rockQueue = new PriorityQueue<Node>(function(a:Node, b:Node) {
            return b.priority - a.priority;
        });
    }

    public function pathMovement():Void {
        scanArea();
        mapOut();
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
                    continue;
                }

                if(world.matrix[j][i] != "F") {
                    var randomDirection = Math.floor(Math.random() * 4);
                    direction = __directions[randomDirection];
                }

                if(world.matrix[j][i] == "C" || world.matrix[j][i] == "A") {
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
                if(world.matrix[j][i] != 'C'
                || world.matrix[j][i] != 'D'
                || world.matrix[j][i] != 'F') {
                    var node:Node = new Node(i, j);
                    node.priority = priorities.get(world.matrix[j][i]);
                    node.char = world.matrix[j][i];
                    __rockQueue.add(node);
                }

                i++;
            }

            j++;
        }
    }

    public function command(cmd:String):Void {
        trace(x + " " + y);

        var client:String = __directory + "game/c" + Main.id + "_" + Main.round + ".txt";

        File.saveContent(client, cmd + "\n");
    }
}