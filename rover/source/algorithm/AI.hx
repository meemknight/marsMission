package algorithm;

import spinehaxe.MathUtils;
import info.Instructions;

class AI {
    private static var visitedNodes:Array<Node>;
    
    public static function findPath(cost:Int, start:Node, end:Node, world:WorldMap):Node {
        var openList = new PriorityQueue();
        var current:Node = null;
        visitedNodes = [];

        for(n in neighbors(start, cost, world)) {
            if(sharePosList(n, visitedNodes) || n.char != ".") {
                continue;
            }

            n.h = heuristic(n, end);
            n.g = start.g + cost;
            openList.addByF(n);
        }

        current = openList.pop();
        visitedNodes.push(current);
        return current;
    }

    private static function retracePath(node:Node):Array<Node> { // Using a LinkedList to get accurate path.
        var path:Array<Node> = [];

        while(node.parent != null) {
            var direction = "";

            switch(MathUtils.clamp(node.y - node.parent.y, -1, 1)) {
                case 1:
                    direction = DOWN;
                case -1:
                    direction = UP;
            }

            switch(MathUtils.clamp(node.x - node.parent.x, -1, 1)) {
                case 1:
                    direction = RIGHT;
                case -1:
                    direction = LEFT;
            }

            node.parent.direction = direction;
            path.insert(0, node);
            node = node.parent;
        }

        return path;
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
                node.char = world.matrix[i][j];
                n.push(node);
                
                i++;
            }
            
            j++;
        }

        return n;
    }

    private static function heuristic(start:Node, end:Node):Int {
        return Std.int(Math.abs(start.x - end.x) + Math.abs(start.y - end.y));
    }

    private static function sharePosList(target:Node, list:Array<Node>):Bool {
        for(node in list) {
            if(node.x == target.x && node.y == target.y) {
                return true;
            }
        }

        return false;
    }
}