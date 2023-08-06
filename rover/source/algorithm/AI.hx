package algorithm;

import spinehaxe.MathUtils;
import info.Instructions;

class AI {
    private static var openList:PriorityQueue<Node>;
    private static var visitedNodes:Array<Node>;
    
    public static function findPath(cost:Int, start:Node, end:Node, world:WorldMap):Array<String> {
        openList = new PriorityQueue<Node>(function(a:Node, b:Node) {
            return b.f(b.h) - a.f(a.h);
        });

        start.h = heuristic(start, end);
        openList.add(start);

        while(!openList.isEmpty()) {
            var current = openList.pop();

            if(current == end) {
                // return findPath(0, current, start, world); // Go back to base.
            }

            visitedNodes.insert(0, current);

            for(n in neighbors(current, cost, world)) {
                if(n.char == 'B' || visitedNodes.contains(n)) {
                    continue;
                }

                var total_g = current.g + n.g;

                if(!openList.contains(n) || total_g < n.g) {
                    n.parent = current;
                    n.g = total_g;
                    n.h = heuristic(n, end);

                    if(!openList.contains(n)) {
                        openList.add(n);
                    }
                }
            }
        }

        return null;
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
                    continue;
                }

                var node = new Node(i, j);
                node.char = world.matrix[i][j];
                node.priority = Player.priorities.get(node.char);
                node.g = cost;
                
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
}