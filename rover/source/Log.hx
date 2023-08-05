package;

class Log {
    public static function warning(message:String):Void {
        untyped __cpp__('printf("%s%s%s", "\\033[1m\\033[37m", "[WARN] ", "\\033[0m");');
        untyped __cpp__('printf("%s%s\\n", "\\033[1m\\033[33m", message.c_str());', message);
    }
}