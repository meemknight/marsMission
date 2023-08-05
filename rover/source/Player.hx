package;

import sys.io.File;
import info.Instructions;
import spinehaxe.MathUtils;

using StringTools;

class Player {
    public var x:Int;
    public var y:Int;
    public var sight:Int;
    public var level:Int;

    public var foundCobble:Vec2;
    public var world(default, null):WorldMap;
    public var facing(default, null):String;

    @:noCompletion private var __directory:String = "../../../";
    @:noCompletion private var __directions:Array<String> = [UP, DOWN, LEFT, RIGHT];

    public function new(world:WorldMap) {
        x = 0;
        y = 0;
        level = 1;
        sight = 2;

        this.world = world;
    }

    public function pathMovement():Void {
        scanArea();
    }

    private function mapOut():Void {
        var j = Std.int(MathUtils.clamp(this.y - (sight - 1), 0, world.height));
        var jl = Std.int(MathUtils.clamp(this.y + (sight - 1), 0, world.height));

        var listOfDirections:Map<Int, String> = [];

        while(j <= jl) {
            var i = Std.int(MathUtils.clamp(this.x - (sight - 1), 0, world.width));
            var il = Std.int(MathUtils.clamp(this.x + (sight - 1), 0, world.width));

            while(i <= il) {
                if((i != x && j != y) || (i == x && j == y) || world.matrix[j][i] == "B") {
                    continue;
                }

                

                j++;
            }
            
            i++;
        }
    }

    private function scanArea():Void {
        var j = Std.int(MathUtils.clamp(this.y - sight, 0, world.height));
        var jl = Std.int(MathUtils.clamp(this.y + sight, 0, world.height));

        while(j <= jl) {
            var i = Std.int(MathUtils.clamp(this.x - sight, 0, world.width));
            var il = Std.int(MathUtils.clamp(this.x + sight, 0, world.width));

            while(i <= il) {
                if(world.matrix[j][i] != '?'
                && world.matrix[j][i] != 'X'
                && world.matrix[j][i] != '.'
                && world.matrix[j][i] != 'O'
                && world.matrix[j][i] != 'B') {
                    AStar.storeCoolRock(world.matrix[j][i], i, j);
                }else if(world.matrix[j][i] != 'X') {
                    foundCobble = {x: i, y: j};
                }

                j++;
            }

            i++;
        }
    }

    public function command(cmd:String):Void {
        if (!cmd.contains('m')) {
            switch(cmd) { // Consider upgrades
                case UP:
                    y -= 1 * level;
                case DOWN:
                    y += 1 * level;
                case LEFT:
                    x -= 1 * level;
                case RIGHT:
                    x += 1 * level;
            }
        }

        var client:String = __directory + "game/c" + Main.id + "_" + Main.round + ".txt";

        File.saveContent(client, cmd + "\n");
    }
}