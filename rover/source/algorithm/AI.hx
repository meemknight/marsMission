package algorithm;

import spinehaxe.MathUtils;
import info.Instructions;

class AI {
    private static var visitedNodes:Array<Node> = [];
    
    public static function findPath(cost:Int, start:Node, end:Node, world:WorldMap):Node {
        var openList:Array<Node> = [];
        var current:Node = null;
        
        var n = neighbors(start, cost, world);

        for(i in 0...n.length) {
            var node = n[i];

            if(node == null) {
                continue;
            }

            if(sharePosList(node, visitedNodes) || node.char == "B") {
                continue;
            }

            node.h = heuristic(node, end);
            node.g = start.g + cost;
            node.f = node.g + node.h;
            openList.push(node);
        }

        openList.sort(function(a:Node, b:Node):Int {
            return b.f - a.f;
        });
        current = openList.pop();
        retracePath(start, current);
        visitedNodes.push(current);

        return current;
    }

    private static function retracePath(start:Node, node:Node):Void {
        switch(MathUtils.clamp(start.y - node.y, -1, 1)) {
            case -1:
                node.direction = DOWN;
            case 1:
                node.direction = UP;
        }

        switch(MathUtils.clamp(start.x - node.x, -1, 1)) {
            case -1:
                node.direction = RIGHT;
            case 1:
                node.direction = LEFT;
        }
    }

    private static function neighbors(current:Node, cost:Int, world:WorldMap):Array<Node> {
        var j = Std.int(MathUtils.clamp(current.y - cost, 1, world.height));
        var jl = Std.int(MathUtils.clamp(current.y + cost, 1, world.height));

        var n:Array<Node> = [];

        while(j <= jl) {
            var i = Std.int(MathUtils.clamp(current.x - cost, 0, world.width));
            var il = Std.int(MathUtils.clamp(current.x + cost, 0, world.width));

            while(i <= il) {
                if((i != current.x && j != current.y) || (i == current.x && j == current.y)) {
                    i++;
                    continue;
                }

                var node = new Node(i, j);
                node.char = world.matrix[j][i];
                n.push(node);
                
                i++;
            }
            
            j++;
        }

        return n;
    }

    public static function heuristic(start:Node, end:Node):Int {
        return Std.int(Math.abs(start.x - end.x) + Math.abs(start.y - end.y));
    }

    public static function clear():Void {
        visitedNodes = [];
    }

    private static function sharePosList(target:Node, list:Array<Node>):Bool {
        for(i in 0...list.length) {
            var node = list[i];

            if(node.x == target.x && node.y == target.y) {
                return true;
            }
        }

        return false;
    }
}