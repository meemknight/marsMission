package shop;

class Market {
    public var name(default, null):String;
    public var cmd(default, null):String;
    public var iron(default, null):Int;
    public var osmium(default, null):Int;
    public var level(default, null):Int;

    public var callback:Null<Void->Void>;

    public function new(name:String, iron:Int, osmium:Int, level:Int, cmd:String) {
        this.name = name;
        this.iron = iron;
        this.osmium = osmium;
        this.level = level;
        this.cmd = cmd;

        this.callback = null;
    }

    public function price():Int {
        return this.iron + this.osmium;
    }
}