package algorithm;

class Node {
    public var g:Int;
    public var h:Int;

    @:optional public var char:String;
    @:optional public var priority:Int;

    public var x:Int;
    public var y:Int;
    public var parent:Node;
    public var direction:String;

    public function new(x:Int, y:Int) {
        this.x = x;
        this.y = y;
        this.g = 0;
    }

    public function f(h:Int):Int {
        return g + h;
    }
}