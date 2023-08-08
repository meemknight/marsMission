package;

import algorithm.AI;
import algorithm.Node;
import shop.Market;
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
    @:noCompletion private var __playerQueue:Array<Node>;
    @:noCompletion private var __whatIwannaBuy:Market;
    @:noCompletion private var __currentRock:Node;
    @:noCompletion private var __currentTarget:Node;
    @:noCompletion private var __playerNode:Node;
    @:noCompletion private var __baseNode:Node;
    @:noCompletion private var __index:Int = 0;

    public static var battery:Bool = false;
    public static var level:Int = 1;
    public static var sight:Int = 2;
    public static var attack:Int = 1;

    public function new(world:WorldMap) {
        x = 0;
        y = 0;
        level = 1;
        sight = 2;

        this.world = world;

        __rockQueue = [];
        __playerQueue = [];
        __playerNode = new Node(x, y);
        __baseNode = new Node(x, y);
    }

    public function pathMovement():Void {
        scanArea();

        __whatIwannaBuy = world.shop.checkToBuy(world.iron, world.osmium);

        /*
        if(__whatIwannaBuy != null && __playerQueue.length == 0) {
            trace("hi");
            // backToBase();
        }
        */

        if(__rockQueue.length > 0) {
            goToOre();
        } else if(__playerQueue.length > 0) {
            fightPlayer();
        }else {
            mapOut();
        }
    }

    private function backToBase():Void {
        if((__playerNode.x == __baseNode.x && __playerNode.y == __baseNode.y)) {
            return;
        }

        var n = AI.findPath(level, __playerNode, __baseNode, world);
        __playerNode = n;
        command(n.direction + " m " + n.direction);
    }

    private function fightPlayer():Void {
        if(__currentRock == null) {
            AI.clear();
            __currentRock = __rockQueue.pop();
        }

        update();
        var n = AI.findPath(attack, __playerNode, __currentTarget, world);
        __playerNode = n;
        command(n.direction + " a " + n.direction);

        if((__playerNode.x == __currentTarget.x && __playerNode.y == __currentTarget.y)) {
            __currentTarget = null;
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

                if(Std.parseInt(world.matrix[j][i]) != null && Std.parseInt(world.matrix[j][i]) != Main.id) {
                    var node:Node = new Node(i, j);
                    node.char = world.matrix[j][i];
                    node.h = AI.heuristic(__playerNode, node);
                    __playerQueue.push(node);
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