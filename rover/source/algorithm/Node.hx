package algorithm;

class Node {
    public var g:Int;
    public var h:Int;
    public var f:Int;
    public var x:Int;
    public var y:Int;
    public var parent:Node;
    public var direction:String;
    public var char:String;

    public function new(x:Int, y:Int) {
        this.x = x;
        this.y = y;
        this.g = 0;
        this.h = 0;
        this.f = 0;
        direction = "";
    }
}