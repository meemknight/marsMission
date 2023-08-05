package info;

#if (haxe_ver < "4.0") @:enum #else enum #end abstract Instructions(String) from String to String {
    public var UP = "u";
    public var DOWN = "d";
    public var LEFT = "l";
    public var RIGHT = "r";
    public var ATTACK = "a";
    public var SIGHT = "s"; // Upgrades sight
    public var RADER = "r"; // Upgrades rader
    public var BATTERY = "p"; // Upgrades battery
    public var HEAL = "h";
}