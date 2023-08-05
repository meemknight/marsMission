package;

@:buildXml('<include name="../../Include.xml.tpl" />')
@:include("AStar.h")
@:keep
@:native("rover::AStar*")
extern class AStar {
    @:native("rover::AStar::init")
    public static function init():Void;

    @:native("rover::AStar::execute")
    public static function execute(x:Int, y:Int, width:Int, height:Int):Void;

    @:native("rover::AStar::storeCoolRock")
    public static function storeCoolRock(name:String, x:Int, y:Int):Void;

    @:native("rover::AStar::foundCoolRock")
    public static function foundCoolRock(name:String):Bool;
}